#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "tree.h"
#include "token.h"
#include "symtab.h"
#include "k0gram.tab.h"

#define NBUCKETS 23

int g_semantic_errors = 0;

//src file name for error msgs
static const char *g_filename = "<unknown>";

static void sem_error(int line, const char *fmt, ...)
{
    va_list ap;
    fprintf(stderr, "%s:%d: semantic error: ", g_filename, line);
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fprintf(stderr, "\n");
    g_semantic_errors++;
}

//scope list
typedef struct scope_node {
    SymbolTable table;
    struct scope_node *next;
} ScopeNode;

static ScopeNode *scope_list_head = NULL;
static ScopeNode *scope_list_tail = NULL;
static SymbolTable g_predefined   = NULL;

static void scope_list_append(SymbolTable st)
{
    ScopeNode *n = (ScopeNode *)calloc(1, sizeof(ScopeNode));
    n->table = st; n->next = NULL;
    if (!scope_list_tail) scope_list_head = scope_list_tail = n;
    else { scope_list_tail->next = n; scope_list_tail = n; }
}

//tree helpers
static const char *leaf_text(struct tree *t)
{
    if (t && t->nkids == 0 && t->leaf && t->leaf->lexeme)
        return t->leaf->lexeme;
    return NULL;
}

static int leaf_line(struct tree *t)
{
    if (t && t->nkids == 0 && t->leaf) return t->leaf->line;
    return 0;
}

static int first_line(struct tree *t)
{
    if (!t) return 0;
    if (t->nkids == 0) return leaf_line(t);
    for (int i = 0; i < t->nkids; i++) {
        int l = first_line(t->kids[i]);
        if (l) return l;
    }
    return 0;
}

// parse a type node -> type_name 

static type_name parse_type_node(struct tree *t, int *out_nullable)
{
    if (!t) return unknown;

    /* nullable: the grammar wraps nullable types via question; we detect the '?'
       by checking if the symbol is "nullableType" or if there's a question leaf */
    if (out_nullable) *out_nullable = 0;

    /* If it's a raw leaf (bare type name) */
    if (t->nkids == 0) {
        const char *name = leaf_text(t);
        if (!name) return unknown;
        if (strcmp(name, "Int")     == 0) return int_type;
        if (strcmp(name, "Long")    == 0) return long_type;
        if (strcmp(name, "Double")  == 0) return double_type;
        if (strcmp(name, "Float")   == 0) return float_type;
        if (strcmp(name, "Boolean") == 0) return bool_type;
        if (strcmp(name, "Char")    == 0) return char_type;
        if (strcmp(name, "String")  == 0) return string_type;
        if (strcmp(name, "Array")   == 0) return array_type;
        if (strcmp(name, "Unit")    == 0) return unit_type;
        return unknown;
    }

    /* compound type node – first leaf is the base name */
    const char *sym = t->symbol ? t->symbol : "";

   //null wrapper
    if (strstr(sym, "nullable") || strstr(sym, "Nullable")) {
        if (out_nullable) *out_nullable = 1;
        return parse_type_node(t->kids[0], NULL);
    }

    
    for (int i = 0; i < t->nkids; i++) {
        if (t->kids[i] && t->kids[i]->nkids == 0 && t->kids[i]->leaf &&
            t->kids[i]->leaf->code == QUEST) {
            if (out_nullable) *out_nullable = 1;
        }
    }

    /* use first child's name as base type */
    return parse_type_node(t->kids[0], NULL);
}

//infer type of expression tree
static type_name infer_type(struct tree *t, SymbolTable scope);

static type_name infer_literal(struct tree *t)
{
    if (!t || t->nkids != 0 || !t->leaf) return unknown;
    switch (t->leaf->code) {
    case INTEGERLITERAL:  return int_type;
    case LONGLITERAL:     return long_type;
    case REALLITERAL:     return float_type;
    case DOUBLELITERAL:   return double_type;
    case BOOLEANLITERAL:  return bool_type;
    case CHARACTERLITERAL:return char_type;
    case STRINGLITERAL:   return string_type;
    case NULLLITERAL:     return null_type;
    default:              return unknown;
    }
}

