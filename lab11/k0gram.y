%code requires {
#include "tree.h"
#include "token.h"
}

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"
#include "token.h"

#define YYDEBUG 1
int  yylex(void);
void yyerror(const char *s);

extern int   lineno;
extern char *current_filename;

tree_t *g_root         = NULL;
extern int g_lex_errors;
int     g_syntax_errors = 0;
%}

/* tree_t is already defined as an explicit structure in tree.h */
%union {
    token_t       *tok;
    tree_t       *tree;
}



/* keywords */
%token <tok> FUN
%token <tok> VAL
%token <tok> VAR
%token <tok> IF
%token <tok> ELSE
%token <tok> WHILE
%token <tok> FOR
%token <tok> RETURN
%token <tok> BREAK
%token <tok> CONTINUE
%token <tok> WHEN
%token <tok> IN
%token <tok> DO
%token <tok> IMPORT
%token <tok> CONST

/* literals */
%token <tok> INTEGERLITERAL
%token <tok> LONGLITERAL
%token <tok> REALLITERAL
%token <tok> DOUBLELITERAL
%token <tok> BOOLEANLITERAL
%token <tok> CHARACTERLITERAL
%token <tok> STRINGLITERAL
%token <tok> NULLLITERAL

/* identifier */
%token <tok> IDENT

/* assignment operators */
%token <tok> ASSIGNMENT
%token <tok> ADD_ASSIGN
%token <tok> SUB_ASSIGN
%token <tok> MUL_ASSIGN
%token <tok> DIV_ASSIGN
%token <tok> MOD_ASSIGN

/* arithmetic */
%token <tok> ADD
%token <tok> SUB
%token <tok> MUL
%token <tok> DIV
%token <tok> MOD
%token <tok> INCR
%token <tok> DECR

/* comparison */
%token <tok> EQ
%token <tok> NEQ
%token <tok> GTE
%token <tok> LTE
%token <tok> REF_EQ
%token <tok> REF_NEQ

/* logical */
%token <tok> AND
%token <tok> OR
%token <tok> NOT

/* nullable */
%token <tok> SAFE_CALL
%token <tok> ELVIS
%token <tok> QUEST

/* ranges */
%token <tok> RANGE
%token <tok> RANGE_UNTIL

/* cast */
%token <tok> AS

/* punctuation */
%token <tok> LPAREN
%token <tok> RPAREN
%token <tok> LCURL
%token <tok> RCURL
%token <tok> LSQUARE
%token <tok> RSQUARE
%token <tok> LANGLE
%token <tok> RANGLE
%token <tok> COMMA
%token <tok> DOT
%token <tok> COLON
%token <tok> SEMI
%token <tok> ARROW
%token <tok> DOUBLE_COLON


%left  OR
%left  AND
%left  EQ NEQ REF_EQ REF_NEQ
%left  LANGLE RANGLE LTE GTE
%left  RANGE RANGE_UNTIL
%left  ADD SUB
%left  MUL DIV MOD
%right NOT
%left  AS
%left  DOT SAFE_CALL DOUBLE_COLON
%left  LPAREN LSQUARE
%nonassoc INCR DECR


%type <tree> program topLevelObjects topLevelObject
%type <tree> functionDeclaration
%type <tree> functionValueParameters functionValueParameterList functionValueParameter
%type <tree> type typeArgumentList
%type <tree> block statements statement
%type <tree> varDeclaration assignment
%type <tree> ifStatement whileStatement forStatement jumpStatement
%type <tree> controlStructureBody
%type <tree> expression argumentList literal

%start program

%%



program
    : topLevelObjects
        {
            $$ = tree_node("program", 1, 1, $1);
            g_root = $$;
        }
    ;

topLevelObjects
    : topLevelObject
        {
            $$ = $1;
        }
    | topLevelObjects topLevelObject
        {
            $$ = tree_node("topLevelObjects", 1, 2, $1, $2);
        }
    ;

topLevelObject
    : functionDeclaration
        {
            $$ = tree_node("topLevelObject", 1, 1, $1);
        }
    ;



