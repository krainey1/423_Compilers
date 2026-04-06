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
        if (fname) insertsym(current, fname);

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
        if (name) insertsym(current, name);
        for (int i = 0; i < t->nkids; i++) collect_scopes(t->kids[i], current);
        return;
    }

    if (t->symbol && strcmp(t->symbol, "functionValueParameter") == 0) {
        const char *name = leaf_lexeme(t->kids[0]);
        if (name) insertsym(current, name);
        for (int i = 0; i < t->nkids; i++) collect_scopes(t->kids[i], current);
        return;
    }

    if (t->symbol && strcmp(t->symbol, "forStatement") == 0) {
        const char *name = leaf_lexeme(t->kids[2]);
        if (name) insertsym(current, name);
        for (int i = 0; i < t->nkids; i++) collect_scopes(t->kids[i], current);
        return;
    }

    for (int i = 0; i < t->nkids; i++) collect_scopes(t->kids[i], current);
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
    printf("\n========== Symbol Tables ==========\n");
    int scope_num = 0;
    for (ScopeNode *n = scope_list_head; n; n = n->next) {
        printsymtab(n->table, scope_num == 0 ? 0 : 1);
        scope_num++;
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