static type_name infer_binary(struct tree *t, SymbolTable scope)
{
    /* binaryExpr has 3 kids: left, op-leaf, right */
    if (!t || t->nkids < 3) return unknown;
    type_name lhs = infer_type(t->kids[0], scope);
    type_name rhs = infer_type(t->kids[2], scope);
    struct tree *op = t->kids[1];
    if (!op || !op->leaf) return unknown;
    int opc = op->leaf->code;
    int line = op->leaf->line;

    /* relational / equality / logical -> Boolean */
    if (opc == EQ || opc == NEQ || opc == LANGLE || opc == RANGLE ||
        opc == LTE || opc == GTE || opc == REF_EQ || opc == REF_NEQ ||
        opc == AND || opc == OR) {

        /* AND/OR require Boolean operands */
        if ((opc == AND || opc == OR) &&
            lhs != unknown && lhs != any_type &&
            rhs != unknown && rhs != any_type) {
            if (lhs != bool_type)
                sem_error(line, "operator '%s' requires Boolean, got %s on left",
                          op->leaf->lexeme, name(lhs));
            if (rhs != bool_type)
                sem_error(line, "operator '%s' requires Boolean, got %s on right",
                          op->leaf->lexeme, name(rhs));
        }
        return bool_type;
    }

    /* arithmetic: +, -, *, /, % */
    if (opc == ADD || opc == SUB || opc == MUL || opc == DIV || opc == MOD) {
        /* String + anything = String concatenation */
        if (opc == ADD && (lhs == string_type || rhs == string_type))
            return string_type;

        if (lhs != unknown && lhs != any_type &&
            rhs != unknown && rhs != any_type) {
            if (!is_numeric(lhs))
                sem_error(line, "operator '%s' requires numeric type, got %s on left",
                          op->leaf->lexeme, name(lhs));
            else if (!is_numeric(rhs))
                sem_error(line, "operator '%s' requires numeric type, got %s on right",
                          op->leaf->lexeme, name(rhs));
        }
        /* result type: widest of lhs/rhs */
        if (lhs == double_type || rhs == double_type) return double_type;
        if (lhs == float_type  || rhs == float_type)  return float_type;
        if (lhs == long_type   || rhs == long_type)   return long_type;
        return int_type;
    }

    return unknown;
}

