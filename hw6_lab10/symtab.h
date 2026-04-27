#ifndef SYMTAB_H
#define SYMTAB_H

//new for hw 5: more of an actual type system

//enumerate types
typedef enum {
    unknown = 0,
    int_type, 
    long_type, 
    double_type, 
    float_type, 
    bool_type, 
    char_type, 
    string_type, 
    array_type, 
    function_type,
    null_type, 
    unit_type, 
    any_type
} type_name;

//adjusting symbol table entries for typing
typedef struct sym_entry {
    char            *s;
    type_name        type;
    char            *declared_type;
    int              is_const;
    int              is_nullable;
    type_name        elem_type;
    type_name        ret_type;
    int              param_count;  //-1 unknown
    type_name       *param_types;
    struct sym_entry *next;
} *SymbolTableEntry;
 

typedef struct sym_table {
    int nBuckets;               
    int nEntries;              
    struct sym_table *parent;   
    struct sym_entry **tbl;     
    const char *name;           
} *SymbolTable;
 

SymbolTable mksymtab(int nbuckets, const char *name, SymbolTable parent);
void freesymtab(SymbolTable st);
 
SymbolTableEntry insertsym(SymbolTable st, const char *s);
SymbolTableEntry lookupsym(SymbolTable st, const char *s);
SymbolTableEntry lookup_current_scope(SymbolTable st, const char *s);
 
void printsymtab(SymbolTable st);
int hash(SymbolTable st, char *s);

const char *name(type_name t);
int types_compatible(type_name lhs, type_name rhs);
int is_numeric(type_name t);
 
#endif