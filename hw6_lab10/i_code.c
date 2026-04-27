#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "i_code.h"
 
//LABEL COUNTER
static long label_counter = 0;
 
addr_t genlabel(void)
{
    addr_t a;
    a.region = R_LABEL;
    a.offset = label_counter++;
    return a;
}
 
//ADDRESS CONSTRUCTORS
addr_t addr_none(void)   { addr_t a; a.region=R_NONE;   a.offset=0; return a; }
addr_t addr_global(long o){ addr_t a; a.region=R_GLOBAL; a.offset=o; return a; }
addr_t addr_local(long o) { addr_t a; a.region=R_LOCAL;  a.offset=o; return a; }
addr_t addr_param(long o) { addr_t a; a.region=R_PARAM;  a.offset=o; return a; }
addr_t addr_const(long v) { addr_t a; a.region=R_CONST;  a.offset=v; return a; }
addr_t addr_string(long o){ addr_t a; a.region=R_STRING; a.offset=o; return a; }
addr_t addr_label(long n) { addr_t a; a.region=R_LABEL;  a.offset=n; return a; }
 
//Region/opcodes names for the output
const char *region_name(region_t r)
{
    switch (r) {
    case R_GLOBAL: return "global";
    case R_LOCAL:  return "loc";
    case R_PARAM:  return "param";
    case R_CONST:  return "const";
    case R_IMMED:  return "immed";
    case R_STRING: return "string";
    case R_LABEL:  return "L";
    default:       return "?";
    }
}
 
const char *opcode_name(opcode_t op)
{
    switch (op) {
    case OP_ASSIGN:   return "assign";
    case OP_ADDR:     return "addr";
    case OP_LOAD:     return "load";
    case OP_STORE:    return "store";
    case OP_ADD_INT:  return "add";
    case OP_SUB_INT:  return "sub";
    case OP_MUL_INT:  return "mul";
    case OP_DIV_INT:  return "div";
    case OP_MOD_INT:  return "mod";
    case OP_NEG_INT:  return "neg";
    case OP_ADD_FLT:  return "fadd";
    case OP_SUB_FLT:  return "fsub";
    case OP_MUL_FLT:  return "fmul";
    case OP_DIV_FLT:  return "fdiv";
    case OP_NEG_FLT:  return "fneg";
    case OP_EQ_INT:   return "eq";
    case OP_NEQ_INT:  return "neq";
    case OP_LT_INT:   return "lt";
    case OP_GT_INT:   return "gt";
    case OP_LTE_INT:  return "lte";
    case OP_GTE_INT:  return "gte";
    case OP_EQ_FLT:   return "feq";
    case OP_NEQ_FLT:  return "fneq";
    case OP_LT_FLT:   return "flt";
    case OP_GT_FLT:   return "fgt";
    case OP_LTE_FLT:  return "flte";
    case OP_GTE_FLT:  return "fgte";
    case OP_ADD_STR:  return "scat";
    case OP_EQ_STR:   return "seq";
    case OP_NEQ_STR:  return "sneq";
    case OP_NOT:      return "not";
    case OP_AND:      return "and";
    case OP_OR:       return "or";
    case OP_INCR:     return "incr";
    case OP_DECR:     return "decr";
    case OP_LABEL:    return "label";
    case OP_JUMP:     return "jump";
    case OP_JUMP_TRUE: return "jtrue";
    case OP_JUMP_FALSE:return "jfalse";
    case OP_PARM:     return "parm";
    case OP_CALL:     return "call";
    case OP_RETURN:   return "return";
    case OP_PROC:     return "proc";
    case OP_ENDPROC:  return "endproc";
    case OP_GLOBAL:   return "global";
    case OP_STRING:   return "string";
    default:          return "???";
    }
}
 
