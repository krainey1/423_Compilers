#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "tree.h"
#include "token.h"
#include "symtab.h"
#include "i_code.h"
#include "codegen.h"
#include "k0gram.tab.h"


#define WORD_SIZE 8L   /* every variable / temp is 8 bytes */

typedef struct cg_attrs {
    addr_t        place;      /* synthesised .place                     */
    icode_list_t *code;       /* synthesised .code                      */

    /* Lab 10 label attributes */
    addr_t        first;      /* entry label (synthesised, post-order)  */
    addr_t        follow;     /* successor label (inherited, pre-order) */
    addr_t        on_true;    /* branch-on-true target  (inherited)     */
    addr_t        on_false;   

    int           first_set;
    int           follow_set;
    int           on_true_set;
    int           on_false_set;
} cg_attrs;

// Simple open-address hash map: tree* -> cg_attrs* 
#define ATTR_BUCKETS 4096
typedef struct attr_entry {
    struct tree *key;
    cg_attrs    *val;
    struct attr_entry *next;
} attr_entry_t;

static attr_entry_t *attr_map[ATTR_BUCKETS];

static cg_attrs *get_attrs(struct tree *t)
{
    if (!t) return NULL;
    unsigned long h = ((unsigned long)(uintptr_t)t) % ATTR_BUCKETS;
    for (attr_entry_t *e = attr_map[h]; e; e = e->next)
        if (e->key == t) return e->val;
    attr_entry_t *e = calloc(1, sizeof(attr_entry_t));
    e->key = t;
    e->val = calloc(1, sizeof(cg_attrs));
    e->val->place    = addr_none();
    e->val->first    = addr_none();
    e->val->follow   = addr_none();
    e->val->on_true  = addr_none();
    e->val->on_false = addr_none();
    e->next = attr_map[h];
    attr_map[h] = e;
    return e->val;
}

static void attr_map_free(void)
{
    for (int i = 0; i < ATTR_BUCKETS; i++) {
        attr_entry_t *e = attr_map[i];
        while (e) {
            attr_entry_t *next = e->next;
            /* note: code lists are owned by parent; don't double-free */
            free(e->val);
            free(e);
            e = next;
        }
        attr_map[i] = NULL;
    }
}

//In which we do a bunch of suff to make symbol table addresses a thing
#define SYM_BUCKETS 512
typedef struct sym_addr_entry {
    SymbolTableEntry    key;
    addr_t              addr;
    struct sym_addr_entry *next;
} sym_addr_entry_t;

static sym_addr_entry_t *sym_addr_map[SYM_BUCKETS];

static void sym_set_addr(SymbolTableEntry e, addr_t a)
{
    unsigned long h = ((unsigned long)(uintptr_t)e) % SYM_BUCKETS;
    for (sym_addr_entry_t *n = sym_addr_map[h]; n; n = n->next)
        if (n->key == e) { n->addr = a; return; }
    sym_addr_entry_t *n = calloc(1, sizeof(sym_addr_entry_t));
    n->key  = e; n->addr = a; n->next = sym_addr_map[h];
    sym_addr_map[h] = n;
}

static addr_t sym_get_addr(SymbolTableEntry e)
{
    if (!e) return addr_none();
    unsigned long h = ((unsigned long)(uintptr_t)e) % SYM_BUCKETS;
    for (sym_addr_entry_t *n = sym_addr_map[h]; n; n = n->next)
        if (n->key == e) return n->addr;
    return addr_none();
}

static void sym_addr_map_free(void)
{
    for (int i = 0; i < SYM_BUCKETS; i++) {
        sym_addr_entry_t *n = sym_addr_map[i];
        while (n) { sym_addr_entry_t *nx = n->next; free(n); n = nx; }
        sym_addr_map[i] = NULL;
    }
}


static long g_temp_offset = 0;   /* reset at each function entry */

static addr_t new_temp(void)
{
    addr_t a = addr_local(g_temp_offset);
    g_temp_offset += WORD_SIZE;
    return a;
}

static long g_string_offset = 0;
static icode_list_t *g_string_seg = NULL;
static icode_list_t *g_data_seg   = NULL;

