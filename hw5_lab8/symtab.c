#include "symtab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//hash
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

//make symbol table
SymbolTable mksymtab(int nbuckets, const char *name, SymbolTable parent)
{
    SymbolTable st = (SymbolTable)calloc(1, sizeof(struct sym_table));
    if (!st) { perror("mksymtab"); exit(1); }
    st->nBuckets = nbuckets;
    st->nEntries = 0;
    st->parent   = parent;
    st->name     = name ? name : "(anonymous)";
    st->tbl = (struct sym_entry **)calloc(nbuckets, sizeof(struct sym_entry *));
    if (!st->tbl) { perror("mksymtab tbl"); exit(1); }
    return st;
}

SymbolTableEntry insertsym(SymbolTable st, const char *s)
{
    if (!st || !s) return NULL;
    int h = hash(st, (char *)s);
    for (SymbolTableEntry e = st->tbl[h]; e; e = e->next)
        if (strcmp(e->s, s) == 0) return e;

    SymbolTableEntry entry = (SymbolTableEntry)calloc(1, sizeof(struct sym_entry));
    if (!entry) { perror("insertsym"); exit(1); }
    entry->s           = strdup(s);
    entry->type        = unknown;
    entry->declared_type = NULL;
    entry->is_const    = 0;
    entry->is_nullable = 0;
    entry->elem_type   = unknown;
    entry->ret_type    = unit_type;
    entry->param_count = -1;
    entry->param_types = NULL;
    entry->next        = st->tbl[h];
    st->tbl[h]         = entry;
    st->nEntries++;
    return entry;
}

SymbolTableEntry lookupsym(SymbolTable st, const char *s)
{
    while (st) {
        int h = hash(st, (char *)s);
        for (SymbolTableEntry e = st->tbl[h]; e; e = e->next)
            if (strcmp(e->s, s) == 0) return e;
        st = st->parent;
    }
    return NULL;
}

SymbolTableEntry lookup_current_scope(SymbolTable st, const char *s)
{
    if (!st || !s) return NULL;
    int h = hash(st, (char *)s);
    for (SymbolTableEntry e = st->tbl[h]; e; e = e->next)
        if (strcmp(e->s, s) == 0) return e;
    return NULL;
}

void printsymtab(SymbolTable st)
{
    if (!st) return;
    if (st->parent == NULL) return;   /* skip predefined */

    if (st->parent->parent == NULL)
        printf("--- symbol table for: package %s ---\n", st->name);
    else
        printf("--- symbol table for: func %s ---\n", st->name);

    for (int i = 0; i < st->nBuckets; i++) {
        for (SymbolTableEntry e = st->tbl[i]; e; e = e->next) {
            printf("    %-20s  type=%-10s  %s  %s\n",
                   e->s,
                   name(e->type),
                   e->is_const    ? "val" : "var",
                   e->is_nullable ? "nullable" : "non-null");
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
            free(e->param_types);
            free(e);
            e = next;
        }
    }
    free(st->tbl);
    free(st);
}

//helpers for typing

const char *name(type_name t)
{
    switch (t) {
    case unknown:  return "unknown";
    case int_type:      return "Int";
    case long_type:     return "Long";
    case double_type:   return "Double";
    case float_type:    return "Float";
    case bool_type:  return "Boolean";
    case char_type:     return "Char";
    case string_type:   return "String";
    case array_type:    return "Array";
    case function_type: return "Function";
    case unit_type:     return "Unit";
    case null_type:     return "null";
    case any_type:      return "Any";
    default:            return "?";
    }
}

int is_numeric(type_name t)
{
    return (t == int_type || t == long_type ||
            t == double_type || t == float_type);
}

// lhs = destination type, rhs = source type.
 //  Returns 1 if the assignment/operation is compatible. 
int types_compatible(type_name lhs, type_name rhs)
{
    if (lhs == any_type || rhs == any_type) return 1;
    if (lhs == unknown|| rhs == unknown) return 1; 
    if (lhs == rhs) return 1;
    if (lhs == long_type  && rhs == int_type)    return 1;
    if (lhs == float_type  && (rhs == int_type || rhs == long_type)) return 1;
    if (lhs == double_type && is_numeric(rhs))    return 1;
    return 0;
}
