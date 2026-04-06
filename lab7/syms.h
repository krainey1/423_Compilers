#ifndef SYMS_H
#define SYMS_H

#include "tree.h"
#include "symtab.h"

extern int g_semantic_errors;

void printsyms(struct tree *t);
SymbolTable buildsymtabs(struct tree *root, const char *filename);
void check_undeclared(struct tree *root, SymbolTable global);
void printsymtabs(void);
void freesymtabs(void);
void annotate_expr_types(struct tree *root, SymbolTable global);

#endif