static addr_t add_string_const(const char *s)
{
    long off = g_string_offset;
    /* strip surrounding Kotlin quotes */
    char *stripped = NULL;
    if (s && s[0] == '"') {
        stripped = strdup(s + 1);
        int len  = strlen(stripped);
        if (len > 0 && stripped[len-1] == '"') stripped[len-1] = '\0';
    } else {
        stripped = s ? strdup(s) : strdup("");
    }
    /* length including NUL */
    g_string_offset += (long)strlen(stripped) + 1;
    icode_append(g_string_seg, instr_string(off, stripped));
    free(stripped);
    return addr_string(off);
}

//walk table and assign offsets
static long assign_table_offsets(SymbolTable st, region_t region, long start)
{
    long offset = start;
    if (!st) return offset;
    for (int b = 0; b < st->nBuckets; b++) {
        for (SymbolTableEntry e = st->tbl[b]; e; e = e->next) {
            addr_t a;
            a.region = region;
            a.offset = offset;
            sym_set_addr(e, a);
            offset += WORD_SIZE;
        }
    }
    return offset;
}

//descend into scopes
long assign_offsets(SymbolTable global)
{
    if (!global) return 0;
    long global_size = assign_table_offsets(global, R_GLOBAL, 0);
    (void)global_size;
    return global_size;
}

static long assign_fn_offsets(SymbolTable fn_scope)
{
    if (!fn_scope) return 0;
    long local_off = 0;
    for (int b = 0; b < fn_scope->nBuckets; b++) {
        for (SymbolTableEntry e = fn_scope->tbl[b]; e; e = e->next) {
            addr_t a = addr_local(local_off);
            sym_set_addr(e, a);
            local_off += WORD_SIZE;
        }
    }
    return local_off;
}

/* -----------------------------------------------------------------------
 * Leaf helpers
 * ---------------------------------------------------------------------- */
static const char *leaf_text(struct tree *t)
{
    if (t && t->nkids == 0 && t->leaf && t->leaf->lexeme)
        return t->leaf->lexeme;
    return NULL;
}

static int leaf_code(struct tree *t)
{
    if (t && t->nkids == 0 && t->leaf) return t->leaf->code;
    return 0;
}

//current scope chain
static SymbolTable g_current_scope = NULL;
static SymbolTable g_global_scope  = NULL;

static addr_t lookup_var_addr(const char *name)
{
    if (!name) return addr_none();
    SymbolTableEntry e = lookupsym(g_current_scope, name);
    if (e) return sym_get_addr(e);
    return addr_none();
}

//give .first labels
void assign_first(struct tree *t)
{
    if (!t) return;
    for (int i = 0; i < t->nkids; i++) assign_first(t->kids[i]);

    const char *sym = t->symbol ? t->symbol : "";

    /* Nodes that can be branch targets need a first label */
    if (strcmp(sym, "ifStatement")    == 0 ||
        strcmp(sym, "whileStatement") == 0 ||
        strcmp(sym, "forStatement")   == 0 ||
        strcmp(sym, "block")          == 0 ||
        strcmp(sym, "statements")     == 0 ||
        strcmp(sym, "binaryExpr")     == 0 ||
        strcmp(sym, "unaryExpr")      == 0) {
        cg_attrs *a = get_attrs(t);
        if (!a->first_set) {
            a->first = genlabel();
            a->first_set = 1;
        }
    }
}

//.follow labels
void assign_follow(struct tree *t, addr_t follow)
{
    if (!t) return;
    const char *sym = t->symbol ? t->symbol : "";
    cg_attrs *a = get_attrs(t);

    a->follow = follow;
    a->follow_set = 1;

    if (strcmp(sym, "statements") == 0 && t->nkids == 2) {
        /* stmt_seq -> stmt1  stmt2 */
        cg_attrs *a1 = get_attrs(t->kids[1]);
        if (!a1->first_set) { a1->first = genlabel(); a1->first_set = 1; }
        assign_follow(t->kids[0], a1->first);   /* stmt1's follow = stmt2.first */
        assign_follow(t->kids[1], follow);
        return;
    }

    if (strcmp(sym, "ifStatement") == 0) {
        /* IF ( cond ) body  [ELSE body2] */
        assign_follow(t->kids[2], follow);   /* cond */
        if (t->nkids == 5) {
            /* if-then */
            assign_follow(t->kids[4], follow);
        } else if (t->nkids == 7) {
            /* if-then-else */
            assign_follow(t->kids[4], follow);
            assign_follow(t->kids[6], follow);
        }
        return;
    }

    if (strcmp(sym, "whileStatement") == 0) {
        /* WHILE ( cond ) body */
        cg_attrs *wa = get_attrs(t);
        assign_follow(t->kids[2], follow);   /* cond: on_false -> follow */
        assign_follow(t->kids[4], wa->first_set ? wa->first : follow);
        return;
    }

    /* default: push follow down to all kids */
    for (int i = 0; i < t->nkids; i++)
        assign_follow(t->kids[i], follow);
}