static type_name infer_type(struct tree *t, SymbolTable scope)
{
    if (!t) return unknown;

    /* leaf: literal or identifier */
    if (t->nkids == 0) {
        if (!t->leaf) return unknown;
        type_name lit = infer_literal(t);
        if (lit != unknown) return lit;
        if (t->leaf->code == IDENT && t->leaf->lexeme) {
            SymbolTableEntry e = lookupsym(scope, t->leaf->lexeme);
            if (e) return e->type;
        }
        return unknown;
    }

    const char *sym = t->symbol ? t->symbol : "";

    if (strcmp(sym, "binaryExpr")  == 0) return infer_binary(t, scope);
    if (strcmp(sym, "rangeExpr")   == 0) return int_type;   /* approximation */
    if (strcmp(sym, "elvisExpr")   == 0) {
        type_name lhs = infer_type(t->kids[0], scope);
        type_name rhs = infer_type(t->kids[2], scope);
        return (lhs != null_type) ? lhs : rhs;
    }

    if (strcmp(sym, "unaryExpr") == 0) {
        if (t->nkids >= 2) {
            struct tree *op = t->kids[0];
            type_name operand = infer_type(t->kids[1], scope);
            if (op && op->leaf && op->leaf->code == NOT) {
                int line = op->leaf->line;
                if (operand != unknown && operand != any_type &&
                    operand != bool_type)
                    sem_error(line, "operator 'not' requires Boolean, got %s",
                              name(operand));
                return bool_type;
            }
            return operand;
        }
    }

    if (strcmp(sym, "postfixExpr") == 0) {
        type_name base = infer_type(t->kids[0], scope);
        if (!is_numeric(base) && base != unknown && base != any_type) {
            int line = first_line(t->kids[1]);
            sem_error(line, "++ / -- requires numeric type, got %s", name(base));
        }
        return base;
    }

    if (strcmp(sym, "castExpr") == 0) {
        /* expression AS type */
        if (t->nkids >= 3) return parse_type_node(t->kids[2], NULL);
    }

    if (strcmp(sym, "callExpr") == 0) {
        /* callee is kids[0], args start at kids[2] */
        struct tree *callee = t->kids[0];
        const char *fname = leaf_text(callee);
        if (fname) {
            SymbolTableEntry e = lookupsym(scope, fname);
            if (e && e->type == function_type) {
                /* check arg count */
                /* count actual arguments */
                int actual = 0;
                if (t->nkids == 4) { /* callExpr with args */
                    /* argumentList is kids[2]; count commas+1 */
                    struct tree *al = t->kids[2];
                    actual = 1;
                    while (al && al->symbol &&
                           strcmp(al->symbol, "argumentList") == 0 &&
                           al->nkids == 3) {
                        actual++;
                        al = al->kids[0];
                    }
                }
                int line = first_line(callee);
                if (e->param_count >= 0 && actual != e->param_count)
                    sem_error(line,
                        "function '%s' expects %d argument(s), got %d",
                        fname, e->param_count, actual);
                return e->ret_type;
            }
            if (e && e->type == any_type) return any_type; /* built-in */
        }
        return unknown;
    }

    if (strcmp(sym, "memberExpr") == 0 || strcmp(sym, "safeMemberExpr") == 0)
        return any_type; /* member access – don't track struct field types */

    if (strcmp(sym, "indexExpr") == 0)
        return any_type;

    /* fall through: descend to first non-null kid */
    for (int i = 0; i < t->nkids; i++) {
        type_name k = infer_type(t->kids[i], scope);
        if (k != unknown) return k;
    }
    return unknown;
}

//the predefined scopes
static void add_builtin_fn(SymbolTable pre, const char *name,
                           type_name ret, int nparams)
{
    SymbolTableEntry e = insertsym(pre, name);
    e->type        = function_type;
    e->ret_type    = ret;
    e->param_count = nparams;  /* -1 = variadic */
    e->is_const    = 1;
}

static SymbolTable make_predefined_scope(void)
{
    SymbolTable pre = mksymtab(NBUCKETS, "predefined", NULL);

    /* built-in types (inserted as TYPE_ANY sentinels so "Int" is found) */
    const char *types[] = {"Int","Long","Double","Float",
                           "Boolean","Char","String","Array","Unit", NULL};
    for (int i = 0; types[i]; i++) {
        SymbolTableEntry e = insertsym(pre, types[i]);
        e->type = any_type; e->is_const = 1;
    }

    /* I/O builtins */
    add_builtin_fn(pre, "print",    unit_type, -1);
    add_builtin_fn(pre, "println",  unit_type, -1);
    add_builtin_fn(pre, "readLine", string_type, 0);

    /* math builtins */
    add_builtin_fn(pre, "maxOf",    any_type, -1);
    add_builtin_fn(pre, "minOf",    any_type, -1);
    add_builtin_fn(pre, "arrayOf",  array_type, -1);
    add_builtin_fn(pre, "intArrayOf", array_type, -1);

    return pre;
}

/* first pass: collect declarations  */
/* Returns the type encoded in a varDeclaration type-annotation subtree.
   production rules:
     1: VAL IDENT = expr           (no type annotation)
     2: VAL IDENT : type = expr
     3: VAR IDENT = expr
     4: VAR IDENT : type = expr
     5: VAR IDENT : type           (uninitialized)
*/
static type_name get_declared_type(struct tree *decl, int *out_nullable)
{
    if (out_nullable) *out_nullable = 0;
    if (!decl) return unknown;
    /* production rule 2,4,5 have a type annotation at kids[3] */
    if (decl->nkids >= 4 && decl->kids[3]) {
        struct tree *tnode = decl->kids[3];
        /* Detect nullable by checking if any leaf in tnode is QUEST */
        int nullable = 0;
        type_name tk = parse_type_node(tnode, &nullable);
        if (out_nullable) *out_nullable = nullable;
        return tk;
    }
    return unknown;
}

