#ifndef SYMTAB_H
#define SYMTAB_H
 

typedef struct sym_entry {
    char *s;                    
    char *declared_type;        
    int   is_const;            
    struct sym_entry *next;     
} *SymbolTableEntry;
 

typedef struct sym_table {
    int nBuckets;               
    int nEntries;              
    struct sym_table *parent;   
    struct sym_entry **tbl;     
    const char *name;           
} *SymbolTable;
 

SymbolTable      mksymtab(int nbuckets, const char *name, SymbolTable parent);
void             freesymtab(SymbolTable st);
 

SymbolTableEntry insertsym(SymbolTable st, const char *s);
SymbolTableEntry lookupsym(SymbolTable st, const char *s);
SymbolTableEntry lookup_current_scope(SymbolTable st, const char *s);
 

void printsymtab(SymbolTable st);
 

int hash(SymbolTable st, char *s);
 
#endif