//boolean labels
void assign_bool_labels(struct tree *t)
{
    if (!t) return;
    const char *sym = t->symbol ? t->symbol : "";
    cg_attrs *a = get_attrs(t);

    if (strcmp(sym, "ifStatement") == 0) {
        /* condition is kids[2] */
        struct tree *cond = t->kids[2];
        if (cond) {
            cg_attrs *ca = get_attrs(cond);
            /* on_true -> body.first, on_false -> follow */
            struct tree *body = (t->nkids >= 5) ? t->kids[4] : NULL;
            cg_attrs *ba = body ? get_attrs(body) : NULL;
            ca->on_true  = (ba && ba->first_set) ? ba->first : genlabel();
            ca->on_true_set = 1;
            if (t->nkids == 7) {
                struct tree *else_body = t->kids[6];
                cg_attrs *ea = else_body ? get_attrs(else_body) : NULL;
                ca->on_false = (ea && ea->first_set) ? ea->first : genlabel();
            } else {
                ca->on_false = a->follow_set ? a->follow : genlabel();
            }
            ca->on_false_set = 1;
        }
    }

    if (strcmp(sym, "whileStatement") == 0) {
        struct tree *cond = t->kids[2];
        if (cond) {
            cg_attrs *ca = get_attrs(cond);
            struct tree *body = (t->nkids >= 5) ? t->kids[4] : NULL;
            cg_attrs *ba = body ? get_attrs(body) : NULL;
            ca->on_true  = (ba && ba->first_set) ? ba->first : genlabel();
            ca->on_true_set  = 1;
            ca->on_false = a->follow_set ? a->follow : genlabel();
            ca->on_false_set = 1;
        }
    }

    for (int i = 0; i < t->nkids; i++)
        assign_bool_labels(t->kids[i]);
}


static addr_t     cg_expr(struct tree *t, icode_list_t *code);
static void       cg_stmt(struct tree *t, icode_list_t *code);
static void       cg_bool(struct tree *t, icode_list_t *code,
                           addr_t on_true, addr_t on_false);

//boolean expression gen
static void cg_bool(struct tree *t, icode_list_t *code,
                    addr_t on_true, addr_t on_false)
{
    if (!t) return;
    const char *sym = t->symbol ? t->symbol : "";

    /* AND: left && right
       if !left goto on_false; if !right goto on_false; goto on_true */
    if (strcmp(sym, "binaryExpr") == 0 &&
        t->nkids == 3 && leaf_code(t->kids[1]) == AND) {
        addr_t mid = genlabel();
        cg_bool(t->kids[0], code, mid, on_false);
        /* mid label */
        icode_append(code, instr_new(OP_LABEL, mid, addr_none(), addr_none()));
        cg_bool(t->kids[2], code, on_true, on_false);
        return;
    }

    /* OR: left || right
       if left goto on_true; if right goto on_true; goto on_false */
    if (strcmp(sym, "binaryExpr") == 0 &&
        t->nkids == 3 && leaf_code(t->kids[1]) == OR) {
        addr_t mid = genlabel();
        cg_bool(t->kids[0], code, on_true, mid);
        icode_append(code, instr_new(OP_LABEL, mid, addr_none(), addr_none()));
        cg_bool(t->kids[2], code, on_true, on_false);
        return;
    }

    /* NOT */
    if (strcmp(sym, "unaryExpr") == 0 &&
        t->nkids == 2 && leaf_code(t->kids[0]) == NOT) {
        cg_bool(t->kids[1], code, on_false, on_true);
        return;
    }

    /* general boolean: evaluate to a temp, then branch */
    addr_t val = cg_expr(t, code);
    icode_append(code, instr_new(OP_JUMP_TRUE,  on_true,  val, addr_none()));
    icode_append(code, instr_new(OP_JUMP,        on_false, addr_none(), addr_none()));
}

