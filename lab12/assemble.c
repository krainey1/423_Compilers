/*
 * assemble.c  --  x86-64 AT&T syntax (Linux) final code generator
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


/* Emit a formatted line with a leading tab */
static void emit(FILE *f, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    fprintf(f, "\t");
    vfprintf(f, fmt, ap);
    fprintf(f, "\n");
    va_end(ap);
}

/* Emit a label (no leading tab) */
static void emit_label(FILE *f, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(f, fmt, ap);
    fprintf(f, ":\n");
    va_end(ap);
}

/* Emit a comment */
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

/* -----------------------------------------------------------------------
 * String-pointer tracking:
 * We track which local/global slots hold char* values (string pointers)
 * so that println/print can decide between puts and printf.
 * We track by (region, offset) pairs.
 * ---------------------------------------------------------------------- */
#define STR_PTR_MAX 512
typedef struct {
    region_t region;
    long     offset;
} str_ptr_entry_t;

static str_ptr_entry_t str_ptr_table[STR_PTR_MAX];
static int  str_ptr_count = 0;

static void str_ptr_reset(void) { str_ptr_count = 0; }

static void str_ptr_record(region_t region, long offset)
{
    /* avoid duplicates */
    for (int i = 0; i < str_ptr_count; i++)
        if (str_ptr_table[i].region == region &&
            str_ptr_table[i].offset == offset)
            return;
    if (str_ptr_count < STR_PTR_MAX) {
        str_ptr_table[str_ptr_count].region = region;
        str_ptr_table[str_ptr_count].offset = offset;
        str_ptr_count++;
    }
}

static int str_ptr_is_string(region_t region, long offset)
{
    for (int i = 0; i < str_ptr_count; i++)
        if (str_ptr_table[i].region == region &&
            str_ptr_table[i].offset == offset)
            return 1;
    return 0;
}

/* Helper: is this addr a string pointer? */
static int addr_is_string(addr_t a)
{
    if (a.region == R_STRING) return 1;
    if (a.region == R_LOCAL || a.region == R_PARAM || a.region == R_GLOBAL)
        return str_ptr_is_string(a.region, a.offset);
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
        addr_t ops[3] = { i->dst, i->src1, i->src2 };
        for (int k = 0; k < 3; k++) {
            if (ops[k].region == R_LOCAL || ops[k].region == R_PARAM) {
                long end = ops[k].offset + 8;
                if (end > max_off) max_off = end;
            }
        }
    }
    if (max_off == 0) max_off = 8;
    /* Align to 16 */
    max_off = (max_off + 15) & ~15L;
    return max_off;
}

static int count_params(instr_t *proc_instr)
{
    /* We embed nparams in the PROC instruction's src1.offset */
    if (proc_instr && proc_instr->op == OP_PROC)
        return (int)proc_instr->src1.offset;
    return 0;
}

/* -----------------------------------------------------------------------
 * Pending PARM queue (up to MAX_PARMS args)
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
 * Load a single address into a given register string.
 */
static void load_to_reg(FILE *f, addr_t a, const char *reg)
{
    char slot[64];
    switch (a.region) {
    case R_LOCAL:
    case R_PARAM:
        rbp_offset(a, slot, sizeof slot);
        emit(f, "movq %s, %s", slot, reg);
        break;
    case R_CONST:
    case R_IMMED:
        emit(f, "movq $%ld, %s", a.offset, reg);
        break;
    case R_STRING:
        emit(f, "leaq __str%ld(%%rip), %s", a.offset, reg);
        break;
    case R_GLOBAL:
        emit(f, "movq __gv%ld(%%rip), %s", a.offset, reg);
        break;
    default:
        break;
    }
}

/*
 * Flush parms into argument registers before a CALL.
 * SysV: first arg -> %rdi, second -> %rsi, etc.
 * Returns 1 if any arg was a string.
 */
