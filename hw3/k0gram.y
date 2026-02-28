%{
#include <stdio.h>

int yylex(void);
void yyerror(const char *s);
%}


//tokens
%token FUN
%token VAL
%token VAR
%token IF
%token ELSE
%token WHILE
%token FOR
%token RETURN
%token BREAK
%token CONTINUE
%token WHEN
%token IN
%token DO
%token IMPORT
%token CONST

//the literals
%token INTEGERLITERAL
%token LONGLITERAL
%token REALLITERAL
%token DOUBLELITERAL
%token BOOLEANLITERAL
%token CHARACTERLITERAL
%token STRINGLITERAL
%token NULLLITERAL

//identifiers 
%token IDENT

//assignments
%token ASSIGNMENT
%token ADD_ASSIGN
%token SUB_ASSIGN
%token MUL_ASSIGN
%token DIV_ASSIGN
%token MOD_ASSIGN

//arithmetic
%token ADD
%token SUB
%token MUL
%token DIV
%token MOD


%token INCR
%token DECR


%token EQ
%token NEQ
%token GTE
%token LTE


%token REF_EQ
%token REF_NEQ


%token AND
%token OR
%token NOT


%token SAFE_CALL
%token ELVIS
%token QUEST


%token RANGE
%token RANGE_UNTIL


%token AS


%token LPAREN
%token RPAREN
%token LCURL
%token RCURL
%token LSQUARE
%token RSQUARE
%token LANGLE
%token RANGLE
%token COMMA
%token DOT
%token COLON
%token SEMI
%token ARROW
%token DOUBLE_COLON

%start program

%%

program:
    topLevelObject
;

topLevelObject:
    functionDeclaration
;

functionDeclaration:
    FUN IDENT LPAREN functionValueParameters RPAREN
    block
;

functionValueParameters:
    /* empty */
    | functionValueParameterList
;

functionValueParameterList:
    functionValueParameter
    | functionValueParameterList COMMA functionValueParameter
;

functionValueParameter:
    IDENT COLON type
;

type:
    IDENT
;

typeConstraints:
    /* empty */
;

functionBody:
    block
;

/* ===== Statements ===== */

statements:
    /* empty */
    | statements statement
;

statement:
    declaration
    | loopStatement
    | expression
;

declaration:
    assignment
    | functionDeclaration
;

/* ===== Assignment ===== */

assignment:
    IDENT ASSIGNMENT expression
    | IDENT ADD_ASSIGN expression
    | IDENT SUB_ASSIGN expression
;

/* ===== Expressions ===== */

expression:
    IDENT LPAREN STRINGLITERAL RPAREN
    | STRINGLITERAL
    | INTEGERLITERAL
    | IDENT
;

/* ===== Loop Statements (stub) ===== */

loopStatement:
    /* empty */
;

/* ===== Control Structure Body ===== */

controlStructureBody:
    block
    | statement SEMI
;

/* ===== Block ===== */

block:
    LCURL statements RCURL
;

%%