// .place address where node was stored
static addr_t cg_expr(struct tree *t, icode_list_t *code)
{
    if (!t) return addr_none();

    if (t->nkids == 0) {
        if (!t->leaf) return addr_none();
        switch (t->leaf->code) {
        case INTEGERLITERAL:
            return addr_const(atol(t->leaf->lexeme));
        case LONGLITERAL: {
            char *lex = t->leaf->lexeme;
            /* strip trailing L */
            long v = atol(lex);
            return addr_const(v);
        }
        case BOOLEANLITERAL:
            return addr_const(strcmp(t->leaf->lexeme, "true") == 0 ? 1 : 0);
        case STRINGLITERAL: {
            addr_t saddr = add_string_const(t->leaf->lexeme);
            addr_t tmp   = new_temp();
            icode_append(code, instr_new(OP_ADDR, tmp, saddr, addr_none()));
            return tmp;
        }
        case IDENT: {
            addr_t va = lookup_var_addr(t->leaf->lexeme);
            return va;
        }
        case NULLLITERAL:
            return addr_const(0);
        default:
            return addr_none();
        }
    }

    const char *sym = t->symbol ? t->symbol : "";

    /* ---- binaryExpr --------------------------------------------------- */
    if (strcmp(sym, "binaryExpr") == 0 && t->nkids == 3) {
        int opc = leaf_code(t->kids[1]);

        /* Short-circuit AND/OR: evaluate to a 0/1 temp via labels */
        if (opc == AND || opc == OR) {
            addr_t res    = new_temp();
            addr_t l_true = genlabel();
            addr_t l_false= genlabel();
            addr_t l_end  = genlabel();
            cg_bool(t, code, l_true, l_false);
            icode_append(code, instr_new(OP_LABEL,  l_true,  addr_none(), addr_none()));
            icode_append(code, instr_new(OP_ASSIGN, res, addr_const(1), addr_none()));
            icode_append(code, instr_new(OP_JUMP,   l_end,   addr_none(), addr_none()));
            icode_append(code, instr_new(OP_LABEL,  l_false, addr_none(), addr_none()));
            icode_append(code, instr_new(OP_ASSIGN, res, addr_const(0), addr_none()));
            icode_append(code, instr_new(OP_LABEL,  l_end,   addr_none(), addr_none()));
            return res;
        }

        addr_t lhs = cg_expr(t->kids[0], code);
        addr_t rhs = cg_expr(t->kids[2], code);
        addr_t tmp = new_temp();
        opcode_t op;
        switch (opc) {
        case ADD:    op = OP_ADD_INT; break;
        case SUB:    op = OP_SUB_INT; break;
        case MUL:    op = OP_MUL_INT; break;
        case DIV:    op = OP_DIV_INT; break;
        case MOD:    op = OP_MOD_INT; break;
        case EQ:     op = OP_EQ_INT;  break;
        case NEQ:    op = OP_NEQ_INT; break;
        case LANGLE: op = OP_LT_INT;  break;
        case RANGLE: op = OP_GT_INT;  break;
        case LTE:    op = OP_LTE_INT; break;
        case GTE:    op = OP_GTE_INT; break;
        case REF_EQ: op = OP_EQ_INT;  break;
        case REF_NEQ:op = OP_NEQ_INT; break;
        default:     op = OP_ADD_INT; break;
        }
        icode_append(code, instr_new(op, tmp, lhs, rhs));
        return tmp;
    }

    /* ---- unaryExpr ---------------------------------------------------- */
    if (strcmp(sym, "unaryExpr") == 0 && t->nkids == 2) {
        int opc = leaf_code(t->kids[0]);
        addr_t operand = cg_expr(t->kids[1], code);
        addr_t tmp = new_temp();
        if (opc == SUB)
            icode_append(code, instr_new(OP_NEG_INT, tmp, operand, addr_none()));
        else if (opc == NOT)
            icode_append(code, instr_new(OP_NOT, tmp, operand, addr_none()));
        else
            return operand;
        return tmp;
    }

    /* ---- postfixExpr -------------------------------------------------- */
    if (strcmp(sym, "postfixExpr") == 0 && t->nkids == 2) {
        addr_t base = cg_expr(t->kids[0], code);
        int opc = leaf_code(t->kids[1]);
        if (opc == INCR)
            icode_append(code, instr_new(OP_INCR, base, addr_none(), addr_none()));
        else
            icode_append(code, instr_new(OP_DECR, base, addr_none(), addr_none()));
        return base;
    }

    /* ---- callExpr ----------------------------------------------------- */
    if (strcmp(sym, "callExpr") == 0) {
        /* kids[0] = callee (usually an IDENT leaf)
           kids[2] = argumentList (if nkids==4)         */
        const char *fname = NULL;
        if (t->kids[0] && t->kids[0]->nkids == 0 && t->kids[0]->leaf)
            fname = t->kids[0]->leaf->lexeme;
        if (!fname) fname = "???";

        /* collect arguments into a local array (reverse-push later) */
        addr_t args[32];
        int    nargs = 0;

        if (t->nkids == 4) {
            /* flatten argumentList (left-recursive tree) */
            struct tree *al = t->kids[2];
            /* collect in a small stack so we push left-to-right */
            struct tree *arg_stack[32];
            int sp = 0;
            while (al && al->symbol &&
                   strcmp(al->symbol, "argumentList") == 0 &&
                   al->nkids == 3) {
                arg_stack[sp++] = al->kids[2]; /* rightmost arg */
                al = al->kids[0];
            }
            arg_stack[sp++] = al; /* first arg */
            /* evaluate left-to-right */
            for (int k = sp - 1; k >= 0 && nargs < 32; k--)
                args[nargs++] = cg_expr(arg_stack[k], code);
        }

        /* push parameters (last-to-first for C ABI, but we push in order) */
        for (int k = nargs - 1; k >= 0; k--)
            icode_append(code, instr_new(OP_PARM, addr_none(), args[k], addr_none()));

        /* result temp */
        addr_t result = new_temp();
        icode_append(code, instr_call(result, fname, nargs));
        return result;
    }

    /* ---- rangeExpr  (evaluate as the start value for now) ------------- */
    if (strcmp(sym, "rangeExpr") == 0 && t->nkids == 3)
        return cg_expr(t->kids[0], code);

    /* ---- elvisExpr (?:) ----------------------------------------------- */
    if (strcmp(sym, "elvisExpr") == 0 && t->nkids == 3) {
        addr_t lhs = cg_expr(t->kids[0], code);
        addr_t rhs = cg_expr(t->kids[2], code);
        addr_t tmp = new_temp();
        addr_t l_nonnull = genlabel();
        addr_t l_end     = genlabel();
        icode_append(code, instr_new(OP_JUMP_TRUE, l_nonnull, lhs, addr_none()));
        icode_append(code, instr_new(OP_ASSIGN, tmp, rhs, addr_none()));
        icode_append(code, instr_new(OP_JUMP, l_end, addr_none(), addr_none()));
        icode_append(code, instr_new(OP_LABEL, l_nonnull, addr_none(), addr_none()));
        icode_append(code, instr_new(OP_ASSIGN, tmp, lhs, addr_none()));
        icode_append(code, instr_new(OP_LABEL, l_end, addr_none(), addr_none()));
        return tmp;
    }

    /* ---- exprStatement ------------------------------------------------ */
    if (strcmp(sym, "exprStatement") == 0 && t->nkids == 2)
        return cg_expr(t->kids[0], code);

    /* ---- memberExpr / indexExpr --------------------------------------- */
    if (strcmp(sym, "memberExpr") == 0 ||
        strcmp(sym, "safeMemberExpr") == 0 ||
        strcmp(sym, "indexExpr") == 0) {
        addr_t base = cg_expr(t->kids[0], code);
        addr_t tmp  = new_temp();
        icode_append(code, instr_new(OP_LOAD, tmp, base, addr_none()));
        return tmp;
    }

    /* ---- castExpr ----------------------------------------------------- */
    if (strcmp(sym, "castExpr") == 0 && t->nkids >= 1)
        return cg_expr(t->kids[0], code);

    /* ---- descend to first child that produces a value ----------------- */
    for (int i = 0; i < t->nkids; i++) {
        if (t->kids[i]) {
            addr_t v = cg_expr(t->kids[i], code);
            if (v.region != R_NONE) return v;
        }
    }
    return addr_none();
}

