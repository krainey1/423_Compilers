/*
 * asm_gen.c  --  x86-64 AT&T syntax (Linux) final code generator
 *
 * Translates the three-address intermediate code produced by codegen.c
 * into GNU/Linux x86-64 assembly that can be assembled and linked by gcc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "assemble.h"
#include "i_code.h"


// Emit a formatted line with a leading tab 
static void emit(FILE *f, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    fprintf(f, "\t");
    vfprintf(f, fmt, ap);
    fprintf(f, "\n");
    va_end(ap);
}

// Emit a label (no leading tab) 
static void emit_label(FILE *f, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(f, fmt, ap);
    fprintf(f, ":\n");
    va_end(ap);
}

// Emit a comment 
static void emit_comment(FILE *f, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    fprintf(f, "\t# ");
    vfprintf(f, fmt, ap);
    fprintf(f, "\n");
    va_end(ap);
}

/*
 * Convert a loc:N / param:N address to its rbp-relative offset string.
 * loc:0  -> -8(%rbp)
 * loc:8  -> -16(%rbp)
 * loc:N  -> -(N+8)(%rbp)
 *
 * Params are passed in registers; after the prologue we spill them into
 * the frame starting right after locals.  For simplicity we treat
 * param:N the same as loc:N here -- the caller already pushed them.
 */
static void rbp_offset(addr_t a, char *buf, int sz)
{
    long slot;
    if (a.region == R_LOCAL || a.region == R_PARAM)
        slot = -(a.offset + 8);
    else
        slot = 0;
    snprintf(buf, sz, "%ld(%%rbp)", slot);
}

/*
 * Load an addr_t into %rax.
 * Handles: R_LOCAL, R_PARAM, R_CONST, R_STRING (pointer), R_GLOBAL.
 */
static void load_to_rax(FILE *f, addr_t a)
{
    char slot[64];
    switch (a.region) {
    case R_LOCAL:
    case R_PARAM:
        rbp_offset(a, slot, sizeof slot);
        emit(f, "movq %s, %%rax", slot);
        break;
    case R_CONST:
    case R_IMMED:
        emit(f, "movq $%ld, %%rax", a.offset);
        break;
    case R_STRING:
        emit(f, "leaq __str%ld(%%rip), %%rax", a.offset);
        break;
    case R_GLOBAL:
        emit(f, "movq __gv%ld(%%rip), %%rax", a.offset);
        break;
    default:
        emit(f, "# load_to_rax: unhandled region %d", (int)a.region);
        break;
    }
}

/* Store %rax into a destination addr_t */
static void store_from_rax(FILE *f, addr_t dst)
{
    char slot[64];
    switch (dst.region) {
    case R_LOCAL:
    case R_PARAM:
        rbp_offset(dst, slot, sizeof slot);
        emit(f, "movq %%rax, %s", slot);
        break;
    case R_GLOBAL:
        emit(f, "movq %%rax, __gv%ld(%%rip)", dst.offset);
        break;
    default:
        emit(f, "# store_from_rax: unhandled region %d", (int)dst.region);
        break;
    }
}

/* Load addr_t into %rdx */
static void load_to_rdx(FILE *f, addr_t a)
{
    char slot[64];
    switch (a.region) {
    case R_LOCAL:
    case R_PARAM:
        rbp_offset(a, slot, sizeof slot);
        emit(f, "movq %s, %%rdx", slot);
        break;
    case R_CONST:
    case R_IMMED:
        emit(f, "movq $%ld, %%rdx", a.offset);
        break;
    case R_STRING:
        emit(f, "leaq __str%ld(%%rip), %%rdx", a.offset);
        break;
    case R_GLOBAL:
        emit(f, "movq __gv%ld(%%rip), %%rdx", a.offset);
        break;
    default:
        break;
    }
}

/* -----------------------------------------------------------------------
 * Argument register table (SysV AMD64)
 * ---------------------------------------------------------------------- */
static const char *arg_regs[] = {
    "%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"
};
#define MAX_REG_ARGS 6

#define STR_PTR_MAX 256
static long str_ptr_slots[STR_PTR_MAX];
static int  str_ptr_count = 0;