/* parse function return type annotation from functionDeclaration.
   Grammar: FUN IDENT ( params ) : type block
   kids layout varies; we search for a COLON leaf followed by a type node. */
static type_name get_fn_return_type(struct tree *fdecl)
{
    if (!fdecl) return unit_type;
    int saw_colon = 0;
    for (int i = 0; i < fdecl->nkids; i++) {
        struct tree *k = fdecl->kids[i];
        if (!k) continue;
        if (k->nkids == 0 && k->leaf && k->leaf->code == COLON) {
            saw_colon = 1; continue;
        }
        if (saw_colon && k->symbol && strncmp(k->symbol, "type", 4) == 0)
            return parse_type_node(k, NULL);
        if (saw_colon && k->nkids == 0 && k->leaf && k->leaf->code == IDENT)
            return parse_type_node(k, NULL);
        /* block signals we've passed the return type position */
        if (k->symbol && strcmp(k->symbol, "block") == 0) break;
    }
    return unit_type;
}

static void collect_scopes(struct tree *t, SymbolTable current);

static void collect_fn_params(struct tree *t, SymbolTable fn_scope)
{
    if (!t) return;
    if (t->symbol && strcmp(t->symbol, "functionValueParameter") == 0) {
        const char *n = leaf_text(t->kids[0]);
        if (n) {
            if (lookup_current_scope(fn_scope, n)) {
                sem_error(first_line(t->kids[0]),
                          "redeclared parameter '%s'", n);
            } else {
                SymbolTableEntry e = insertsym(fn_scope, n);
                int nullable = 0;
                type_name pt = (t->nkids >= 3) ?
                    parse_type_node(t->kids[2], &nullable) : unknown;
                e->type        = pt;
                e->is_nullable = nullable;
                e->is_const    = 1; /* params are immutable by default */
                if (t->kids[0] && t->kids[0]->leaf)
                    e->declared_type = strdup(name(pt));
            }
        }
        return;
    }
    for (int i = 0; i < t->nkids; i++)
        collect_fn_params(t->kids[i], fn_scope);
}

static void collect_scopes(struct tree *t, SymbolTable current)
{
    if (!t) return;
    if (t->nkids == 0) return;

    /* ---- functionDeclaration -------------------------------------------- */
    if (t->symbol && strcmp(t->symbol, "functionDeclaration") == 0) {
        const char *fname = leaf_text(t->kids[1]);
        if (fname) {
            if (lookup_current_scope(current, fname)) {
                sem_error(first_line(t->kids[1]),
                          "redeclared function '%s'", fname);
            } else {
                SymbolTableEntry fe = insertsym(current, fname);
                fe->type      = function_type;
                fe->is_const  = 1;
                fe->ret_type  = get_fn_return_type(t);
                fe->param_count = 0; /* will be updated below */
            }
        }

        SymbolTable fn_scope = mksymtab(NBUCKETS,
                                        fname ? fname : "fn", current);
        scope_list_append(fn_scope);
        collect_fn_params(t, fn_scope);

        /* update param_count on the entry */
        if (fname) {
            SymbolTableEntry fe = lookupsym(current, fname);
            if (fe) fe->param_count = fn_scope->nEntries;
        }

        for (int i = 0; i < t->nkids; i++)
            collect_scopes(t->kids[i], fn_scope);
        return;
    }

    /* ---- varDeclaration ------------------------------------------------- */
    if (t->symbol && strcmp(t->symbol, "varDeclaration") == 0) {
        const char *n = leaf_text(t->kids[1]);
        if (n) {
            if (lookup_current_scope(current, n)) {
                sem_error(first_line(t->kids[1]),
                          "redeclared variable '%s'", n);
            } else {
                SymbolTableEntry e = insertsym(current, n);
                int nullable = 0;
                type_name ann = get_declared_type(t, &nullable);
                e->is_nullable = nullable;
                e->declared_type = ann != unknown ? strdup(name(ann)) : NULL;

                /* val vs var */
                if (t->kids[0] && t->kids[0]->leaf &&
                    strcmp(t->kids[0]->leaf->lexeme, "val") == 0)
                    e->is_const = 1;

                /* if we have an initialiser expression, infer its type */
                struct tree *init_expr = NULL;
                if (t->nkids == 4) init_expr = t->kids[3];
                else if (t->nkids == 6) init_expr = t->kids[5];

                type_name inf = init_expr ?
                    infer_type(init_expr, current) : unknown;

                /* use annotation if present, otherwise infer */
                e->type = (ann != unknown) ? ann : inf;

                /* nullability: assigning null to non-nullable */
                if (inf == null_type && !nullable && ann != unknown) {
                    sem_error(first_line(t->kids[1]),
                        "cannot assign null to non-nullable variable '%s'", name);
                }
                /* type mismatch between annotation and initialiser */
                if (ann != unknown && inf != unknown &&
                    inf != null_type && !types_compatible(ann, inf)) {
                    sem_error(first_line(t->kids[1]),
                        "type mismatch: variable '%s' declared %s but initialised with %s",
                        name, name(ann), name(inf));
                }
            }
        }
        for (int i = 0; i < t->nkids; i++)
            collect_scopes(t->kids[i], current);
        return;
    }

    /* ---- forStatement loop variable ------------------------------------- */
    if (t->symbol && strcmp(t->symbol, "forStatement") == 0) {
        /* FOR ( IDENT IN expr ) body */
        const char *name = leaf_text(t->kids[2]);
        if (name) {
            if (!lookup_current_scope(current, name)) {
                SymbolTableEntry e = insertsym(current, name);
                e->type     = int_type;  /* conservative: range elements */
                e->is_const = 1;         /* loop var is immutable in Kotlin */
            }
        }
        for (int i = 0; i < t->nkids; i++)
            collect_scopes(t->kids[i], current);
        return;
    }

    for (int i = 0; i < t->nkids; i++)
        collect_scopes(t->kids[i], current);
}

