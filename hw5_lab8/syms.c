#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tree.h"
#include "token.h"
#include "symtab.h"
#include "k0gram.tab.h"

/* number of hash buckets per scope */
#define NBUCKETS 23

/* semantic error counter */
int g_semantic_errors = 0;


void printsyms(struct tree *t)
{
    if (!t) return;
    if (t->nkids == 0) {
        if (t->leaf && t->leaf->code == IDENT && t->leaf->lexeme)
            printf("%s\n", t->leaf->lexeme);
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


static SymbolTable g_predefined = NULL;

static SymbolTable make_predefined_scope(void)
{
    /* parent == NULL marks this as the root of the chain */
    SymbolTable pre = mksymtab(NBUCKETS, "predefined", NULL);

    /* built-in types */
    insertsym(pre, "Int");
    insertsym(pre, "Long");
    insertsym(pre, "Double");
    insertsym(pre, "Float");
    insertsym(pre, "Boolean");
    insertsym(pre, "Char");
    insertsym(pre, "String");
    insertsym(pre, "Array");

    /* built-in I/O functions */
    insertsym(pre, "print");
    insertsym(pre, "println");
    insertsym(pre, "readLine");

    return pre;
}


static void collect_scopes(struct tree *t, SymbolTable current)
{
    if (!t) return;
    if (t->nkids == 0) return;

    /* --- function declaration ------------------------------------------ */
    if (t->symbol && strcmp(t->symbol, "functionDeclaration") == 0) {
        const char *fname = leaf_lexeme(t->kids[1]);

        /* Insert function name into the enclosing (package or outer fn) scope */
        if (fname) {
            if (lookup_current_scope(current, fname)) {
                fprintf(stderr, "%s: semantic error: redeclared variable %s\n",
                        current->name, fname);
                g_semantic_errors++;
            } else {
                insertsym(current, fname);
            }
        }

        /* Create a new scope for this function's parameters and locals */
        SymbolTable fn_scope = mksymtab(NBUCKETS, fname ? fname : "fn", current);
        scope_list_append(fn_scope);

        /* Recurse into all children using the function scope */
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
                /* mark val declarations as const */
                if (e && t->kids[0] && t->kids[0]->leaf &&
                    strcmp(t->kids[0]->leaf->lexeme, "val") == 0)
                    e->is_const = 1;
            }
        }
        for (int i = 0; i < t->nkids; i++) collect_scopes(t->kids[i], current);
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
                insertsym(current, name);
            }
        }
        for (int i = 0; i < t->nkids; i++) collect_scopes(t->kids[i], current);
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
                insertsym(current, name);
            }
        }
        for (int i = 0; i < t->nkids; i++) collect_scopes(t->kids[i], current);
        return;
    }


    for (int i = 0; i < t->nkids; i++) collect_scopes(t->kids[i], current);
}


static void check_undeclared_rec(struct tree *t, struct tree *parent,
                                 int kid_index, SymbolTable current)
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

    /* Switch into a function's own scope when descending into it */
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


SymbolTable buildsymtabs(struct tree *root, const char *filename)
{
    (void)filename; 

  
    g_predefined = make_predefined_scope();
   

   
    SymbolTable global = mksymtab(NBUCKETS, "main", g_predefined);
    scope_list_append(global);

  
    collect_scopes(root, global);

    return global;
}


void printsymtabs(void)
{
    if (g_semantic_errors > 0) return;

    for (ScopeNode *n = scope_list_head; n; n = n->next)
        printsymtab(n->table);
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

    /* free predefined scope separately (not in scope_list) */
    if (g_predefined) {
        freesymtab(g_predefined);
        g_predefined = NULL;
    }
}