static int parm_flush(FILE *f)
{
    int has_string = 0;
    int n = parm_count < MAX_REG_ARGS ? parm_count : MAX_REG_ARGS;
    for (int k = 0; k < n; k++) {
        addr_t a = parm_queue[k];
        if (addr_is_string(a)) has_string = 1;
        load_to_reg(f, a, arg_regs[k]);
    }
    /* Stack args (beyond 6) pushed right-to-left */
    for (int k = parm_count - 1; k >= MAX_REG_ARGS; k--) {
        load_to_rax(f, parm_queue[k]);
        emit(f, "pushq %%rax");
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

        /* Spill parameter registers into their frame slots.
         * The nparams count is stored in src1.offset of the PROC instr. */
        int nparams = count_params(instr);
        /* Parameters are assigned offsets 0, 8, 16, ... by codegen.
         * Slot for param k: offset = k * 8, frame slot = -(k*8 + 8)(%rbp) */
        for (int k = 0; k < nparams && k < MAX_REG_ARGS; k++) {
            long off = (long)k * 8;
            char slot[64];
            addr_t pa;
            pa.region = R_LOCAL;
            pa.offset = off;
            rbp_offset(pa, slot, sizeof slot);
            emit(f, "movq %s, %s", arg_regs[k], slot);
            /* Mark these slots as potentially string pointers if reg was str */
            /* (We can't know statically; caller marks them via OP_ADDR)     */
        }
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
        /* Propagate string-pointer attribute through assignments */
        if (addr_is_string(instr->src1) &&
            (instr->dst.region == R_LOCAL || instr->dst.region == R_PARAM ||
             instr->dst.region == R_GLOBAL)) {
            str_ptr_record(instr->dst.region, instr->dst.offset);
        }
        break;

    /* ------------------------------------------------------------------ */
    /* addr: dst = &src1  -- for string pointers we load the address */
    case OP_ADDR:
        if (instr->src1.region == R_STRING) {
            emit(f, "leaq __str%ld(%%rip), %%rax", instr->src1.offset);
            if (instr->dst.region == R_LOCAL || instr->dst.region == R_PARAM ||
                instr->dst.region == R_GLOBAL)
                str_ptr_record(instr->dst.region, instr->dst.offset);
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
        /* Load divisor into %rcx before cqto trashes %rdx */
        load_to_rdx(f, instr->src2);
        emit(f, "movq %%rdx, %%rcx");
        load_to_rax(f, instr->src1);
        emit(f, "cqto");
        emit(f, "idivq %%rcx");
        store_from_rax(f, instr->dst);
        break;

    case OP_MOD_INT:
        load_to_rdx(f, instr->src2);
        emit(f, "movq %%rdx, %%rcx");
        load_to_rax(f, instr->src1);
        emit(f, "cqto");
        emit(f, "idivq %%rcx");
        emit(f, "movq %%rdx, %%rax");   /* remainder in %rdx */
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
     *     println(str)  -> puts(str)
     *     println(int)  -> printf with "%ld\n" format
     *     println()     -> puts("")
     *     print(str)    -> printf("%s", str)  (no newline)
     *     print(int)    -> printf("%ld", val)
     *     readLine()    -> ... (stub)
     */
    case OP_CALL: {
        const char *callee = instr->name ? instr->name : "unknown";

        int is_println = (strcmp(callee, "println") == 0);
        int is_print   = (strcmp(callee, "print")   == 0);

        if (is_println || is_print) {
            if (parm_count == 0) {
                /* println() with no args -> puts("") */
                if (is_println) {
                    emit(f, "leaq __empty_str(%%rip), %%rdi");
                    emit(f, "call puts@PLT");
                }
            } else {
                addr_t arg = parm_queue[0];
                parm_reset();

                int arg_is_str = addr_is_string(arg);

                if (arg_is_str) {
                    /* String pointer: load into %rdi */
                    if (arg.region == R_STRING) {
                        emit(f, "leaq __str%ld(%%rip), %%rdi", arg.offset);
                    } else {
                        load_to_reg(f, arg, "%rdi");
                    }
                    if (is_println)
                        emit(f, "call puts@PLT");
                    else {
                        emit(f, "xorl %%eax, %%eax");
                        emit(f, "call printf@PLT");
                    }
                } else {
                    /* Integer / boolean: use printf with format string */
                    if (is_println)
                        emit(f, "leaq __fmt_int_ln(%%rip), %%rdi");
                    else
                        emit(f, "leaq __fmt_int(%%rip), %%rdi");
                    load_to_reg(f, arg, "%rsi");
                    emit(f, "xorl %%eax, %%eax");
                    emit(f, "call printf@PLT");
                }
            }

            if (instr->dst.region != R_NONE)
                store_from_rax(f, instr->dst);

        } else {
            /* General function call: flush params into registers */
            parm_flush(f);
            emit(f, "xorl %%eax, %%eax");   /* clear AL (no SSE args) */
            emit(f, "call %s@PLT", callee);
            if (instr->dst.region != R_NONE)
                store_from_rax(f, instr->dst);
        }
        break;
    }

    /* ------------------------------------------------------------------ */
    /* Memory load */
    case OP_LOAD:
        load_to_rax(f, instr->src1);
        emit(f, "movq (%%rax), %%rax");
        store_from_rax(f, instr->dst);
        break;

    /* Memory store */
    case OP_STORE:
        load_to_rax(f, instr->src1);
        load_to_rdx(f, instr->dst);
        emit(f, "movq %%rax, (%%rdx)");
        break;

    /* ------------------------------------------------------------------ */
    /* Global declaration and string: handled in the section pass */
    case OP_GLOBAL:
    case OP_STRING:
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
 * Preserves the directory of source_file, replaces .kt extension with .s
 * ---------------------------------------------------------------------- */
char *make_asm_filename(const char *source_file)
{
    if (!source_file) return strdup("out.s");

    char *out = strdup(source_file);
    char *dot = strrchr(out, '.');
    if (dot && strcmp(dot, ".kt") == 0)
        strcpy(dot, ".s");
    else {
        char *tmp = malloc(strlen(out) + 3);
        sprintf(tmp, "%s.s", out);
        free(out);
        out = tmp;
    }
    return out;
}

/* -----------------------------------------------------------------------
 * asm_gen  --  top-level entry point
 * ---------------------------------------------------------------------- */
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
    /* Scan code to determine what format strings we need */
    int need_fmt_int   = 0;
    int need_empty_str = 0;

    if (code) {
        for (instr_t *i = code->head; i; i = i->next) {
            if (i->op == OP_CALL) {
                if (i->name &&
                    (strcmp(i->name, "println") == 0 ||
                     strcmp(i->name, "print")   == 0)) {
                    need_fmt_int   = 1;
                    need_empty_str = 1;
                }
            }
        }
    }

    /* ------------------------------------------------------------------ */
    /* .rodata: string literals */
    fprintf(f, "\t.section .rodata\n");

    if (str_seg) {
        for (instr_t *i = str_seg->head; i; i = i->next) {
            if (i->op != OP_STRING || !i->sval) continue;
            fprintf(f, "__str%ld:\n", i->dst.offset);
            fprintf(f, "\t.string \"");
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
    str_ptr_reset();

    if (code) {
        for (instr_t *i = code->head; i; i = i->next) {
            emit_instr(f, i);
        }
    }
    {
        /* Collect defined function names */
        const char *defined[128];
        int ndef = 0;
        /* Also known external symbols (libc) */
        const char *known_extern[] = {
            "println", "print", "puts", "printf", "scanf",
            "malloc", "free", "exit", "readLine", NULL
        };

        if (code) {
            for (instr_t *i = code->head; i; i = i->next) {
                if (i->op == OP_PROC && i->name && ndef < 128)
                    defined[ndef++] = i->name;
            }
        }

        /* Find calls to undefined functions */
        int stubs_emitted = 0;
        if (code) {
            for (instr_t *i = code->head; i; i = i->next) {
                if (i->op != OP_CALL || !i->name) continue;
                const char *cn = i->name;
                /* Check if it's a known extern */
                int is_known = 0;
                for (int k = 0; known_extern[k]; k++)
                    if (strcmp(cn, known_extern[k]) == 0) { is_known = 1; break; }
                if (is_known) continue;
                /* Check if defined in our .text */
                for (int k = 0; k < ndef; k++)
                    if (strcmp(cn, defined[k]) == 0) { is_known = 1; break; }
                if (is_known) continue;

                /* Emit a weak stub */
                if (!stubs_emitted) {
                    fprintf(f, "\t# --- runtime stubs for Kotlin builtins ---\n");
                    fprintf(f, "\t.text\n");
                    stubs_emitted = 1;
                }
                /* Check for duplicates */
                int already = 0;
                for (int k = 0; k < ndef; k++)
                    if (strcmp(cn, defined[k]) == 0) { already = 1; break; }
                if (already) continue;
                if (ndef < 128) defined[ndef++] = cn;

                fprintf(f, "\t.weak %s\n", cn);
                fprintf(f, "\t.type %s, @function\n", cn);
                fprintf(f, "%s:\n", cn);
                fprintf(f, "\tpushq %%rbp\n");
                fprintf(f, "\tmovq %%rsp, %%rbp\n");
                fprintf(f, "\txorl %%eax, %%eax\n");
                fprintf(f, "\tpopq %%rbp\n");
                fprintf(f, "\tret\n");
                fprintf(f, "\t.size %s, .-%s\n", cn, cn);
            }
        }
    }

    /* ------------------------------------------------------------------ */
    /* .note.GNU-stack: mark stack as non-executable */
    fprintf(f, "\t.section .note.GNU-stack,\"\",@progbits\n");

    fclose(f);
}