/* ---- second pass: check assignments, types, undeclared ------------------- */

static int is_decl_context(struct tree *parent, int kid_index)
{
    if (!parent || !parent->symbol) return 0;
    if (strcmp(parent->symbol, "varDeclaration") == 0 && kid_index == 1) return 1;
    if (strcmp(parent->symbol, "functionDeclaration") == 0 && kid_index == 1) return 1;
    if (strcmp(parent->symbol, "functionValueParameter") == 0 && kid_index == 0) return 1;
    if (strcmp(parent->symbol, "forStatement") == 0 && kid_index == 2) return 1;
    return 0;
}

static SymbolTable find_fn_scope(SymbolTable current, const char *fname)
{
    for (ScopeNode *n = scope_list_head; n; n = n->next) {
        if (n->table->parent == current &&
            fname && strcmp(n->table->name, fname) == 0)
            return n->table;
    }
    return NULL;
}

static void check_tree(struct tree *t, struct tree *parent,
                        int kid_index, SymbolTable current)
{
    if (!t) return;

    /* ---- leaf: check undeclared identifiers ----------------------------- */
    if (t->nkids == 0) {
        if (t->leaf && t->leaf->code == IDENT && t->leaf->lexeme) {
            if (!is_decl_context(parent, kid_index)) {
                if (!lookupsym(current, t->leaf->lexeme)) {
                    sem_error(t->leaf->line,
                              "undeclared identifier '%s'", t->leaf->lexeme);
                }
            }
        }
        return;
    }

    const char *sym = t->symbol ? t->symbol : "";

    /* ---- assignment: mutability + type checks --------------------------- */
    if (strcmp(sym, "assignment") == 0) {
        /* kids[0]=IDENT, kids[1]=op, kids[2]=expr */
        const char *n = leaf_text(t->kids[0]);
        int line = first_line(t->kids[0]);
        if (n) {
            SymbolTableEntry e = lookupsym(current, n);
            if (!e) {
                sem_error(line, "undeclared identifier '%s'", n);
            } else {
                /* mutability check */
                if (e->is_const) {
                    sem_error(line,
                        "cannot reassign val '%s'", n);
                }
                /* type check */
                type_name rhs = infer_type(t->kids[2], current);
                /* null assigned to non-nullable? */
                if (rhs == null_type && !e->is_nullable) {
                    sem_error(line,
                        "cannot assign null to non-nullable variable '%s'", n);
                } else if (rhs != unknown && rhs != null_type &&
                           e->type != unknown && e->type != any_type &&
                           !types_compatible(e->type, rhs)) {
                    sem_error(line,
                        "type mismatch: cannot assign %s to variable '%s' of type %s",
                        name(rhs), n, name(e->type));
                }
            }
        }
        /* recurse into rhs expression */
        check_tree(t->kids[2], t, 2, current);
        return;
    }

    /* ---- functionDeclaration: switch scope ------------------------------ */
    if (strcmp(sym, "functionDeclaration") == 0) {
        const char *fname = leaf_text(t->kids[1]);
        SymbolTable fn_scope = find_fn_scope(current, fname);
        if (!fn_scope) fn_scope = current;
        for (int i = 0; i < t->nkids; i++)
            check_tree(t->kids[i], t, i, fn_scope);
        return;
    }

    /* ---- callExpr: already type-checked inside infer_type -------------- */
    /* Still recurse so arguments get undeclared-var checks. */
    if (strcmp(sym, "callExpr") == 0) {
        /* trigger type inference (which emits errors) */
        infer_type(t, current);
        for (int i = 0; i < t->nkids; i++)
            check_tree(t->kids[i], t, i, current);
        return;
    }

    /* ---- binaryExpr / unaryExpr: trigger type check -------------------- */
    if (strcmp(sym, "binaryExpr") == 0 ||
        strcmp(sym, "unaryExpr")  == 0 ||
        strcmp(sym, "postfixExpr") == 0) {
        infer_type(t, current);
        for (int i = 0; i < t->nkids; i++)
            check_tree(t->kids[i], t, i, current);
        return;
    }

    /* ---- ifStatement / whileStatement: condition must be Boolean -------- */
    if (strcmp(sym, "ifStatement") == 0 || strcmp(sym, "whileStatement") == 0) {
        /* condition is kids[2] */
        if (t->nkids >= 3 && t->kids[2]) {
            type_name cond = infer_type(t->kids[2], current);
            if (cond != bool_type && cond != unknown && cond != any_type) {
                int line = first_line(t->kids[2]);
                sem_error(line,
                    "condition must be Boolean, got %s", name(cond));
            }
        }
        for (int i = 0; i < t->nkids; i++)
            check_tree(t->kids[i], t, i, current);
        return;
    }

    /* default: recurse */
    for (int i = 0; i < t->nkids; i++)
        check_tree(t->kids[i], t, i, current);
}