functionDeclaration
    : FUN IDENT LPAREN functionValueParameters RPAREN block
        {
            $$ = tree_node("functionDeclaration", 1, 6,
                           tree_leaf($1), tree_leaf($2),
                           tree_leaf($3), $4,
                           tree_leaf($5), $6);
        }
    | FUN IDENT LPAREN functionValueParameters RPAREN COLON type block
        {
            $$ = tree_node("functionDeclaration", 2, 8,
                           tree_leaf($1), tree_leaf($2),
                           tree_leaf($3), $4,
                           tree_leaf($5), tree_leaf($6),
                           $7, $8);
        }
    | FUN IDENT LPAREN functionValueParameters RPAREN ASSIGNMENT expression
        {
            $$ = tree_node("functionDeclaration", 3, 7,
                           tree_leaf($1), tree_leaf($2),
                           tree_leaf($3), $4,
                           tree_leaf($5), tree_leaf($6),
                           $7);
        }
    | FUN IDENT LPAREN functionValueParameters RPAREN COLON type ASSIGNMENT expression
        {
            $$ = tree_node("functionDeclaration", 4, 9,
                           tree_leaf($1), tree_leaf($2),
                           tree_leaf($3), $4,
                           tree_leaf($5), tree_leaf($6),
                           $7, tree_leaf($8), $9);
        }
    ;

functionValueParameters
    : /* empty */
        { $$ = NULL; }
    | functionValueParameterList
        { $$ = $1; }
    ;

functionValueParameterList
    : functionValueParameter
        { $$ = $1; }
    | functionValueParameterList COMMA functionValueParameter
        {
            $$ = tree_node("functionValueParameterList", 1, 3,
                           $1, tree_leaf($2), $3);
        }
    ;

functionValueParameter
    : IDENT COLON type
        {
            $$ = tree_node("functionValueParameter", 1, 3,
                           tree_leaf($1), tree_leaf($2), $3);
        }
    | IDENT COLON type ASSIGNMENT expression
        {
            $$ = tree_node("functionValueParameter", 2, 5,
                           tree_leaf($1), tree_leaf($2),
                           $3, tree_leaf($4), $5);
        }
    ;

type
    : IDENT
        { $$ = tree_node("type", 1, 1, tree_leaf($1)); }
    | IDENT QUEST
        { $$ = tree_node("type", 2, 2, tree_leaf($1), tree_leaf($2)); }
    | IDENT LANGLE typeArgumentList RANGLE
        {
            $$ = tree_node("type", 3, 4,
                           tree_leaf($1), tree_leaf($2),
                           $3, tree_leaf($4));
        }
    | IDENT LANGLE typeArgumentList RANGLE QUEST
        {
            $$ = tree_node("type", 4, 5,
                           tree_leaf($1), tree_leaf($2),
                           $3, tree_leaf($4), tree_leaf($5));
        }
    ;

typeArgumentList
    : type
        { $$ = $1; }
    | typeArgumentList COMMA type
        {
            $$ = tree_node("typeArgumentList", 1, 3,
                           $1, tree_leaf($2), $3);
        }
    ;



block
    : LCURL statements RCURL
        {
            $$ = tree_node("block", 1, 3,
                           tree_leaf($1), $2, tree_leaf($3));
        }
    ;



statements
    : /* empty */
        { $$ = NULL; }
    | statements statement
        {
            if ($1 == NULL)
                $$ = $2;
            else
                $$ = tree_node("statements", 1, 2, $1, $2);
        }
    ;


statement
    : varDeclaration        { $$ = $1; }
    | functionDeclaration   { $$ = $1; }
    | ifStatement           { $$ = $1; }
    | whileStatement        { $$ = $1; }
    | forStatement          { $$ = $1; }
    | jumpStatement         { $$ = $1; }
    | assignment            { $$ = $1; }
    | expression SEMI
        { $$ = tree_node("exprStatement", 1, 2, $1, tree_leaf($2)); }
    | expression
        { $$ = $1; }
    ;



varDeclaration
    : VAL IDENT ASSIGNMENT expression
        {
            $$ = tree_node("varDeclaration", 1, 4,
                           tree_leaf($1), tree_leaf($2),
                           tree_leaf($3), $4);
        }
    | VAL IDENT COLON type ASSIGNMENT expression
        {
            $$ = tree_node("varDeclaration", 2, 6,
                           tree_leaf($1), tree_leaf($2),
                           tree_leaf($3), $4,
                           tree_leaf($5), $6);
        }
    | VAR IDENT ASSIGNMENT expression
        {
            $$ = tree_node("varDeclaration", 3, 4,
                           tree_leaf($1), tree_leaf($2),
                           tree_leaf($3), $4);
        }
    | VAR IDENT COLON type ASSIGNMENT expression
        {
            $$ = tree_node("varDeclaration", 4, 6,
                           tree_leaf($1), tree_leaf($2),
                           tree_leaf($3), $4,
                           tree_leaf($5), $6);
        }
    | VAR IDENT COLON type
        {
            $$ = tree_node("varDeclaration", 5, 4,
                           tree_leaf($1), tree_leaf($2),
                           tree_leaf($3), $4);
        }
    ;



