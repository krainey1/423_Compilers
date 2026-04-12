#include "symtab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int hash(SymbolTable st, char *s)
{
    register int h = 0;
    register char c;
    while ((c = *s++)) {
        h += c & 0377;
        h *= 37;
    }
    if (h < 0) h = -h;
    return h % st->nBuckets;
}


SymbolTable mksymtab(int nbuckets, const char *name, SymbolTable parent)
{
    SymbolTable st = (SymbolTable)calloc(1, sizeof(struct sym_table));
    if (!st) { perror("mksymtab"); exit(1); }

    st->nBuckets = nbuckets;
    st->nEntries = 0;
    st->parent   = parent;
    st->name     = name ? name : "(anonymous)";
    st->tbl      = (struct sym_entry **)calloc(nbuckets, sizeof(struct sym_entry *));
    if (!st->tbl) { perror("mksymtab tbl"); exit(1); }

    return st;
}


SymbolTableEntry insertsym(SymbolTable st, const char *s)
{
    if (!st || !s) return NULL;

    int h = hash(st, (char *)s);

    for (SymbolTableEntry e = st->tbl[h]; e; e = e->next) {
        if (strcmp(e->s, s) == 0)
            return e;
    }

    SymbolTableEntry entry = (SymbolTableEntry)calloc(1, sizeof(struct sym_entry));
    if (!entry) { perror("insertsym"); exit(1); }

    entry->s             = strdup(s);
    entry->declared_type = NULL;   
    entry->is_const      = 0;
    entry->next          = st->tbl[h];
    st->tbl[h]           = entry;
    st->nEntries++;

    return entry;
}


SymbolTableEntry lookupsym(SymbolTable st, const char *s)
{
    while (st) {
        int h = hash(st, (char *)s);
        for (SymbolTableEntry e = st->tbl[h]; e; e = e->next) {
            if (strcmp(e->s, s) == 0)
                return e;
        }
        st = st->parent;
    }
    return NULL;
}


void printsymtab(SymbolTable st)
{
    if (!st) return;

    /* predefined scope: skip silently */
    if (st->parent == NULL)
        return;

    
    if (st->parent->parent == NULL)
        printf("--- symbol table for: package %s ---\n", st->name);
    else
        printf("--- symbol table for: func %s ---\n", st->name);

    for (int i = 0; i < st->nBuckets; i++) {
        for (SymbolTableEntry e = st->tbl[i]; e; e = e->next) {
            printf("    %s\n", e->s);
        }
    }
    printf("---\n");
}


void freesymtab(SymbolTable st)
{
    if (!st) return;
    for (int i = 0; i < st->nBuckets; i++) {
        SymbolTableEntry e = st->tbl[i];
        while (e) {
            SymbolTableEntry next = e->next;
            free(e->s);
            free(e->declared_type);
            free(e);
            e = next;
        }
    }
    free(st->tbl);
    free(st);
}

SymbolTableEntry lookup_current_scope(SymbolTable st, const char *s)
{
    if (!st || !s) return NULL;
    int h = hash(st, (char *)s);
    for (SymbolTableEntry e = st->tbl[h]; e; e = e->next) {
        if (strcmp(e->s, s) == 0)
            return e;
    }
    return NULL;
}