static void str_ptr_reset(void) { str_ptr_count = 0; }

static void str_ptr_record(long offset)
{
    if (str_ptr_count < STR_PTR_MAX)
        str_ptr_slots[str_ptr_count++] = offset;
}

static int str_ptr_is_string(long offset)
{
    for (int i = 0; i < str_ptr_count; i++)
        if (str_ptr_slots[i] == offset) return 1;
    return 0;
}

/* -----------------------------------------------------------------------
 * Per-function state
 * ---------------------------------------------------------------------- */
static long g_frame_size = 0;   /* bytes to reserve on the stack */

/*
 * Compute frame size: largest local offset seen in the instruction stream
 * for this function.  We do a quick scan between PROC and ENDPROC.
 */
static long compute_frame_size(instr_t *proc_instr)
{
    long max_off = 0;
    for (instr_t *i = proc_instr ? proc_instr->next : NULL; i; i = i->next) {
        if (i->op == OP_ENDPROC) break;
        /* Check all three operand slots */
        addr_t ops[3] = { i->dst, i->src1, i->src2 };
        for (int k = 0; k < 3; k++) {
            if (ops[k].region == R_LOCAL || ops[k].region == R_PARAM) {
                long end = ops[k].offset + 8;
                if (end > max_off) max_off = end;
            }
        }
    }
    /* Round up to 16-byte alignment: frame = max_off + 8 (saved rbp already
       pushed), then align total RSP to 16.  We just align max_off to 16. */
    if (max_off == 0) max_off = 8; /* at least one slot */
    /* Align to 16 */
    max_off = (max_off + 15) & ~15L;
    return max_off;
}

/* -----------------------------------------------------------------------
 * Pending PARM queue (up to 6 reg args)
 * ---------------------------------------------------------------------- */
#define MAX_PARMS 32
static addr_t parm_queue[MAX_PARMS];
static int    parm_count = 0;

static void parm_reset(void) { parm_count = 0; }

static void parm_push(addr_t a)
{
    if (parm_count < MAX_PARMS)
        parm_queue[parm_count++] = a;
}

/*
 * Flush parms into argument registers before a CALL.
 * Returns 1 if any arg was a string (so we know to use puts vs printf).
 */
static int parm_flush(FILE *f)
{
    int has_string = 0;
    int n = parm_count < MAX_REG_ARGS ? parm_count : MAX_REG_ARGS;
    for (int k = 0; k < n; k++) {
        addr_t a = parm_queue[k];
        if (a.region == R_STRING) has_string = 1;
        /* Load each parm into its arg register */
        char slot[64];
        switch (a.region) {
        case R_LOCAL:
        case R_PARAM:
            rbp_offset(a, slot, sizeof slot);
            emit(f, "movq %s, %s", slot, arg_regs[k]);
            break;
        case R_CONST:
        case R_IMMED:
            emit(f, "movq $%ld, %s", a.offset, arg_regs[k]);
            break;
        case R_STRING:
            emit(f, "leaq __str%ld(%%rip), %s", a.offset, arg_regs[k]);
            break;
        case R_GLOBAL:
            emit(f, "movq __gv%ld(%%rip), %s", a.offset, arg_regs[k]);
            break;
        default:
            break;
        }
    }
    parm_reset();
    return has_string;
}

/* -----------------------------------------------------------------------
 * Emit one IC instruction as x86-64 assembly
 * ---------------------------------------------------------------------- */
