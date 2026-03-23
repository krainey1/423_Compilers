#ifndef SYMS_H
#define SYMS_H
 
#include "tree.h"
#include "symtab.h"
void printsyms(struct tree *t);
SymbolTable buildsymtabs(struct tree *root, const char *filename);
void printsymtabs(void);
void freesymtabs(void);
 
#endif 