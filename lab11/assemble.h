#ifndef ASM_GEN_H
#define ASM_GEN_H
 
#include "i_code.h"
// in which we get the .s settled
char *make_asm_filename(const char *source_file);
 
/*
 * asm_gen:
 *   Translate the three-address IC lists into x86-64 AT&T assembly and
 *   write to out_filename.  Immediately after writing, compile and run
 *   the result so the user sees the program's output.
 */
void asm_gen(icode_list_t *code,
             icode_list_t *str_seg,
             icode_list_t *dat_seg,
             const char   *out_filename);
 
#endif 