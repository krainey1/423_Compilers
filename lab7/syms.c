#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tree.h"
#include "token.h"
#include "symtab.h"
#include "type.h"
#include "k0gram.tab.h"

/* number of hash buckets per scope */
#define NBUCKETS 23

/* semantic error counter */
int g_semantic_errors = 0;

static void printsymbol(const char *s)
{
    printf("%s\n", s);
    fflush(stdout);
}

void printsyms(struct tree *t)
{
    if (!t) return;

    if (t->nkids == 0) {
        if (t->leaf && t->leaf->code == IDENT && t->leaf->lexeme)
            printsymbol(t->leaf->lexeme);
        return;
    }

    for (int i = 0; i < t->nkids; i++)
        printsyms(t->kids[i]);
}

static const char *leaf_lexeme(struct tree *t)
{
    if (t && t->nkids == 0 && t->leaf && t->leaf->lexeme)
        return t->leaf->lexeme;
    return NULL;
}

static int is_decl_context(struct tree *parent, int kid_index)
{
    if (!parent || !parent->symbol) return 0;

    if (strcmp(parent->symbol, "varDeclaration") == 0 && kid_index == 1) return 1;
    if (strcmp(parent->symbol, "functionDeclaration") == 0 && kid_index == 1) return 1;
    if (strcmp(parent->symbol, "functionValueParameter") == 0 && kid_index == 0) return 1;
    if (strcmp(parent->symbol, "forStatement") == 0 && kid_index == 2) return 1;

    return 0;
}

typedef struct scope_node {
    SymbolTable table;
    struct scope_node *next;
} ScopeNode;

static ScopeNode *scope_list_head = NULL;
static ScopeNode *scope_list_tail = NULL;

static void scope_list_append(SymbolTable st)
{
    ScopeNode *n = (ScopeNode *)calloc(1, sizeof(ScopeNode));
    n->table = st;
    n->next  = NULL;

    if (!scope_list_tail) {
        scope_list_head = scope_list_tail = n;
    } else {
        scope_list_tail->next = n;
        scope_list_tail = n;
    }
}

static typeptr declared_type_of_var(struct tree *t)
{
    if (!t || !t->symbol || strcmp(t->symbol, "varDeclaration") != 0)
        return NULL;

    /*
      varDeclaration rules:
      1: VAL IDENT ASSIGNMENT expression
      2: VAL IDENT COLON type ASSIGNMENT expression
      3: VAR IDENT ASSIGNMENT expression
      4: VAR IDENT COLON type ASSIGNMENT expression
      5: VAR IDENT COLON type
    */

    if (t->prodrule == 2 || t->prodrule == 4 || t->prodrule == 5) {
        t->type = type_from_tree(t->kids[3]);
        return t->type;
    }

    /* no explicit annotation; keep broad for now */
    t->type = any_typeptr;
    return t->type;
}

static typeptr declared_type_of_param(struct tree *t)
{
    if (!t || !t->symbol || strcmp(t->symbol, "functionValueParameter") != 0)
        return NULL;

    /*
      functionValueParameter:
      1: IDENT COLON type
      2: IDENT COLON type ASSIGNMENT expression
    */
    t->type = type_from_tree(t->kids[2]);
    return t->type;
}

static typeptr declared_type_of_function(struct tree *t, SymbolTable fn_scope)
{
    if (!t || !t->symbol || strcmp(t->symbol, "functionDeclaration") != 0)
        return NULL;

    /*
      functionDeclaration:
      1: FUN IDENT LPAREN functionValueParameters RPAREN block
      2: FUN IDENT LPAREN functionValueParameters RPAREN COLON type block
      3: FUN IDENT LPAREN functionValueParameters RPAREN ASSIGNMENT expression
      4: FUN IDENT LPAREN functionValueParameters RPAREN COLON type ASSIGNMENT expression
    */

    struct tree *params = t->kids[3];
    struct tree *ret = NULL;

    if (t->prodrule == 2 || t->prodrule == 4)
        ret = t->kids[6];

    t->type = alcfunctype(ret, params, fn_scope, leaf_lexeme(t->kids[1]));
    return t->type;
}

static void install_predefined(SymbolTable global)
{
    SymbolTableEntry e;

    e = insertsym(global, "println");
    e->type = alcfunctype(NULL, NULL, NULL, "println");

    e = insertsym(global, "Int");
    e->type = integer_typeptr;

    e = insertsym(global, "String");
    e->type = string_typeptr;

    e = insertsym(global, "Array");
    e->type = any_typeptr;
}