/* -----------------------------------------------------------------------
 * Statement code generation
 * ---------------------------------------------------------------------- */

/* We need a scope-finding helper.  We store fn scopes in a global list
   in syms.c; replicate the search by name here via a small interface.
   Since we cannot depend on syms.c internals, we just walk parent.     */
static SymbolTable find_fn_scope_by_name(SymbolTable parent, const char *name)
{
    /* syms.c keeps a global scope_list; we don't have access here.
       We expose a simple lookup via a weak extern that syms.c can fill. */
    extern SymbolTable cg_find_fn_scope(SymbolTable parent, const char *name);
    return cg_find_fn_scope(parent, name);
}

static void cg_stmt(struct tree *t, icode_list_t *code)
{
    if (!t) return;
    const char *sym = t->symbol ? t->symbol : "";

    /* ---- function declaration ----------------------------------------- */
    if (strcmp(sym, "functionDeclaration") == 0) {
        const char *fname = leaf_text(t->kids[1]);
        if (!fname) fname = "???";

        /* Find the function's own scope */
        SymbolTable fn_scope = find_fn_scope_by_name(g_current_scope, fname);
        long frame_size = 0;
        if (fn_scope) {
            frame_size = assign_fn_offsets(fn_scope);
        }

        /* Reset temp allocator; temps live above the param/local area.
           We start temps at frame_size and grow upward.                */
        long saved_temp = g_temp_offset;
        g_temp_offset   = frame_size;

        SymbolTable saved_scope = g_current_scope;
        if (fn_scope) g_current_scope = fn_scope;

        /* Emit PROC pseudo-instruction */
        icode_append(code, instr_proc(fname, 0 /* placeholder */));

        /* Find the block kid */
        struct tree *blk = NULL;
        for (int i = 0; i < t->nkids; i++) {
            if (t->kids[i] && t->kids[i]->symbol &&
                strcmp(t->kids[i]->symbol, "block") == 0) {
                blk = t->kids[i]; break;
            }
        }
        if (blk) cg_stmt(blk, code);

        icode_append(code, instr_endproc(fname));

        g_current_scope = saved_scope;
        g_temp_offset   = saved_temp;
        return;
    }

    /* ---- block -------------------------------------------------------- */
    if (strcmp(sym, "block") == 0) {
        /* kids[0]=LCURL, kids[1]=statements, kids[2]=RCURL */
        if (t->nkids >= 2) cg_stmt(t->kids[1], code);
        return;
    }

    /* ---- statements --------------------------------------------------- */
    if (strcmp(sym, "statements") == 0) {
        if (t->nkids == 2) {
            cg_stmt(t->kids[0], code);
            cg_stmt(t->kids[1], code);
        } else if (t->nkids == 1) {
            cg_stmt(t->kids[0], code);
        }
        return;
    }

    /* ---- varDeclaration ----------------------------------------------- */
    if (strcmp(sym, "varDeclaration") == 0) {
        /* kids[1] = IDENT, initialiser is the last kid (expression) */
        const char *vname = leaf_text(t->kids[1]);
        addr_t dst = vname ? lookup_var_addr(vname) : addr_none();

        /* find the initialiser expression (last kid that isn't a type/punctuation) */
        struct tree *init = NULL;
        if (t->nkids == 4) init = t->kids[3];        /* val/var IDENT = expr  */
        else if (t->nkids == 6) init = t->kids[5];   /* val/var IDENT : T = expr */

        if (init && dst.region != R_NONE) {
            addr_t src = cg_expr(init, code);
            icode_append(code, instr_new(OP_ASSIGN, dst, src, addr_none()));
        }
        return;
    }

    /* ---- assignment --------------------------------------------------- */
    if (strcmp(sym, "assignment") == 0 && t->nkids == 3) {
        const char *vname = leaf_text(t->kids[0]);
        addr_t dst = vname ? lookup_var_addr(vname) : addr_none();
        addr_t src = cg_expr(t->kids[2], code);

        int asgn_op = leaf_code(t->kids[1]);
        if (asgn_op == ADD_ASSIGN) {
            addr_t tmp = new_temp();
            icode_append(code, instr_new(OP_ADD_INT, tmp, dst, src));
            src = tmp;
        } else if (asgn_op == SUB_ASSIGN) {
            addr_t tmp = new_temp();
            icode_append(code, instr_new(OP_SUB_INT, tmp, dst, src));
            src = tmp;
        } else if (asgn_op == MUL_ASSIGN) {
            addr_t tmp = new_temp();
            icode_append(code, instr_new(OP_MUL_INT, tmp, dst, src));
            src = tmp;
        } else if (asgn_op == DIV_ASSIGN) {
            addr_t tmp = new_temp();
            icode_append(code, instr_new(OP_DIV_INT, tmp, dst, src));
            src = tmp;
        } else if (asgn_op == MOD_ASSIGN) {
            addr_t tmp = new_temp();
            icode_append(code, instr_new(OP_MOD_INT, tmp, dst, src));
            src = tmp;
        }

        if (dst.region != R_NONE)
            icode_append(code, instr_new(OP_ASSIGN, dst, src, addr_none()));
        return;
    }

    /* ---- ifStatement -------------------------------------------------- */
    if (strcmp(sym, "ifStatement") == 0) {
        /*
         * IF ( cond ) body
         * IF ( cond ) body ELSE else_body
         *
         * We use the .on_true / .on_false attrs if set, otherwise
         * generate fresh labels.
         */
        struct tree *cond      = (t->nkids >= 3) ? t->kids[2] : NULL;
        struct tree *then_body = (t->nkids >= 5) ? t->kids[4] : NULL;
        struct tree *else_body = (t->nkids == 7) ? t->kids[6] : NULL;

        cg_attrs *ca = cond ? get_attrs(cond) : NULL;

        addr_t l_true  = (ca && ca->on_true_set)  ? ca->on_true
                                                   : genlabel();
        addr_t l_false = (ca && ca->on_false_set) ? ca->on_false
                                                   : genlabel();
        addr_t l_end   = get_attrs(t)->follow_set  ? get_attrs(t)->follow
                                                   : genlabel();

        /* Emit condition as a boolean branch */
        cg_bool(cond, code, l_true, l_false);

        /* then branch */
        icode_append(code, instr_new(OP_LABEL, l_true, addr_none(), addr_none()));
        if (then_body) cg_stmt(then_body, code);
        icode_append(code, instr_new(OP_JUMP, l_end, addr_none(), addr_none()));

        /* else branch */
        icode_append(code, instr_new(OP_LABEL, l_false, addr_none(), addr_none()));
        if (else_body) cg_stmt(else_body, code);

        /* end label */
        icode_append(code, instr_new(OP_LABEL, l_end, addr_none(), addr_none()));
        return;
    }

    /* ---- whileStatement ----------------------------------------------- */
    if (strcmp(sym, "whileStatement") == 0) {
        /*
         * WHILE ( cond ) body
         *
         * l_top:  (loop header)
         *   cg_bool(cond) -> jtrue l_body, jfalse l_end
         * l_body:
         *   cg_stmt(body)
         *   jump l_top
         * l_end:
         */
        struct tree *cond = (t->nkids >= 3) ? t->kids[2] : NULL;
        struct tree *body = (t->nkids >= 5) ? t->kids[4] : NULL;

        addr_t l_top  = genlabel();
        addr_t l_body = genlabel();
        addr_t l_end  = genlabel();

        icode_append(code, instr_new(OP_LABEL, l_top, addr_none(), addr_none()));
        cg_bool(cond, code, l_body, l_end);

        icode_append(code, instr_new(OP_LABEL, l_body, addr_none(), addr_none()));
        if (body) cg_stmt(body, code);
        icode_append(code, instr_new(OP_JUMP, l_top, addr_none(), addr_none()));

        icode_append(code, instr_new(OP_LABEL, l_end, addr_none(), addr_none()));
        return;
    }

    /* ---- forStatement ------------------------------------------------- */
    if (strcmp(sym, "forStatement") == 0) {
        /*
         * FOR ( IDENT IN rangeExpr ) body
         * kids: FOR LPAREN IDENT IN rangeExpr RPAREN body
         *
         * We generate:
         *   IDENT = range.start
         * l_top:
         *   if IDENT > range.end goto l_end
         *   body
         *   IDENT++
         *   goto l_top
         * l_end:
         */
        const char *iname = leaf_text(t->kids[2]);
        struct tree *range = (t->nkids >= 5) ? t->kids[4] : NULL;
        struct tree *body  = (t->nkids >= 7) ? t->kids[6] : NULL;

        addr_t i_addr = iname ? lookup_var_addr(iname) : addr_none();

        addr_t start_val = addr_const(0);
        addr_t end_val   = addr_const(0);

        if (range && range->symbol && strcmp(range->symbol, "rangeExpr") == 0) {
            start_val = cg_expr(range->kids[0], code);
            end_val   = cg_expr(range->kids[2], code);
        } else if (range) {
            start_val = cg_expr(range, code);
        }

        /* init loop variable */
        if (i_addr.region != R_NONE)
            icode_append(code, instr_new(OP_ASSIGN, i_addr, start_val, addr_none()));

        addr_t l_top = genlabel();
        addr_t l_end = genlabel();

        icode_append(code, instr_new(OP_LABEL, l_top, addr_none(), addr_none()));

        /* condition: i <= end  (or i < end for until) */
        addr_t cmp_tmp = new_temp();
        icode_append(code, instr_new(OP_GT_INT, cmp_tmp, i_addr, end_val));
        icode_append(code, instr_new(OP_JUMP_TRUE, l_end, cmp_tmp, addr_none()));

        if (body) cg_stmt(body, code);

        /* increment */
        if (i_addr.region != R_NONE)
            icode_append(code, instr_new(OP_INCR, i_addr, addr_none(), addr_none()));

        icode_append(code, instr_new(OP_JUMP, l_top, addr_none(), addr_none()));
        icode_append(code, instr_new(OP_LABEL, l_end, addr_none(), addr_none()));
        return;
    }

    /* ---- jumpStatement (return / break / continue) -------------------- */
    if (strcmp(sym, "jumpStatement") == 0) {
        if (t->nkids == 1) {
            /* RETURN or BREAK or CONTINUE */
            int kw = leaf_code(t->kids[0]);
            if (kw == RETURN)
                icode_append(code, instr_new(OP_RETURN, addr_none(),
                                              addr_none(), addr_none()));
        } else if (t->nkids == 2) {
            /* RETURN expr */
            addr_t val = cg_expr(t->kids[1], code);
            icode_append(code, instr_new(OP_RETURN, addr_none(), val, addr_none()));
        }
        return;
    }

    /* ---- expression statement ----------------------------------------- */
    if (strcmp(sym, "exprStatement") == 0) {
        cg_expr(t->kids[0], code);
        return;
    }

    /* ---- generic expression used as statement ------------------------- */
    /* (bare function calls, etc.) */
    if (strcmp(sym, "callExpr") == 0 ||
        strcmp(sym, "postfixExpr") == 0 ||
        strcmp(sym, "binaryExpr") == 0) {
        cg_expr(t, code);
        return;
    }

    /* ---- program / topLevel: recurse ---------------------------------- */
    for (int i = 0; i < t->nkids; i++)
        cg_stmt(t->kids[i], code);
}

