#ifndef TYPE_H
#define TYPE_H

#include "tree.h"
#include "symtab.h"

/* Forward declaration */
struct typeinfo;
typedef struct typeinfo *typeptr;

/* Parameter list for functions */
typedef struct param {
    char *name;
    struct typeinfo *type;
    struct param *next;
} *paramlist;

/* ===== Type Constants ===== */

#define FIRST_TYPE   1000000

#define NONE_TYPE    1000000
#define INT_TYPE     1000001
#define FLOAT_TYPE   1000002
#define BOOL_TYPE    1000003
#define CHAR_TYPE    1000004
#define STRING_TYPE  1000005
#define ARRAY_TYPE   1000006
#define FUNC_TYPE    1000007
#define ANY_TYPE     1000008

#define LAST_TYPE    1000008

/* ===== Type Representation ===== */

typedef struct typeinfo {
    int basetype;

    union {
        struct funcinfo {
            char *name;
            int defined;
            SymbolTable st;
            struct typeinfo *returntype;
            int nparams;
            struct param *parameters;
        } f;

        struct arrayinfo {
            int size; /* -1 means unknown */
            struct typeinfo *elemtype;
        } a;

    } u;

} *typeptr;

/* ===== Constructors ===== */

typeptr alctype(int base);

typeptr alcfunctype(struct tree *ret_type_node,
                    struct tree *params_node,
                    SymbolTable st,
                    const char *name);

typeptr alcarraytype(typeptr elemtype, int size);

/* ===== Utilities ===== */

typeptr clone_type(typeptr t);
typeptr type_from_tree(struct tree *t);
typeptr type_from_token_code(int tokcode);

char *typename(typeptr t);

/* ===== Global Type Singletons ===== */

extern typeptr none_typeptr;
extern typeptr integer_typeptr;
extern typeptr float_typeptr;
extern typeptr boolean_typeptr;
extern typeptr char_typeptr;
extern typeptr string_typeptr;
extern typeptr any_typeptr;

/* ===== Names ===== */

extern char *typenam[];

#endif
