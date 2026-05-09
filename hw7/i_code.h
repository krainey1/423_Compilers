#ifndef ICODE_H
#define ICODE_H

/* THE MEMORY ADDRESS REGIONS 
 *  R_GLOBAL  – global (static) variables             (.data)
 *  R_LOCAL   – local / temporaries (frame-relative)
 *  R_PARAM   – formal parameters
 *  R_CONST   – integer/float immediate constant      (value stored in offset)
 *  R_IMMED   – alias for R_CONST (immediate integer)
 *  R_STRING  – offset into .string region
 *  R_LABEL   – code label (used for branch targets / genlabel)
 *  R_NONE    – "no address" / unused operand slot
 */
 
typedef enum {
    R_NONE   = 0,
    R_GLOBAL,
    R_LOCAL,
    R_PARAM,
    R_CONST,
    R_IMMED,   
    R_STRING,
    R_LABEL
} region_t;
 
/* mem address <region, offset> pair.
   For R_CONST/R_IMMED the offset IS the integer value.
   R_STRING byte offset into .string.
   R_LABEL unique label counter. */
typedef struct addr {
    region_t region;
    long     offset;   
} addr_t;
 

// OPCODES  
typedef enum { //number handled by the enum
   
    OP_ASSIGN = 1,   /* dst = src                         */
    OP_ADDR,         /* dst = &src  (address-of)          */
    OP_LOAD,         /* dst = *src  (deref)               */
    OP_STORE,        /* *dst = src                        */
 
    // integer arithmetic 
    OP_ADD_INT,
    OP_SUB_INT,
    OP_MUL_INT,
    OP_DIV_INT,
    OP_MOD_INT,
    OP_NEG_INT,      
 
    //float arithmetic
    OP_ADD_FLT,
    OP_SUB_FLT,
    OP_MUL_FLT,
    OP_DIV_FLT,
    OP_NEG_FLT,
 
    //comparisons
    OP_EQ_INT,
    OP_NEQ_INT,
    OP_LT_INT,
    OP_GT_INT,
    OP_LTE_INT,
    OP_GTE_INT,
 
    //float comparisons
    OP_EQ_FLT,
    OP_NEQ_FLT,
    OP_LT_FLT,
    OP_GT_FLT,
    OP_LTE_FLT,
    OP_GTE_FLT,
 
    //string operations
    OP_ADD_STR,      //concat
    OP_EQ_STR,
    OP_NEQ_STR,
 
    //logic
    OP_NOT,          /* dst = !src1                       */
    OP_AND,          /* dst = src1 && src2                */
    OP_OR,           /* dst = src1 || src2                */
 
    //postfix
    OP_INCR,         /* src1++  (in-place; dst==src1)     */
    OP_DECR,         /* src1--                            */
 
    //control flow
    OP_LABEL,        /* label:                            */
    OP_JUMP,         /* goto label                        */
    OP_JUMP_TRUE,    /* if src1  goto label               */
    OP_JUMP_FALSE,   /* if !src1 goto label               */
 
    //function call / return
    OP_PARM,         /* push argument                     */
    OP_CALL,         /* dst = call fname, nargs           */
    OP_RETURN,       /* return src1 (or no value)         */
 
    //pseudo declarations
    OP_PROC,         /* function entry marker             */
    OP_ENDPROC,      /* function end marker               */
    OP_GLOBAL,       /* global variable declaration       */
    OP_STRING        /* .string directive                  */
} opcode_t;
 
//SINGLE 3-address instruction
typedef struct instr {
    opcode_t   op;
    addr_t     dst;    /* destination (result)             */
    addr_t     src1;   /* first source operand             */
    addr_t     src2;   /* second source operand (if used)  */
 
    /* extra fields for CALL and PROC */
    char      *name;   /* function name for CALL/PROC      */
    int        ival;   /* nargs for CALL; frame size for PROC */
 
    /* string data (OP_STRING) */
    char      *sval;
 
    struct instr *next;
} instr_t;
 
//INSTRUCTION LIST
typedef struct icode_list {
    instr_t *head;
    instr_t *tail;
    int      len;
} icode_list_t;
 

 
// addr constructors 
addr_t addr_none(void);
addr_t addr_global(long offset);
addr_t addr_local(long offset);
addr_t addr_param(long offset);
addr_t addr_const(long value);
addr_t addr_string(long offset);
addr_t addr_label(long n);
 
// label generator 
addr_t genlabel(void);
 
// instruction constructors 
instr_t *instr_new(opcode_t op, addr_t dst, addr_t src1, addr_t src2);
instr_t *instr_call(addr_t dst, const char *name, int nargs);
instr_t *instr_proc(const char *name, int frame_size);
instr_t *instr_endproc(const char *name);
instr_t *instr_global_decl(const char *name, addr_t location);
instr_t *instr_string(long offset, const char *data);
 
// list operations 
icode_list_t *icode_new(instr_t *i);          
icode_list_t *icode_empty(void);              
icode_list_t *icode_concat(icode_list_t *a,
                            icode_list_t *b); 
void          icode_append(icode_list_t *lst, instr_t *i);
void          icode_free(icode_list_t *lst);
 
//output
void icode_emit(icode_list_t *lst, const char *out_filename,
                icode_list_t *string_seg, icode_list_t *data_seg);
 
//some nifty helpers
const char *region_name(region_t r);
const char *opcode_name(opcode_t op);
void        addr_print(addr_t a, char *buf, int bufsz);



#endif