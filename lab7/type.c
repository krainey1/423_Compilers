#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "type.h"
#include "tree.h"
#include "token.h"
#include "symtab.h"
#include "k0gram.tab.h"

static struct typeinfo none_type_obj    = { NONE_TYPE };
static struct typeinfo integer_type_obj = { INT_TYPE };
static struct typeinfo float_type_obj   = { FLOAT_TYPE };
static struct typeinfo boolean_type_obj = { BOOL_TYPE };
static struct typeinfo char_type_obj    = { CHAR_TYPE };
static struct typeinfo string_type_obj  = { STRING_TYPE };
static struct typeinfo any_type_obj     = { ANY_TYPE };

typeptr none_typeptr    = &none_type_obj;
typeptr integer_typeptr = &integer_type_obj;
typeptr float_typeptr   = &float_type_obj;
typeptr boolean_typeptr = &boolean_type_obj;
typeptr char_typeptr    = &char_type_obj;
typeptr string_typeptr  = &string_type_obj;
typeptr any_typeptr     = &any_type_obj;

char *typenam[] = {
    "none", "int", "float", "bool", "char",
    "string", "array", "func", "any"
};

typeptr alctype(int base)
{
    typeptr rv;

    if (base == NONE_TYPE)   return none_typeptr;
    if (base == INT_TYPE)    return integer_typeptr;
    if (base == FLOAT_TYPE)  return float_typeptr;
    if (base == BOOL_TYPE)   return boolean_typeptr;
    if (base == CHAR_TYPE)   return char_typeptr;
    if (base == STRING_TYPE) return string_typeptr;
    if (base == ANY_TYPE)    return any_typeptr;

    rv = (typeptr)calloc(1, sizeof(struct typeinfo));
    if (!rv) return NULL;
    rv->basetype = base;
    return rv;
}

typeptr clone_type(typeptr t)
{
    if (!t) return NULL;

    if (t == none_typeptr || t == integer_typeptr || t == float_typeptr ||
        t == boolean_typeptr || t == char_typeptr || t == string_typeptr ||
        t == any_typeptr) {
        return t;
    }

    typeptr c = alctype(t->basetype);
    if (!c) return NULL;

    if (t->basetype == ARRAY_TYPE) {
        c->u.a.size = t->u.a.size;
        c->u.a.elemtype = clone_type(t->u.a.elemtype);
    } else if (t->basetype == FUNC_TYPE) {
        c->u.f.name = t->u.f.name ? strdup(t->u.f.name) : NULL;
        c->u.f.defined = t->u.f.defined;
        c->u.f.st = t->u.f.st;
        c->u.f.returntype = clone_type(t->u.f.returntype);
        c->u.f.nparams = t->u.f.nparams;

        struct param *src = t->u.f.parameters;
        struct param **dst = &c->u.f.parameters;
        while (src) {
            *dst = (struct param *)calloc(1, sizeof(struct param));
            (*dst)->name = src->name ? strdup(src->name) : NULL;
            (*dst)->type = clone_type(src->type);
            dst = &((*dst)->next);
            src = src->next;
        }
    }

    return c;
}

typeptr alcarraytype(typeptr elemtype, int size)
{
    typeptr rv = alctype(ARRAY_TYPE);
    if (!rv) return NULL;
    rv->u.a.size = size;
    rv->u.a.elemtype = elemtype ? elemtype : any_typeptr;
    return rv;
}

static int count_params(struct tree *t)
{
    if (!t) return 0;

    if (t->symbol && strcmp(t->symbol, "functionValueParameter") == 0)
        return 1;

    if (t->symbol && strcmp(t->symbol, "functionValueParameterList") == 0) {
        if (t->nkids == 3)
            return count_params(t->kids[0]) + count_params(t->kids[2]);
    }

    if (t->nkids > 0)
        return count_params(t->kids[0]);

    return 0;
}

static struct param *build_params(struct tree *t)
{
    if (!t) return NULL;

    if (t->symbol && strcmp(t->symbol, "functionValueParameter") == 0) {
        struct param *p = (struct param *)calloc(1, sizeof(struct param));
        if (!p) return NULL;

        if (t->kids[0] && t->kids[0]->leaf && t->kids[0]->leaf->lexeme)
            p->name = strdup(t->kids[0]->leaf->lexeme);

        p->type = type_from_tree(t->kids[2]);
        p->next = NULL;
        return p;
    }

