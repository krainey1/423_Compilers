#ifndef CODEGEN_H
#define CODEGEN_H
 
#include "tree.h"
#include "symtab.h"
#include "i_code.h"
 
/*
 * Call sequence from main():
 *
 *   1. assign_offsets(global)          -- walk every symbol table and assign
 *                                         memory addresses to each variable
 *   2. assign_first(root)              -- bottom-up: give each control-flow
 *                                         node a "first" label
 *   3. assign_follow(root, end_label)  -- top-down: propagate "follow" labels
 *   4. assign_bool_labels(root)        -- top-down: propagate onTrue/onFalse
 *   5. ic = codegen(root, global)      -- synthesize code attribute
 *   6. icode_emit(ic, "foo.ic", ...)   -- write the .ic file
 */
 
//add addresses to symboltable entries
long assign_offsets(SymbolTable global);
 
//label traversals for lab 10
void assign_first(struct tree *t);
void assign_follow(struct tree *t, addr_t follow);
void assign_bool_labels(struct tree *t);
 
//codegen traversal
icode_list_t *codegen(struct tree *root, SymbolTable global,
                       icode_list_t **str_seg, icode_list_t **dat_seg);
 
//make the output filename based on src
char *make_ic_filename(const char *source_file);
 
#endif 