static void collect_scopes(struct tree *t, SymbolTable current)
{
    if (!t) return;
    if (t->nkids == 0) return;

    if (t->symbol && strcmp(t->symbol, "functionDeclaration") == 0) {
        const char *fname = leaf_lexeme(t->kids[1]);
        SymbolTable fn_scope = mksymtab(NBUCKETS, fname ? fname : "fn", current);
        scope_list_append(fn_scope);

        if (fname) {
            if (lookup_current_scope(current, fname)) {
                fprintf(stderr, "%s: semantic error: redeclared variable %s\n",
                        current->name, fname);
                g_semantic_errors++;
            } else {
                SymbolTableEntry e = insertsym(current, fname);
                e->type = declared_type_of_function(t, fn_scope);
            }
        }

        for (int i = 0; i < t->nkids; i++)
            collect_scopes(t->kids[i], fn_scope);
        return;
    }

    if (t->symbol && strcmp(t->symbol, "varDeclaration") == 0) {
        const char *name = leaf_lexeme(t->kids[1]);
        if (name) {
            if (lookup_current_scope(current, name)) {
                fprintf(stderr, "%s: semantic error: redeclared variable %s\n",
                        current->name, name);
                g_semantic_errors++;
            } else {
                SymbolTableEntry e = insertsym(current, name);
                e->type = declared_type_of_var(t);
            }
        }
        for (int i = 0; i < t->nkids; i++)
            collect_scopes(t->kids[i], current);
        return;
    }

    if (t->symbol && strcmp(t->symbol, "functionValueParameter") == 0) {
        const char *name = leaf_lexeme(t->kids[0]);
        if (name) {
            if (lookup_current_scope(current, name)) {
                fprintf(stderr, "%s: semantic error: redeclared variable %s\n",
                        current->name, name);
                g_semantic_errors++;
            } else {
                SymbolTableEntry e = insertsym(current, name);
                e->type = declared_type_of_param(t);
            }
        }
        for (int i = 0; i < t->nkids; i++)
            collect_scopes(t->kids[i], current);
        return;
    }

    if (t->symbol && strcmp(t->symbol, "forStatement") == 0) {
        const char *name = leaf_lexeme(t->kids[2]);
        if (name) {
            if (lookup_current_scope(current, name)) {
                fprintf(stderr, "%s: semantic error: redeclared variable %s\n",
                        current->name, name);
                g_semantic_errors++;
            } else {
                SymbolTableEntry e = insertsym(current, name);
                e->type = any_typeptr;
            }
        }
        for (int i = 0; i < t->nkids; i++)
            collect_scopes(t->kids[i], current);
        return;
    }

    for (int i = 0; i < t->nkids; i++)
        collect_scopes(t->kids[i], current);
}

static void check_undeclared_rec(struct tree *t, struct tree *parent, int kid_index, SymbolTable current)
{
    if (!t) return;

    if (t->nkids == 0) {
        if (t->leaf && t->leaf->code == IDENT && t->leaf->lexeme) {
            if (!is_decl_context(parent, kid_index)) {
                if (!lookupsym(current, t->leaf->lexeme)) {
                    fprintf(stderr, "%s: semantic error: undeclared variable %s\n",
                            current->name, t->leaf->lexeme);
                    g_semantic_errors++;
                }
            }
        }
        return;
    }

    if (t->symbol && strcmp(t->symbol, "functionDeclaration") == 0) {
        const char *fname = leaf_lexeme(t->kids[1]);
        SymbolTable fn_scope = NULL;

        for (ScopeNode *n = scope_list_head; n; n = n->next) {
            if (n->table->parent == current &&
                fname && strcmp(n->table->name, fname) == 0) {
                fn_scope = n->table;
                break;
            }
        }
        if (!fn_scope) fn_scope = current;

        for (int i = 0; i < t->nkids; i++)
            check_undeclared_rec(t->kids[i], t, i, fn_scope);
        return;
    }

    for (int i = 0; i < t->nkids; i++)
        check_undeclared_rec(t->kids[i], t, i, current);
}

void check_undeclared(struct tree *root, SymbolTable global)
{
    check_undeclared_rec(root, NULL, -1, global);
}

