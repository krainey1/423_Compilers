#ifndef SYMTAB_H
#define SYMTAB_H

struct typeinfo;
typedef struct typeinfo *typeptr;

// symbol table entry
typedef struct sym_entry {
    char *s;
    typeptr type;
    struct sym_entry *next;
} *SymbolTableEntry;

// hash table symbol table
typedef struct sym_table {
    int nBuckets;
    int nEntries;
    struct sym_table *parent;
    struct sym_entry **tbl;
    const char *name;
} *SymbolTable;

//needed func prototypes
SymbolTable mksymtab(int nbuckets, const char *name, SymbolTable parent);
SymbolTableEntry insertsym(SymbolTable st, const char *s);
SymbolTableEntry lookupsym(SymbolTable st, const char *s);
void printsymtab(SymbolTable st, int indent);
void freesymtab(SymbolTable st);

SymbolTableEntry lookup_current_scope(SymbolTable st, const char *s);

int hash(SymbolTable st, char *s);

#endif
