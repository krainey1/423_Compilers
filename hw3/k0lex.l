%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "k0gram.tab.h"   /* Bison-generated token codes + yylval */
#include "token.h"        /* token_t + token_create() */

int lineno = 1;
char *current_filename;

/* HW exit-code rule: lexical errors -> exit status 1 (handled in main) */
int g_lex_errors = 0;

int comment_depth = 0;

/* Helper: report lexical error but DON'T exit here (valgrind/exit codes cleaner) */
static void lexical_error(const char *msg) {
   fprintf(stderr, "%s:%d: lexical error: %s\n",
           current_filename ? current_filename : "<stdin>", lineno, msg);
   g_lex_errors++;
}

/* Helper: set yylval.tok with a heap token */
static void set_tok(int category) {
   /* token_create should strdup yytext internally */
   yylval.tok = token_create(category, yytext, lineno);
}
%}

%option noyywrap
%x COMMENT STRING

%%

\n              { lineno++; }
[ \t\r\f]+      ;

"#".*           ;
"//".*          ;

/* ===== Block comments with nesting ===== */

"/*" {
    comment_depth = 1;
    BEGIN(COMMENT);
}

<COMMENT>"/*"   { comment_depth++; }

<COMMENT>"*/" {
    comment_depth--;
    if (comment_depth == 0)
        BEGIN(INITIAL);
}

<COMMENT>\n     { lineno++; }
<COMMENT>.      ;

<COMMENT><<EOF>> {
    lexical_error("unterminated comment");
    /* stop scanning: parser can finish / main will exit 1 */
    return 0;
}

/* ===== Keywords / reserved words ===== */

"fun"      { set_tok(FUN); return FUN; }
"val"      { set_tok(VAL); return VAL; }
"var"      { set_tok(VAR); return VAR; }
"if"       { set_tok(IF); return IF; }
"else"     { set_tok(ELSE); return ELSE; }
"while"    { set_tok(WHILE); return WHILE; }
"for"      { set_tok(FOR); return FOR; }
"return"   { set_tok(RETURN); return RETURN; }
"break"    { set_tok(BREAK); return BREAK; }
"continue" { set_tok(CONTINUE); return CONTINUE; }
"when"     { set_tok(WHEN); return WHEN; }
"in"       { set_tok(IN); return IN; }
"do"       { set_tok(DO); return DO; }
"import"   { set_tok(IMPORT); return IMPORT; }
"const"    { set_tok(CONST); return CONST; }
"as"       { set_tok(AS); return AS; }

"true"     { set_tok(BOOLEANLITERAL); return BOOLEANLITERAL; }
"false"    { set_tok(BOOLEANLITERAL); return BOOLEANLITERAL; }
"null"     { set_tok(NULLLITERAL); return NULLLITERAL; }

/* ===== Identifiers ===== */

[a-zA-Z_][a-zA-Z0-9_]* {
    set_tok(IDENT);
    return IDENT;
}

/* ===== Numeric literals ===== */

([0-9]+"."[0-9]*([eE][+-]?[0-9]+)?)[dD] {
    set_tok(DOUBLELITERAL);
    return DOUBLELITERAL;
}

([0-9]+"."[0-9]*([eE][+-]?[0-9]+)?|[0-9]+[eE][+-]?[0-9]+) {
    set_tok(REALLITERAL);
    return REALLITERAL;
}

[0-9]+[lL] {
    set_tok(LONGLITERAL);
    return LONGLITERAL;
}

[0-9]+ {
    set_tok(INTEGERLITERAL);
    return INTEGERLITERAL;
}

/* ===== Character literal ===== */

\'([^\\\n]|\\.)\' {
    set_tok(CHARACTERLITERAL);
    return CHARACTERLITERAL;
}

\'([^\\\n]|\\.)* {
    lexical_error("unterminated character literal");
    /* try to recover by discarding; no token returned */
}

/* ===== String literal (state machine) ===== */
/* We keep your STRING state approach but simplify: we still return STRINGLITERAL once closed.
   token_create() stores yytext at the final match; here, yytext is just "\"" or "\\n", etc.
   To preserve your original behavior (storing entire raw string), we'd need a custom builder
   in token_create or a separate buffer. For HW3 tree-printing, having lexeme is usually enough.
*/