static void emit_instr(FILE *f, instr_t *instr)
{
    char dst_slot[64], src1_slot[64];

    switch (instr->op) {

    /* ------------------------------------------------------------------ */
    case OP_LABEL:
        fprintf(f, ".L%ld:\n", instr->dst.offset);
        break;

    /* ------------------------------------------------------------------ */
    case OP_PROC: {
        const char *fname = instr->name ? instr->name : "unknown";
        fprintf(f, "\t.globl %s\n", fname);
        fprintf(f, "\t.type %s, @function\n", fname);
        emit_label(f, "%s", fname);

        /* prologue */
        str_ptr_reset();
        g_frame_size = compute_frame_size(instr);
        emit(f, "pushq %%rbp");
        emit(f, "movq %%rsp, %%rbp");
        if (g_frame_size > 0)
            emit(f, "subq $%ld, %%rsp", g_frame_size);
        break;
    }

    /* ------------------------------------------------------------------ */
    case OP_ENDPROC:
        emit_comment(f, "epilogue");
        emit(f, "movq %%rbp, %%rsp");
        emit(f, "popq %%rbp");
        emit(f, "ret");
        fprintf(f, "\t.size %s, .-%s\n",
                instr->name ? instr->name : "unknown",
                instr->name ? instr->name : "unknown");
        break;

    /* ------------------------------------------------------------------ */
    case OP_RETURN:
        if (instr->src1.region != R_NONE) {
            load_to_rax(f, instr->src1);
        } else {
            emit(f, "xorl %%eax, %%eax");
        }
        emit(f, "movq %%rbp, %%rsp");
        emit(f, "popq %%rbp");
        emit(f, "ret");
        break;

    /* ------------------------------------------------------------------ */
    case OP_ASSIGN:
        load_to_rax(f, instr->src1);
        store_from_rax(f, instr->dst);
        break;

    /* ------------------------------------------------------------------ */
    /* addr: dst = &src1  -- for string pointers we load the address */
    case OP_ADDR:
        if (instr->src1.region == R_STRING) {
            emit(f, "leaq __str%ld(%%rip), %%rax", instr->src1.offset);
            /* record that dst slot holds a string pointer */
            if (instr->dst.region == R_LOCAL || instr->dst.region == R_PARAM)
                str_ptr_record(instr->dst.offset);
        } else if (instr->src1.region == R_LOCAL || instr->src1.region == R_PARAM) {
            rbp_offset(instr->src1, src1_slot, sizeof src1_slot);
            emit(f, "leaq %s, %%rax", src1_slot);
        } else if (instr->src1.region == R_GLOBAL) {
            emit(f, "leaq __gv%ld(%%rip), %%rax", instr->src1.offset);
        } else {
            load_to_rax(f, instr->src1);
        }
        store_from_rax(f, instr->dst);
        break;

    /* Integer arithmetic */
    case OP_ADD_INT:
        load_to_rax(f, instr->src1);
        load_to_rdx(f, instr->src2);
        emit(f, "addq %%rdx, %%rax");
        store_from_rax(f, instr->dst);
        break;

    case OP_SUB_INT:
        load_to_rax(f, instr->src1);
        load_to_rdx(f, instr->src2);
        emit(f, "subq %%rdx, %%rax");
        store_from_rax(f, instr->dst);
        break;

    case OP_MUL_INT:
        load_to_rax(f, instr->src1);
        load_to_rdx(f, instr->src2);
        emit(f, "imulq %%rdx, %%rax");
        store_from_rax(f, instr->dst);
        break;

    case OP_DIV_INT:
        /* Load divisor into %rcx first (before cqto trashes %rdx) */
        load_to_rdx(f, instr->src2);
        emit(f, "movq %%rdx, %%rcx");
        load_to_rax(f, instr->src1);
        emit(f, "cqto");              /* sign-extend %rax -> %rdx:%rax */
        emit(f, "idivq %%rcx");       /* %rax = quotient */
        store_from_rax(f, instr->dst);
        break;

    case OP_MOD_INT:
        load_to_rdx(f, instr->src2);
        emit(f, "movq %%rdx, %%rcx");
        load_to_rax(f, instr->src1);
        emit(f, "cqto");
        emit(f, "idivq %%rcx");
        emit(f, "movq %%rdx, %%rax"); /* remainder in %rdx */
        store_from_rax(f, instr->dst);
        break;

    case OP_NEG_INT:
        load_to_rax(f, instr->src1);
        emit(f, "negq %%rax");
        store_from_rax(f, instr->dst);
        break;

    /* ------------------------------------------------------------------ */
    /* Comparisons -> produce 0 or 1 in dst */
#define CMP_OP(setcc) \
        load_to_rax(f, instr->src1); \
        load_to_rdx(f, instr->src2); \
        emit(f, "cmpq %%rdx, %%rax"); \
        emit(f, "set" setcc " %%al"); \
        emit(f, "movzbq %%al, %%rax"); \
        store_from_rax(f, instr->dst); \
        break

    case OP_EQ_INT:  CMP_OP("e");
    case OP_NEQ_INT: CMP_OP("ne");
    case OP_LT_INT:  CMP_OP("l");
    case OP_GT_INT:  CMP_OP("g");
    case OP_LTE_INT: CMP_OP("le");
    case OP_GTE_INT: CMP_OP("ge");
#undef CMP_OP

    /* ------------------------------------------------------------------ */
    /* Logical */
    case OP_NOT:
        load_to_rax(f, instr->src1);
        emit(f, "testq %%rax, %%rax");
        emit(f, "sete %%al");
        emit(f, "movzbq %%al, %%rax");
        store_from_rax(f, instr->dst);
        break;

    case OP_AND:
        load_to_rax(f, instr->src1);
        load_to_rdx(f, instr->src2);
        emit(f, "testq %%rax, %%rax");
        emit(f, "setne %%al");
        emit(f, "testq %%rdx, %%rdx");
        emit(f, "setne %%dl");
        emit(f, "andb %%dl, %%al");
        emit(f, "movzbq %%al, %%rax");
        store_from_rax(f, instr->dst);
        break;

    case OP_OR:
        load_to_rax(f, instr->src1);
        load_to_rdx(f, instr->src2);
        emit(f, "orq %%rdx, %%rax");
        emit(f, "setne %%al");
        emit(f, "movzbq %%al, %%rax");
        store_from_rax(f, instr->dst);
        break;

    /* ------------------------------------------------------------------ */
    /* Increment / Decrement */
    case OP_INCR:
        if (instr->dst.region == R_LOCAL || instr->dst.region == R_PARAM) {
            rbp_offset(instr->dst, dst_slot, sizeof dst_slot);
            emit(f, "addq $1, %s", dst_slot);
        } else if (instr->dst.region == R_GLOBAL) {
            emit(f, "addq $1, __gv%ld(%%rip)", instr->dst.offset);
        }
        break;

    case OP_DECR:
        if (instr->dst.region == R_LOCAL || instr->dst.region == R_PARAM) {
            rbp_offset(instr->dst, dst_slot, sizeof dst_slot);
            emit(f, "subq $1, %s", dst_slot);
        } else if (instr->dst.region == R_GLOBAL) {
            emit(f, "subq $1, __gv%ld(%%rip)", instr->dst.offset);
        }
        break;

    /* ------------------------------------------------------------------ */
    /* Control flow */
    case OP_JUMP:
        emit(f, "jmp .L%ld", instr->dst.offset);
        break;

    case OP_JUMP_TRUE:
        /* JUMP_TRUE label, cond  (dst=label, src1=cond) */
        load_to_rax(f, instr->src1);
        emit(f, "testq %%rax, %%rax");
        emit(f, "jne .L%ld", instr->dst.offset);
        break;

    case OP_JUMP_FALSE:
        load_to_rax(f, instr->src1);
        emit(f, "testq %%rax, %%rax");
        emit(f, "je .L%ld", instr->dst.offset);
        break;

    /* ------------------------------------------------------------------ */
    /* Parameter passing: queue the argument */
    case OP_PARM:
        parm_push(instr->src1);
        break;

    /* ------------------------------------------------------------------ */
    /*
     * CALL:
     *   Flush queued parms into registers, then emit call.
     *
     *   Built-in mapping (Kotlin -> libc):
     *     println(str) -> puts(str)
     *     println(int) -> printf with "%ld\n" format
     *     print(str)   -> printf(str)    (no newline)
     */
    case OP_CALL: {
        const char *callee = instr->name ? instr->name : "unknown";

        /* Is this println / print? */
        int is_println = (strcmp(callee, "println") == 0);
        int is_print   = (strcmp(callee, "print")   == 0);

        if (is_println || is_print) {
            /*
             * peek at what is queued:
             *  - 0 args  -> puts("")
             *  - 1 string arg -> puts(str)  [println]  / printf("%s",str) [print]
             *  - 1 non-string -> printf("%ld\n", val)
             */
            if (parm_count == 0) {
                if (is_println) {
                    emit(f, "leaq __empty_str(%%rip), %%rdi");
                    emit(f, "call puts@PLT");
                }
            } else {
                addr_t arg = parm_queue[0];
                parm_reset();

                if (arg.region == R_STRING) {
                    /* String literal: use puts for println, printf for print */
                    emit(f, "leaq __str%ld(%%rip), %%rdi", arg.offset);
                    if (is_println)
                        emit(f, "call puts@PLT");
                    else {
                        emit(f, "xorl %%eax, %%eax");
                        emit(f, "call printf@PLT");
                    }
                } else if (arg.region == R_LOCAL || arg.region == R_PARAM) {
                    /*
                     * Check if this local slot holds a string pointer
                     * (set by a preceding OP_ADDR dst=loc:T, src1=string:S).
                     * If so, call puts/printf with it as a char*.
                     * Otherwise treat it as an integer and use printf.
                     */
                    rbp_offset(arg, src1_slot, sizeof src1_slot);
                    if (str_ptr_is_string(arg.offset)) {
                        /* String pointer path */
                        emit(f, "movq %s, %%rdi", src1_slot);
                        if (is_println)
                            emit(f, "call puts@PLT");
                        else {
                            emit(f, "xorl %%eax, %%eax");
                            emit(f, "call printf@PLT");
                        }
                    } else {
                        /* Integer path */
                        if (is_println)
                            emit(f, "leaq __fmt_int_ln(%%rip), %%rdi");
                        else
                            emit(f, "leaq __fmt_int(%%rip), %%rdi");
                        emit(f, "movq %s, %%rsi", src1_slot);
                        emit(f, "xorl %%eax, %%eax");
                        emit(f, "call printf@PLT");
                    }
                } else {
                    /* Integer / constant: use printf with format string */
                    if (is_println)
                        emit(f, "leaq __fmt_int_ln(%%rip), %%rdi");
                    else
                        emit(f, "leaq __fmt_int(%%rip), %%rdi");

                    switch (arg.region) {
                    case R_CONST:
                    case R_IMMED:
                        emit(f, "movq $%ld, %%rsi", arg.offset);
                        break;
                    case R_GLOBAL:
                        emit(f, "movq __gv%ld(%%rip), %%rsi", arg.offset);
                        break;
                    default:
                        load_to_rax(f, arg);
                        emit(f, "movq %%rax, %%rsi");
                        break;
                    }
                    emit(f, "xorl %%eax, %%eax");
                    emit(f, "call printf@PLT");
                }
            }

            /* Store return value if destination is not R_NONE */
            if (instr->dst.region != R_NONE)
                store_from_rax(f, instr->dst);

        } else {
            /* General function call */
            parm_flush(f);
            emit(f, "xorl %%eax, %%eax");   /* clear AL (no SSE args) */
            emit(f, "call %s@PLT", callee);
            if (instr->dst.region != R_NONE)
                store_from_rax(f, instr->dst);
        }
        break;
    }

    /* ------------------------------------------------------------------ */
    /* Global declaration: handled separately in the .bss section */
    case OP_GLOBAL:
    case OP_STRING:
        /* emitted in the rodata/bss pass, not here */
        break;

    /* ------------------------------------------------------------------ */
    default:
        emit_comment(f, "TODO: opcode %d (%s)", (int)instr->op,
                     opcode_name(instr->op));
        break;
    }
}