//print address into buffer
void addr_print(addr_t a, char *buf, int bufsz)
{
    if (a.region == R_NONE) {
        snprintf(buf, bufsz, "-");
    } else if (a.region == R_LABEL) {
        snprintf(buf, bufsz, "L%ld", a.offset);
    } else {
        snprintf(buf, bufsz, "%s:%ld", region_name(a.region), a.offset);
    }
}
 
//Instruction Constructors
instr_t *instr_new(opcode_t op, addr_t dst, addr_t src1, addr_t src2)
{
    instr_t *i = (instr_t *)calloc(1, sizeof(instr_t));
    i->op   = op;
    i->dst  = dst;
    i->src1 = src1;
    i->src2 = src2;
    i->next = NULL;
    return i;
}
 
instr_t *instr_call(addr_t dst, const char *name, int nargs)
{
    instr_t *i = instr_new(OP_CALL, dst, addr_none(), addr_none());
    i->name = name ? strdup(name) : NULL;
    i->ival = nargs;
    return i;
}
 
instr_t *instr_proc(const char *name, int frame_size)
{
    instr_t *i = instr_new(OP_PROC, addr_none(), addr_none(), addr_none());
    i->name = name ? strdup(name) : NULL;
    i->ival = frame_size;
    return i;
}
 
instr_t *instr_endproc(const char *name)
{
    instr_t *i = instr_new(OP_ENDPROC, addr_none(), addr_none(), addr_none());
    i->name = name ? strdup(name) : NULL;
    return i;
}
 
instr_t *instr_global_decl(const char *name, addr_t location)
{
    instr_t *i = instr_new(OP_GLOBAL, location, addr_none(), addr_none());
    i->name = name ? strdup(name) : NULL;
    return i;
}
 
instr_t *instr_string(long offset, const char *data)
{
    instr_t *i = instr_new(OP_STRING, addr_string(offset),
                            addr_none(), addr_none());
    i->sval = data ? strdup(data) : NULL;
    return i;
}
 
//LIST OPERATIONS
icode_list_t *icode_empty(void)
{
    icode_list_t *lst = (icode_list_t *)calloc(1, sizeof(icode_list_t));
    lst->head = lst->tail = NULL;
    lst->len  = 0;
    return lst;
}
 
icode_list_t *icode_new(instr_t *i)
{
    icode_list_t *lst = icode_empty();
    if (i) {
        lst->head = lst->tail = i;
        lst->len  = 1;
    }
    return lst;
}
 
void icode_append(icode_list_t *lst, instr_t *i)
{
    if (!lst || !i) return;
    if (!lst->tail) {
        lst->head = lst->tail = i;
    } else {
        lst->tail->next = i;
        lst->tail = i;
    }
    lst->len++;
}
 
//concat
icode_list_t *icode_concat(icode_list_t *a, icode_list_t *b)
{
    if (!a) return b;
    if (!b) return a;
    if (!b->head) { free(b); return a; }
    if (!a->head) {
        a->head = b->head;
        a->tail = b->tail;
        a->len  = b->len;
        free(b);
        return a;
    }
    a->tail->next = b->head;
    a->tail       = b->tail;
    a->len       += b->len;
    free(b);
    return a;
}
 
void icode_free(icode_list_t *lst)
{
    if (!lst) return;
    instr_t *i = lst->head;
    while (i) {
        instr_t *next = i->next;
        if (i->name) free(i->name);
        if (i->sval) free(i->sval);
        free(i);
        i = next;
    }
    free(lst);
}
 