\" {
    BEGIN(STRING);
    /* Start of string: we’ll accumulate in a static buffer */
    /* We'll build the full raw string including quotes. */
    yyextra = NULL;
    /* Use Flex's yyextra? Not enabled here. We'll use a static buffer instead: */
    /* See below: we keep a global dynamic buffer. */
    yymore(); /* keep scanning into yytext buffer */
}

<STRING>\" {
    BEGIN(INITIAL);
    /* At this point yytext contains the whole string including quotes because of yymore() */
    set_tok(STRINGLITERAL);
    return STRINGLITERAL;
}

<STRING>\n {
    lexical_error("unterminated string literal");
    BEGIN(INITIAL);
}

<STRING><<EOF>> {
    lexical_error("unterminated string literal");
    BEGIN(INITIAL);
    return 0;
}

/* Accept escapes + any chars inside string */
<STRING>\\n   { yymore(); }
<STRING>\\t   { yymore(); }
<STRING>\\\\  { yymore(); }
<STRING>\\\"  { yymore(); }
<STRING>.     { yymore(); }

/* ===== Multi-char operators ===== */

"==="    { set_tok(REF_EQ); return REF_EQ; }
"!=="    { set_tok(REF_NEQ); return REF_NEQ; }
"=="     { set_tok(EQ); return EQ; }
"!="     { set_tok(NEQ); return NEQ; }
">="     { set_tok(GTE); return GTE; }
"<="     { set_tok(LTE); return LTE; }
"..<"    { set_tok(RANGE_UNTIL); return RANGE_UNTIL; }
".."     { set_tok(RANGE); return RANGE; }
"&&"     { set_tok(AND); return AND; }
"||"     { set_tok(OR); return OR; }
"++"     { set_tok(INCR); return INCR; }
"--"     { set_tok(DECR); return DECR; }
"+="     { set_tok(ADD_ASSIGN); return ADD_ASSIGN; }
"-="     { set_tok(SUB_ASSIGN); return SUB_ASSIGN; }
"*="     { set_tok(MUL_ASSIGN); return MUL_ASSIGN; }
"/="     { set_tok(DIV_ASSIGN); return DIV_ASSIGN; }
"%="     { set_tok(MOD_ASSIGN); return MOD_ASSIGN; }
"->"     { set_tok(ARROW); return ARROW; }
"::"     { set_tok(DOUBLE_COLON); return DOUBLE_COLON; }
"?."     { set_tok(SAFE_CALL); return SAFE_CALL; }
"?:"     { set_tok(ELVIS); return ELVIS; }

/* ===== Single-char tokens ===== */

"+"  { set_tok(ADD); return ADD; }
"-"  { set_tok(SUB); return SUB; }
"*"  { set_tok(MUL); return MUL; }
"/"  { set_tok(DIV); return DIV; }
"%"  { set_tok(MOD); return MOD; }
"="  { set_tok(ASSIGNMENT); return ASSIGNMENT; }
"!"  { set_tok(NOT); return NOT; }
"?"  { set_tok(QUEST); return QUEST; }
";"  { set_tok(SEMI); return SEMI; }
":"  { set_tok(COLON); return COLON; }
","  { set_tok(COMMA); return COMMA; }
"."  { set_tok(DOT); return DOT; }
"("  { set_tok(LPAREN); return LPAREN; }
")"  { set_tok(RPAREN); return RPAREN; }
"{"  { set_tok(LCURL); return LCURL; }
"}"  { set_tok(RCURL); return RCURL; }
"["  { set_tok(LSQUARE); return LSQUARE; }
"]"  { set_tok(RSQUARE); return RSQUARE; }
"<"  { set_tok(LANGLE); return LANGLE; }
">"  { set_tok(RANGLE); return RANGLE; }

/* ===== Catch-all ===== */

. {
    lexical_error("illegal character");
    /* discard and continue */
}

<<EOF>> {
    return 0;  /* EOF for bison/flex convention */
}

%%