    if (t->symbol && strcmp(t->symbol, "functionValueParameterList") == 0 && t->nkids == 3) {
        struct param *left = build_params(t->kids[0]);
        struct param *right = build_params(t->kids[2]);

        if (!left) return right;

        struct param *cur = left;
        while (cur->next) cur = cur->next;
        cur->next = right;
        return left;
    }

    return NULL;
}

typeptr alcfunctype(struct tree *ret_type_node,
                    struct tree *params_node,
                    SymbolTable st,
                    const char *name)
{
    typeptr rv = alctype(FUNC_TYPE);
    if (!rv) return NULL;

    rv->u.f.name = name ? strdup(name) : NULL;
    rv->u.f.defined = 1;
    rv->u.f.st = st;
    rv->u.f.returntype = ret_type_node ? type_from_tree(ret_type_node) : none_typeptr;
    rv->u.f.nparams = count_params(params_node);
    rv->u.f.parameters = build_params(params_node);

    return rv;
}

typeptr type_from_token_code(int tokcode)
{
    switch (tokcode) {
        case INTEGERLITERAL:
        case LONGLITERAL:
            return integer_typeptr;

        case REALLITERAL:
        case DOUBLELITERAL:
            return float_typeptr;

        case BOOLEANLITERAL:
            return boolean_typeptr;

        case CHARACTERLITERAL:
            return char_typeptr;

        case STRINGLITERAL:
            return string_typeptr;

        case NULLLITERAL:
            return none_typeptr;

        default:
            return any_typeptr;
    }
}

static typeptr builtin_named_type(const char *name)
{
    if (!name) return any_typeptr;

    if (strcmp(name, "Int") == 0) return integer_typeptr;
    if (strcmp(name, "String") == 0) return string_typeptr;
    if (strcmp(name, "Boolean") == 0) return boolean_typeptr;
    if (strcmp(name, "Char") == 0) return char_typeptr;
    if (strcmp(name, "Float") == 0) return float_typeptr;
    if (strcmp(name, "Double") == 0) return float_typeptr;
    if (strcmp(name, "Unit") == 0) return none_typeptr;

    return any_typeptr;
}

typeptr type_from_tree(struct tree *t)
{
    if (!t) return none_typeptr;

    if (t->type) return t->type;

    /* leaf literals */
    if (t->nkids == 0 && t->leaf) {
        if (t->leaf->code == IDENT) {
            t->type = any_typeptr;
            return t->type;
        }
        t->type = type_from_token_code(t->leaf->code);
        return t->type;
    }

    if (t->symbol && strcmp(t->symbol, "type") == 0) {
        /* type : IDENT */
        if (t->prodrule == 1) {
            const char *name = t->kids[0]->leaf ? t->kids[0]->leaf->lexeme : NULL;
            t->type = builtin_named_type(name);
            return t->type;
        }

        /* type : IDENT QUEST */
        if (t->prodrule == 2) {
            const char *name = t->kids[0]->leaf ? t->kids[0]->leaf->lexeme : NULL;
            t->type = builtin_named_type(name);
            return t->type;
        }

        /* type : IDENT < typeArgumentList > */
        /* type : IDENT < typeArgumentList > ? */
        if (t->prodrule == 3 || t->prodrule == 4) {
            const char *name = t->kids[0]->leaf ? t->kids[0]->leaf->lexeme : NULL;

            if (name && strcmp(name, "Array") == 0) {
                typeptr elem = type_from_tree(t->kids[2]);
                t->type = alcarraytype(elem, -1);
                return t->type;
            }

            t->type = builtin_named_type(name);
            return t->type;
        }
    }

    if (t->symbol && strcmp(t->symbol, "typeArgumentList") == 0) {
        if (t->prodrule == 1) {
            t->type = type_from_tree(t->kids[0]);
            return t->type;
        }
    }

    t->type = any_typeptr;
    return t->type;
}

char *typename(typeptr t)
{
    static char buf[256];

    if (!t) return "(NULL)";

    if (t->basetype == ARRAY_TYPE) {
        snprintf(buf, sizeof(buf), "array[%s]",
                 typename(t->u.a.elemtype));
        return buf;
    }

    if (t->basetype == FUNC_TYPE) {
        snprintf(buf, sizeof(buf), "func");
        return buf;
    }

    if (t->basetype < FIRST_TYPE || t->basetype > LAST_TYPE)
        return "(BOGUS)";

    return typenam[t->basetype - FIRST_TYPE];
}