//print single instruction to file
static void instr_print(FILE *f, instr_t *i)
{
    char d[64], s1[64], s2[64];
    addr_print(i->dst,  d,  sizeof d);
    addr_print(i->src1, s1, sizeof s1);
    addr_print(i->src2, s2, sizeof s2);
 
    switch (i->op) {
    case OP_PROC:
        fprintf(f, "%s:\n", i->name ? i->name : "???");
        return;
    case OP_ENDPROC:
        fprintf(f, "\t; end %s\n", i->name ? i->name : "???");
        return;
    case OP_LABEL:
        fprintf(f, "L%ld:\n", i->dst.offset);
        return;
    case OP_JUMP:
        fprintf(f, "\tjump\t%s\n", d);
        return;
    case OP_JUMP_TRUE:
        fprintf(f, "\tjtrue\t%s,%s\n", d, s1);
        return;
    case OP_JUMP_FALSE:
        fprintf(f, "\tjfalse\t%s,%s\n", d, s1);
        return;
    case OP_CALL:
        fprintf(f, "\tcall\t%s,%d,%s\n",
                i->name ? i->name : "???", i->ival, d);
        return;
    case OP_PARM:
        fprintf(f, "\tparm\t%s\n", s1);
        return;
    case OP_RETURN:
        if (i->src1.region == R_NONE)
            fprintf(f, "\treturn\n");
        else
            fprintf(f, "\treturn\t%s\n", s1);
        return;
    case OP_GLOBAL:
        fprintf(f, "\t.global\t%s,%s\n", i->name ? i->name : "?", d);
        return;
    case OP_STRING:
        /* printed in string segment separately */
        return;
    case OP_ASSIGN:
        fprintf(f, "\tassign\t%s,%s\n", d, s1);
        return;
    case OP_ADDR:
        fprintf(f, "\taddr\t%s,%s\n", d, s1);
        return;
    case OP_NEG_INT: case OP_NEG_FLT:
        fprintf(f, "\t%s\t%s,%s\n", opcode_name(i->op), d, s1);
        return;
    case OP_NOT:
        fprintf(f, "\tnot\t%s,%s\n", d, s1);
        return;
    case OP_INCR: case OP_DECR:
        fprintf(f, "\t%s\t%s\n", opcode_name(i->op), d);
        return;
    default:
        /* 3-address: dst = src1 op src2 */
        fprintf(f, "\t%s\t%s,%s,%s\n",
                opcode_name(i->op), d, s1, s2);
        return;
    }
}
 
//produce the .ic file
void icode_emit(icode_list_t *code, const char *out_filename,
                icode_list_t *string_seg, icode_list_t *data_seg)
{
    FILE *f = fopen(out_filename, "w");
    if (!f) { perror(out_filename); return; }
 
    /* .string region */
    if (string_seg && string_seg->head) {
        /* compute total byte size */
        long total = 0;
        for (instr_t *i = string_seg->head; i; i = i->next) {
            if (i->sval) total += (long)strlen(i->sval) + 1; /* +1 for NUL */
        }
        fprintf(f, ".string\t%ld\n", total);
        for (instr_t *i = string_seg->head; i; i = i->next) {
            if (!i->sval) continue;
            /* Print string, encoding non-printables as octal escapes
               (<= 16 raw bytes per line)                              */
            const char *s = i->sval;
            int col = 0;
            fprintf(f, "\t");
            while (*s) {
                unsigned char c = (unsigned char)*s++;
                if (c == '\n')       { fprintf(f, "\\012"); col += 4; }
                else if (c == '\t')  { fprintf(f, "\\011"); col += 4; }
                else if (c == '\r')  { fprintf(f, "\\015"); col += 4; }
                else if (c == '\\')  { fprintf(f, "\\\\");  col += 2; }
                else if (c < 32 || c > 126) {
                    fprintf(f, "\\%03o", c); col += 4;
                } else {
                    fputc(c, f); col++;
                }
                if (col >= 60 && *s) { fprintf(f, "\n\t"); col = 0; }
            }
            fprintf(f, "\\000\n");   
        }
    }
 
    /* .data region */
    if (data_seg && data_seg->head) {
        fprintf(f, ".data\n");
        for (instr_t *i = data_seg->head; i; i = i->next)
            instr_print(f, i);
    }
 
    /* .code region */
    fprintf(f, ".code\n");
    if (code) {
        for (instr_t *i = code->head; i; i = i->next)
            instr_print(f, i);
    }
 
    fclose(f);
}