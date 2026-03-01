%{
#include <stdio.h>
#include <stdlib.h>

#include "tree.h"
#include "token.h"

/* provided by flex */
int yylex(void);
extern int yylineno;
extern const char *yyfilename;   /* set in main */
extern FILE *yyin;

void yyerror(const char *s);

/* HW requirement: root of syntax tree */
tree_t *g_root = NULL;

/* for exit codes (main checks this) */
int g_syntax_errors = 0;
%}

/* IMPORTANT: this goes into k0gram.tab.h so lex.yy.c can see token_t/tree_t */
%code requires {
  #include "token.h"
  #include "tree.h"
}

/* yylval must carry either token* (from lexer) or tree* (from parser) */
%union {
  token_t *tok;
  tree_t  *tree;
}

/* =========================
   TOKENS (NOW WITH <tok>)
   ========================= */

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

/* identifiers */
%token <tok> IDENT

/* assignments */
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

%token <tok> EQ
%token <tok> NEQ
%token <tok> GTE
%token <tok> LTE

%token <tok> REF_EQ
%token <tok> REF_NEQ

%token <tok> AND
%token <tok> OR
%token <tok> NOT

%token <tok> SAFE_CALL
%token <tok> ELVIS
%token <tok> QUEST

%token <tok> RANGE
%token <tok> RANGE_UNTIL

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

/* =========================
   NONTERMINAL TYPES
   ========================= */
%type <tree> program topLevelObject functionDeclaration
%type <tree> functionValueParameters functionValueParameterList functionValueParameter
%type <tree> type typeConstraints functionBody
%type <tree> statements statement declaration
%type <tree> assignment expression
%type <tree> loopStatement controlStructureBody block

%start program

%%

program:
    topLevelObject
    { g_root = $1; $$ = $1; }
;

topLevelObject:
    functionDeclaration
    { $$ = $1; }
;

functionDeclaration:
    FUN IDENT LPAREN functionValueParameters RPAREN block
    {
      $$ = tree_node("functionDeclaration", 6, 6,
                     tree_leaf($1),
                     tree_leaf($2),
                     tree_leaf($3),
                     $4,
                     tree_leaf($5),
                     $6);
    }
;

functionValueParameters:
    /* empty */
    { $$ = NULL; }
  | functionValueParameterList
    { $$ = $1; }
;

functionValueParameterList:
    functionValueParameter
    { $$ = $1; }
  | functionValueParameterList COMMA functionValueParameter
    {
      $$ = tree_node("functionValueParameterList", 3, 3,
                     $1, tree_leaf($2), $3);
    }
;

functionValueParameter:
    IDENT COLON type
    {
      $$ = tree_node("functionValueParameter", 3, 3,
                     tree_leaf($1), tree_leaf($2), $3);
    }
;

type:
    IDENT
    { $$ = tree_node("type", 1, 1, tree_leaf($1)); }
;

typeConstraints:
    /* empty */
    { $$ = NULL; }
;

functionBody:
    block
    { $$ = $1; }
;

/* ===== Statements ===== */

statements:
    /* empty */
    { $$ = NULL; }
  | statements statement
    { $$ = tree_node("statements", 2, 2, $1, $2); }
;

statement:
    declaration
    { $$ = $1; }
  | loopStatement
    { $$ = $1; }
  | expression
    { $$ = $1; }
;

declaration:
    assignment
    { $$ = $1; }
  | functionDeclaration
    { $$ = $1; }
;

/* ===== Assignment ===== */

assignment:
    IDENT ASSIGNMENT expression
    {
      $$ = tree_node("assignment", 3, 3,
                     tree_leaf($1), tree_leaf($2), $3);
    }
  | IDENT ADD_ASSIGN expression
    {
      $$ = tree_node("assignment", 3, 3,
                     tree_leaf($1), tree_leaf($2), $3);
    }
  | IDENT SUB_ASSIGN expression
    {
      $$ = tree_node("assignment", 3, 3,
                     tree_leaf($1), tree_leaf($2), $3);
    }
;

/* ===== Expressions ===== */

expression:
    IDENT LPAREN STRINGLITERAL RPAREN
    {
      $$ = tree_node("callExpr", 4, 4,
                     tree_leaf($1), tree_leaf($2), tree_leaf($3), tree_leaf($4));
    }
  | STRINGLITERAL
    { $$ = tree_leaf($1); }
  | INTEGERLITERAL
    { $$ = tree_leaf($1); }
  | IDENT
    { $$ = tree_leaf($1); }
;

/* ===== Loop Statements (stub) ===== */

loopStatement:
    /* empty */
    { $$ = NULL; }
;

/* ===== Control Structure Body ===== */

controlStructureBody:
    block
    { $$ = $1; }
  | statement SEMI
    { $$ = tree_node("controlStructureBody", 2, 2, $1, tree_leaf($2)); }
;

/* ===== Block ===== */

block:
    LCURL statements RCURL
    {
      $$ = tree_node("block", 3, 3,
                     tree_leaf($1), $2, tree_leaf($3));
    }
;

%%

void yyerror(const char *s) {
  g_syntax_errors++;
  fprintf(stderr, "%s:%d: syntax error: %s\n",
          (yyfilename ? yyfilename : "<stdin>"),
          yylineno, s);
}