static void annotate_expr_types_rec(struct tree *t, SymbolTable current)
{
    if (!t) return;

    if (t->symbol && strcmp(t->symbol, "functionDeclaration") == 0) {
        const char *fname = leaf_lexeme(t->kids[1]);
        SymbolTable fn_scope = NULL;

        for (ScopeNode *n = scope_list_head; n; n = n->next) {
            if (n->table->parent == current &&
                fname && strcmp(n->table->name, fname) == 0) {
                fn_scope = n->table;
                break;
            }
        }
        if (!fn_scope) fn_scope = current;

        for (int i = 0; i < t->nkids; i++)
            annotate_expr_types_rec(t->kids[i], fn_scope);
        return;
    }

    for (int i = 0; i < t->nkids; i++)
        annotate_expr_types_rec(t->kids[i], current);

    /* leaf nodes */
    if (t->nkids == 0 && t->leaf) {
        if (t->leaf->code == IDENT) {
            SymbolTableEntry e = lookupsym(current, t->leaf->lexeme);
            if (e && e->type)
                t->type = e->type;
            else
                t->type = any_typeptr;
        } else {
            t->type = type_from_token_code(t->leaf->code);
        }
        return;
    }

    if (t->symbol && strcmp(t->symbol, "type") == 0) {
        t->type = type_from_tree(t);
        return;
    }

    if (t->symbol && strcmp(t->symbol, "typeArgumentList") == 0) {
        t->type = type_from_tree(t);
        return;
    }

    if (t->symbol && strcmp(t->symbol, "binaryExpr") == 0) {
        typeptr l = t->kids[0] ? t->kids[0]->type : NULL;
        typeptr r = t->kids[2] ? t->kids[2]->type : NULL;

        /* arithmetic */
        if (t->prodrule >= 1 && t->prodrule <= 5) {
            if (l == float_typeptr || r == float_typeptr)
                t->type = float_typeptr;
            else if (l == integer_typeptr && r == integer_typeptr)
                t->type = integer_typeptr;
            else
                t->type = any_typeptr;
            return;
        }

        /* comparisons + logicals */
        t->type = boolean_typeptr;
        return;
    }

    if (t->symbol && strcmp(t->symbol, "unaryExpr") == 0) {
        if (t->prodrule == 2)   /* NOT expr */
            t->type = boolean_typeptr;
        else
            t->type = t->kids[1] ? t->kids[1]->type : any_typeptr;
        return;
    }

    if (t->symbol && strcmp(t->symbol, "postfixExpr") == 0) {
        t->type = t->kids[0] ? t->kids[0]->type : any_typeptr;
        return;
    }

    if (t->symbol && strcmp(t->symbol, "callExpr") == 0) {
        typeptr callee = t->kids[0] ? t->kids[0]->type : NULL;
        if (callee && callee->basetype == FUNC_TYPE && callee->u.f.returntype)
            t->type = callee->u.f.returntype;
        else
            t->type = any_typeptr;
        return;
    }

    if (t->symbol && strcmp(t->symbol, "castExpr") == 0) {
        t->type = type_from_tree(t->kids[2]);
        return;
    }

    if (t->symbol && strcmp(t->symbol, "indexExpr") == 0) {
        typeptr base = t->kids[0] ? t->kids[0]->type : NULL;
        if (base && base->basetype == ARRAY_TYPE)
            t->type = base->u.a.elemtype;
        else
            t->type = any_typeptr;
        return;
    }

    if (t->symbol && strcmp(t->symbol, "rangeExpr") == 0) {
        t->type = any_typeptr;
        return;
    }

    if (t->symbol && strcmp(t->symbol, "elvisExpr") == 0) {
        t->type = t->kids[0] ? t->kids[0]->type : any_typeptr;
        return;
    }

    if (t->symbol && strcmp(t->symbol, "literal") == 0) {
        if (t->nkids == 1 && t->kids[0])
            t->type = t->kids[0]->type;
        else
            t->type = any_typeptr;
        return;
    }

    if (t->symbol && strcmp(t->symbol, "varDeclaration") == 0) {
        t->type = declared_type_of_var(t);
        return;
    }

    if (t->symbol && strcmp(t->symbol, "functionValueParameter") == 0) {
        t->type = declared_type_of_param(t);
        return;
    }

    if (t->symbol && strcmp(t->symbol, "functionDeclaration") == 0) {
        /* handled above for scope, but fine as fallback */
        t->type = any_typeptr;
        return;
    }
}

void annotate_expr_types(struct tree *root, SymbolTable global)
{
    annotate_expr_types_rec(root, global);
}

SymbolTable buildsymtabs(struct tree *root, const char *filename)
{
    SymbolTable global = mksymtab(NBUCKETS, filename, NULL);
    scope_list_append(global);

    install_predefined(global);
    collect_scopes(root, global);

    return global;
}

void printsymtabs(void)
{
    if (g_semantic_errors > 0) return;

    printf("\n========== Symbol Tables ==========\n");
    for (ScopeNode *n = scope_list_head; n; n = n->next) {
        printsymtab(n->table, 0);
    }
    printf("===================================\n");
}

void freesymtabs(void)
{
    ScopeNode *n = scope_list_head;
    while (n) {
        ScopeNode *next = n->next;
        freesymtab(n->table);
        free(n);
        n = next;
    }
    scope_list_head = scope_list_tail = NULL;
}