/* ---- public API ---------------------------------------------------------- */

void printsyms(struct tree *t)
{
    if (!t) return;
    if (t->nkids == 0) {
        if (t->leaf && t->leaf->code == IDENT && t->leaf->lexeme)
            printf("%s\n", t->leaf->lexeme);
        return;
    }
    for (int i = 0; i < t->nkids; i++) printsyms(t->kids[i]);
}

SymbolTable buildsymtabs(struct tree *root, const char *filename)
{
    g_filename   = filename ? filename : "<unknown>";

    g_predefined = make_predefined_scope();
    SymbolTable global = mksymtab(NBUCKETS, "main", g_predefined);
    scope_list_append(global);

    collect_scopes(root, global);
    return global;
}

void check_undeclared(struct tree *root, SymbolTable global)
{
    check_tree(root, NULL, -1, global);
}

void printsymtabs(void)
{
    if (g_semantic_errors > 0) return;
    for (ScopeNode *n = scope_list_head; n; n = n->next)
        printsymtab(n->table);
}

void freesymtabs(void)
{
    ScopeNode *n = scope_list_head;
    while (n) {
        ScopeNode *next = n->next;
        freesymtab(n->table);
        free(n);
        n = next;
    }
    scope_list_head = scope_list_tail = NULL;
    if (g_predefined) { freesymtab(g_predefined); g_predefined = NULL; }
}