assignment
    : IDENT ASSIGNMENT expression
        {
            $$ = tree_node("assignment", 1, 3,
                           tree_leaf($1), tree_leaf($2), $3);
        }
    | IDENT ADD_ASSIGN expression
        {
            $$ = tree_node("assignment", 2, 3,
                           tree_leaf($1), tree_leaf($2), $3);
        }
    | IDENT SUB_ASSIGN expression
        {
            $$ = tree_node("assignment", 3, 3,
                           tree_leaf($1), tree_leaf($2), $3);
        }
    | IDENT MUL_ASSIGN expression
        {
            $$ = tree_node("assignment", 4, 3,
                           tree_leaf($1), tree_leaf($2), $3);
        }
    | IDENT DIV_ASSIGN expression
        {
            $$ = tree_node("assignment", 5, 3,
                           tree_leaf($1), tree_leaf($2), $3);
        }
    | IDENT MOD_ASSIGN expression
        {
            $$ = tree_node("assignment", 6, 3,
                           tree_leaf($1), tree_leaf($2), $3);
        }
    ;



ifStatement
    : IF LPAREN expression RPAREN controlStructureBody
        {
            $$ = tree_node("ifStatement", 1, 5,
                           tree_leaf($1), tree_leaf($2),
                           $3, tree_leaf($4), $5);
        }
    | IF LPAREN expression RPAREN controlStructureBody ELSE controlStructureBody
        {
            $$ = tree_node("ifStatement", 2, 7,
                           tree_leaf($1), tree_leaf($2),
                           $3, tree_leaf($4),
                           $5, tree_leaf($6), $7);
        }
    ;

whileStatement
    : WHILE LPAREN expression RPAREN controlStructureBody
        {
            $$ = tree_node("whileStatement", 1, 5,
                           tree_leaf($1), tree_leaf($2),
                           $3, tree_leaf($4), $5);
        }
    ;

forStatement
    : FOR LPAREN IDENT IN expression RPAREN controlStructureBody
        {
            $$ = tree_node("forStatement", 1, 7,
                           tree_leaf($1), tree_leaf($2),
                           tree_leaf($3), tree_leaf($4),
                           $5, tree_leaf($6), $7);
        }
    ;

jumpStatement
    : RETURN
        { $$ = tree_node("jumpStatement", 1, 1, tree_leaf($1)); }
    | RETURN expression
        { $$ = tree_node("jumpStatement", 2, 2, tree_leaf($1), $2); }
    | BREAK
        { $$ = tree_node("jumpStatement", 3, 1, tree_leaf($1)); }
    | CONTINUE
        { $$ = tree_node("jumpStatement", 4, 1, tree_leaf($1)); }
    ;

controlStructureBody
    : block     { $$ = $1; }
    | statement { $$ = $1; }
    ;