/* -----------------------------------------------------------------------
 * make_asm_filename
 * ---------------------------------------------------------------------- */
char *make_asm_filename(const char *source_file)
{
    if (!source_file) return strdup("out.s");

    const char *base = strrchr(source_file, '/');
    base = base ? base + 1 : source_file;

    char *out = strdup(base);
    char *dot = strrchr(out, '.');
    if (dot) strcpy(dot, ".s");
    else {
        char *tmp = malloc(strlen(out) + 3);
        sprintf(tmp, "%s.s", out);
        free(out);
        out = tmp;
    }
    return out;
}

void asm_gen(icode_list_t *code,
             icode_list_t *str_seg,
             icode_list_t *dat_seg,
             const char   *out_filename)
{
    FILE *f = fopen(out_filename, "w");
    if (!f) { perror(out_filename); return; }

    /* ------------------------------------------------------------------ */
    /* File header */
    fprintf(f, "\t.file \"%s\"\n", out_filename);
    fprintf(f, "\t# k0 compiler -- x86-64 AT&T Linux\n\n");

    /* ------------------------------------------------------------------ */
    /* .rodata: string literals */
    int has_strings    = str_seg && str_seg->head;
    int need_fmt_int   = 0;   /* set if any int println is encountered */
    int need_empty_str = 0;   /* set if 0-arg println is encountered   */

    /* Scan for format string needs */
    if (code) {
        for (instr_t *i = code->head; i; i = i->next) {
            if (i->op == OP_CALL) {
                if (i->name && (strcmp(i->name, "println") == 0 ||
                                strcmp(i->name, "print")   == 0)) {
                    need_fmt_int   = 1;
                    need_empty_str = 1;
                }
            }
        }
    }

    fprintf(f, "\t.section .rodata\n");

    /* Emit each string literal as __str<byte_offset>: */
    if (has_strings) {
        for (instr_t *i = str_seg->head; i; i = i->next) {
            if (i->op != OP_STRING || !i->sval) continue;
            fprintf(f, "__str%ld:\n", i->dst.offset);
            fprintf(f, "\t.string \"");
            /* Re-encode the string: sval has already been unquoted by
               add_string_const; we need to escape it for .string */
            for (const char *p = i->sval; *p; p++) {
                unsigned char c = (unsigned char)*p;
                if      (c == '"')  fprintf(f, "\\\"");
                else if (c == '\\') fprintf(f, "\\\\");
                else if (c == '\n') fprintf(f, "\\n");
                else if (c == '\t') fprintf(f, "\\t");
                else if (c == '\r') fprintf(f, "\\r");
                else if (c < 32 || c > 126)
                    fprintf(f, "\\%03o", c);
                else
                    fputc(c, f);
            }
            fprintf(f, "\"\n");
        }
    }

    /* Format strings for printf-based println */
    if (need_fmt_int) {
        fprintf(f, "__fmt_int_ln:\n\t.string \"%%ld\\n\"\n");
        fprintf(f, "__fmt_int:\n\t.string \"%%ld\"\n");
    }
    if (need_empty_str) {
        fprintf(f, "__empty_str:\n\t.string \"\"\n");
    }
    fprintf(f, "\n");

    /* ------------------------------------------------------------------ */
    /* .bss: global variables (8-byte each) */
    if (dat_seg && dat_seg->head) {
        fprintf(f, "\t.bss\n");
        for (instr_t *i = dat_seg->head; i; i = i->next) {
            if (i->op != OP_GLOBAL) continue;
            /* Use __gv<offset> as the asm symbol, but also expose the
               programmer name via a .set alias if we have it. */
            fprintf(f, "\t.globl __gv%ld\n", i->dst.offset);
            fprintf(f, "\t.align 8\n");
            fprintf(f, "__gv%ld:\n", i->dst.offset);
            if (i->name)
                fprintf(f, "\t# global var '%s'\n", i->name);
            fprintf(f, "\t.zero 8\n");
        }
        fprintf(f, "\n");
    }

    /* ------------------------------------------------------------------ */
    /* .text: function bodies */
    fprintf(f, "\t.text\n\n");

    parm_reset();
    g_frame_size = 0;

    if (code) {
        for (instr_t *i = code->head; i; i = i->next) {
            emit_instr(f, i);
        }
    }

    /* ------------------------------------------------------------------ */
    /* main entrypoint wrapper:
       If the source defines 'main', it is already emitted.
       We need _start only when not linking with gcc (we ARE using gcc,
       so the CRT provides _start -> calls main).  Nothing extra needed. */

    /* ------------------------------------------------------------------ */
    /* .note.GNU-stack: mark stack as non-executable (silences ld warning) */
    fprintf(f, "\t.section .note.GNU-stack,\"\",@progbits\n");

    fclose(f);
}