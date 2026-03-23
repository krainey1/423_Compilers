#include "symtab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//hash func
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

//constructor
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

//insert into sym table
SymbolTableEntry insertsym(SymbolTable st, const char *s)
{
    if (!st || !s) return NULL;

    int h = hash(st, (char *)s);


    for (SymbolTableEntry e = st->tbl[h]; e; e = e->next) {
        if (strcmp(e->s, s) == 0)
            return e;   /* already present, return existing entry */
    }

    SymbolTableEntry entry = (SymbolTableEntry)calloc(1, sizeof(struct sym_entry));
    if (!entry) { perror("insertsym"); exit(1); }

    entry->s    = strdup(s);
    entry->next = st->tbl[h];
    st->tbl[h]  = entry;
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

//print sym table
void printsymtab(SymbolTable st, int depth)
{
    if (!st) return;
 
    #define IND(d) do { for (int _i=0;_i<(d)*3;_i++) putchar(' '); } while(0)
 
    const char *kind = (st->parent == NULL) ? "GLOBAL" : "LOCAL ";
 
    IND(depth);
    printf("[%s]  scope: \"%s\"", kind, st->name);
    if (st->parent)
        printf("  (enclosing: \"%s\")", st->parent->name);
    printf("  %d entr%s\n", st->nEntries, st->nEntries == 1 ? "y" : "ies");
 
    if (st->nEntries == 0) {
        IND(depth);
        printf("         (no declarations)\n");
    } else {
        for (int i = 0; i < st->nBuckets; i++) {
            for (SymbolTableEntry e = st->tbl[i]; e; e = e->next) {
                IND(depth);
                printf("         bucket[%2d]  %s\n", i, e->s);
            }
        }
    }
    #undef IND
}

//freeing of course
void freesymtab(SymbolTable st)
{
    if (!st) return;
    for (int i = 0; i < st->nBuckets; i++) {
        SymbolTableEntry e = st->tbl[i];
        while (e) {
            SymbolTableEntry next = e->next;
            free(e->s);
            free(e);
            e = next;
        }
    }
    free(st->tbl);
    free(st);
}