/* -----------------------------------------------------------------------
 * codegen  --  top-level entry point
 * ---------------------------------------------------------------------- */
icode_list_t *codegen(struct tree *root, SymbolTable global,
                       icode_list_t **str_seg_out, icode_list_t **dat_seg_out)
{
    /* Initialise string / data segments */
    g_string_seg = icode_empty();
    g_data_seg   = icode_empty();

    /* Assign global variable offsets */
    g_global_scope  = global;
    g_current_scope = global;

    /* Assign globals: iterate the global symbol table */
    long goff = 0;
    for (int b = 0; b < global->nBuckets; b++) {
        for (SymbolTableEntry e = global->tbl[b]; e; e = e->next) {
            if (e->type == function_type) continue; /* not a variable */
            addr_t a = addr_global(goff);
            sym_set_addr(e, a);
            goff += WORD_SIZE;
            /* emit global declaration into .data */
            icode_append(g_data_seg, instr_global_decl(e->s, a));
        }
    }

    /* Assign label attributes (Lab 10) */
    assign_first(root);
    addr_t end_label = genlabel();
    assign_follow(root, end_label);
    assign_bool_labels(root);

    /* Main code generation */
    icode_list_t *code = icode_empty();
    cg_stmt(root, code);

    if (str_seg_out) *str_seg_out = g_string_seg;
    if (dat_seg_out) *dat_seg_out = g_data_seg;

    attr_map_free();
    sym_addr_map_free();

    return code;
}

/* -----------------------------------------------------------------------
 * make_ic_filename
 * Strip directory prefix, replace .kt extension with .ic.
 * ---------------------------------------------------------------------- */
char *make_ic_filename(const char *source_file)
{
    if (!source_file) return strdup("out.ic");

    /* basename: last '/' */
    const char *base = strrchr(source_file, '/');
    base = base ? base + 1 : source_file;

    char *out = strdup(base);
    char *dot = strrchr(out, '.');
    if (dot && strcmp(dot, ".kt") == 0)
        strcpy(dot, ".ic");
    else {
        char *tmp = malloc(strlen(out) + 4);
        sprintf(tmp, "%s.ic", out);
        free(out);
        out = tmp;
    }
    return out;
}