expression
    : literal
        { $$ = $1; }
    | IDENT
        { $$ = tree_leaf($1); }
    | expression LPAREN RPAREN
        {
            $$ = tree_node("callExpr", 1, 3,
                           $1, tree_leaf($2), tree_leaf($3));
        }
    | expression LPAREN argumentList RPAREN
        {
            $$ = tree_node("callExpr", 2, 4,
                           $1, tree_leaf($2), $3, tree_leaf($4));
        }
    | expression LSQUARE expression RSQUARE
        {
            $$ = tree_node("indexExpr", 1, 4,
                           $1, tree_leaf($2), $3, tree_leaf($4));
        }
    | expression DOT IDENT
        {
            $$ = tree_node("memberExpr", 1, 3,
                           $1, tree_leaf($2), tree_leaf($3));
        }
    | expression SAFE_CALL IDENT
        {
            $$ = tree_node("safeMemberExpr", 1, 3,
                           $1, tree_leaf($2), tree_leaf($3));
        }
    | expression AS type
        {
            $$ = tree_node("castExpr", 1, 3,
                           $1, tree_leaf($2), $3);
        }
    | expression ADD expression
        { $$ = tree_node("binaryExpr", 1,  3, $1, tree_leaf($2), $3); }
    | expression SUB expression
        { $$ = tree_node("binaryExpr", 2,  3, $1, tree_leaf($2), $3); }
    | expression MUL expression
        { $$ = tree_node("binaryExpr", 3,  3, $1, tree_leaf($2), $3); }
    | expression DIV expression
        { $$ = tree_node("binaryExpr", 4,  3, $1, tree_leaf($2), $3); }
    | expression MOD expression
        { $$ = tree_node("binaryExpr", 5,  3, $1, tree_leaf($2), $3); }
    | SUB expression
        { $$ = tree_node("unaryExpr",  1,  2, tree_leaf($1), $2); }
    | NOT expression
        { $$ = tree_node("unaryExpr",  2,  2, tree_leaf($1), $2); }
    | expression INCR
        { $$ = tree_node("postfixExpr", 1, 2, $1, tree_leaf($2)); }
    | expression DECR
        { $$ = tree_node("postfixExpr", 2, 2, $1, tree_leaf($2)); }
    | expression EQ      expression
        { $$ = tree_node("binaryExpr", 6,  3, $1, tree_leaf($2), $3); }
    | expression NEQ     expression
        { $$ = tree_node("binaryExpr", 7,  3, $1, tree_leaf($2), $3); }
    | expression LANGLE  expression
        { $$ = tree_node("binaryExpr", 8,  3, $1, tree_leaf($2), $3); }
    | expression RANGLE  expression
        { $$ = tree_node("binaryExpr", 9,  3, $1, tree_leaf($2), $3); }
    | expression LTE     expression
        { $$ = tree_node("binaryExpr", 10, 3, $1, tree_leaf($2), $3); }
    | expression GTE     expression
        { $$ = tree_node("binaryExpr", 11, 3, $1, tree_leaf($2), $3); }
    | expression REF_EQ  expression
        { $$ = tree_node("binaryExpr", 12, 3, $1, tree_leaf($2), $3); }
    | expression REF_NEQ expression
        { $$ = tree_node("binaryExpr", 13, 3, $1, tree_leaf($2), $3); }
    | expression AND expression
        { $$ = tree_node("binaryExpr", 14, 3, $1, tree_leaf($2), $3); }
    | expression OR  expression
        { $$ = tree_node("binaryExpr", 15, 3, $1, tree_leaf($2), $3); }
    | expression RANGE       expression
        { $$ = tree_node("rangeExpr",  1,  3, $1, tree_leaf($2), $3); }
    | expression RANGE_UNTIL expression
        { $$ = tree_node("rangeExpr",  2,  3, $1, tree_leaf($2), $3); }
    | expression ELVIS expression
        { $$ = tree_node("elvisExpr",  1,  3, $1, tree_leaf($2), $3); }
    | LPAREN expression RPAREN
        { $$ = $2; }
    ;

argumentList
    : expression
        { $$ = $1; }
    | argumentList COMMA expression
        {
            $$ = tree_node("argumentList", 1, 3,
                           $1, tree_leaf($2), $3);
        }
    ;



literal
    : INTEGERLITERAL    { $$ = tree_leaf($1); }
    | LONGLITERAL       { $$ = tree_leaf($1); }
    | REALLITERAL       { $$ = tree_leaf($1); }
    | DOUBLELITERAL     { $$ = tree_leaf($1); }
    | BOOLEANLITERAL    { $$ = tree_leaf($1); }
    | CHARACTERLITERAL  { $$ = tree_leaf($1); }
    | STRINGLITERAL     { $$ = tree_leaf($1); }
    | NULLLITERAL       { $$ = tree_leaf($1); }
    ;

%%

void yyerror(const char *s) {
    g_syntax_errors++;
    fprintf(stderr, "%s:%d: syntax error: %s\n",
            current_filename ? current_filename : "<stdin>",
            lineno, s);
}

const char *yyname(int sym) {
    /* yytname starts at index 3 for user tokens: 0=$end,1=error,2=$undefined */
    /* tokens start at 258; FUN is the first %token (index 3 in yytname)      */
    int idx = sym - FUN + 3;
    if (idx < 0 || idx >= (int)(sizeof(yytname)/sizeof(yytname[0])))
        return "UNKNOWN";
    return yytname[idx];
}
