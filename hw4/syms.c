#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tree.h"
#include "token.h"
#include "symtab.h"
#include "k0gram.tab.h"



//number of hash buckets per scope
#define NBUCKETS 23

///semantic error counter
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
        /* Leaf node */
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

static void install_predefined(SymbolTable global)
{
    insertsym(global, "println");
    insertsym(global, "Int");
    insertsym(global, "String");
    insertsym(global, "Array");
}


static void collect_scopes(struct tree *t, SymbolTable current)
{
    if (!t) return;
    if (t->nkids == 0) return;

    if (t->symbol && strcmp(t->symbol, "functionDeclaration") == 0) {
        const char *fname = leaf_lexeme(t->kids[1]);
        SymbolTable fn_scope = mksymtab(NBUCKETS, fname ? fname : "fn", current);
        scope_list_append(fn_scope);

        /* Insert the function name into the parent scope */
        if (fname) {
            if (lookup_current_scope(current, fname)) {
                fprintf(stderr, "%s: semantic error: redeclared variable %s\n",
                        current->name, fname);
                g_semantic_errors++;
            } else {
                insertsym(current, fname);
            }
        }

        for (int i = 0; i < t->nkids; i++) {
            if (t->symbol && strcmp(t->symbol, "functionValueParameter") == 0)
                collect_scopes(t->kids[i], fn_scope);
            else
                collect_scopes(t->kids[i], fn_scope);
        }
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
                insertsym(current, name);
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


SymbolTable buildsymtabs(struct tree *root, const char *filename)
{
    /* Create the global/file-level scope */
    SymbolTable global = mksymtab(NBUCKETS, filename, NULL);
    scope_list_append(global);

    install_predefined(global);

    /* Populate: insert declarations, create function child scopes */
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
