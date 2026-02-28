%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "k0gram.tab.h"
#include "token.h"     /* token_t, token_create(), token_free() */

/* Your existing state */
int lineno = 1;
char *current_filename;

int comment_depth = 0;

/* HW exit-code rule: lexical errors -> status 1 (handled in main) */
int g_lex_errors = 0;

/* Keep your struct if you want (not used by parser anymore, but harmless) */
struct token {
   int category;
   char *text;
   int lineno;
   char *filename;
   int ival;
   double dval;
   char *sval;
};

struct token *yytoken;

/* Change: do NOT exit(1) here */
void lexical_error(const char *msg) {
   fprintf(stderr, "%s:%d: lexical error: %s\n",
           current_filename ? current_filename : "<stdin>", lineno, msg);
   g_lex_errors++;
}

/* same helpers */
char *copy_string(const char *s) {
   char *p = malloc(strlen(s)+1);
   if (!p) exit(1);
   strcpy(p, s);
   return p;
}

/* NEW helper: after make_token, set yylval.tok using yytoken->text (the full built lexeme) */
static void set_yylval_from_yytoken(int category) {
   /* Use yytoken->text so STRINGLITERAL gets full accumulated string */
   yylval.tok = token_create(category,
                             (yytoken && yytoken->text) ? yytoken->text : yytext,
                             lineno);
}

/* Your make_token stays the same, but we’ll call set_yylval_from_yytoken()
   from each rule right after make_token() */
void make_token(int category) {
   yytoken = malloc(sizeof(struct token));
   if (!yytoken) exit(1);

   yytoken->category = category;
   yytoken->text = copy_string(yytext);
   yytoken->lineno = lineno;
   yytoken->filename = current_filename;
   yytoken->ival = 0;
   yytoken->dval = 0.0;
   yytoken->sval = NULL;
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
    return 0; /* stop scanning */
}

/* ===== Keywords / literals ===== */

"fun"      { make_token(FUN);      set_yylval_from_yytoken(FUN);      return FUN; }
"val"      { make_token(VAL);      set_yylval_from_yytoken(VAL);      return VAL; }
"var"      { make_token(VAR);      set_yylval_from_yytoken(VAR);      return VAR; }
"if"       { make_token(IF);       set_yylval_from_yytoken(IF);       return IF; }
"else"     { make_token(ELSE);     set_yylval_from_yytoken(ELSE);     return ELSE; }
"while"    { make_token(WHILE);    set_yylval_from_yytoken(WHILE);    return WHILE; }
"for"      { make_token(FOR);      set_yylval_from_yytoken(FOR);      return FOR; }
"return"   { make_token(RETURN);   set_yylval_from_yytoken(RETURN);   return RETURN; }
"break"    { make_token(BREAK);    set_yylval_from_yytoken(BREAK);    return BREAK; }
"continue" { make_token(CONTINUE); set_yylval_from_yytoken(CONTINUE); return CONTINUE; }
"when"     { make_token(WHEN);     set_yylval_from_yytoken(WHEN);     return WHEN; }
"in"       { make_token(IN);       set_yylval_from_yytoken(IN);       return IN; }
"do"       { make_token(DO);       set_yylval_from_yytoken(DO);       return DO; }
"import"   { make_token(IMPORT);   set_yylval_from_yytoken(IMPORT);   return IMPORT; }
"const"    { make_token(CONST);    set_yylval_from_yytoken(CONST);    return CONST; }
"as"       { make_token(AS);       set_yylval_from_yytoken(AS);       return AS; }

"true"     { make_token(BOOLEANLITERAL); set_yylval_from_yytoken(BOOLEANLITERAL); return BOOLEANLITERAL; }
"false"    { make_token(BOOLEANLITERAL); set_yylval_from_yytoken(BOOLEANLITERAL); return BOOLEANLITERAL; }
"null"     { make_token(NULLLITERAL);    set_yylval_from_yytoken(NULLLITERAL);    return NULLLITERAL; }

/* ===== Identifiers ===== */

[a-zA-Z_][a-zA-Z0-9_]* {
    make_token(IDENT);
    set_yylval_from_yytoken(IDENT);
    return IDENT;
}

/* ===== Numeric literals ===== */

([0-9]+"."[0-9]*([eE][+-]?[0-9]+)?)[dD] {
    make_token(DOUBLELITERAL);
    yytoken->dval = strtod(yytext, NULL);
    set_yylval_from_yytoken(DOUBLELITERAL);
    return DOUBLELITERAL;
}

([0-9]+"."[0-9]*([eE][+-]?[0-9]+)?|[0-9]+[eE][+-]?[0-9]+) {
    make_token(REALLITERAL);
    yytoken->dval = strtod(yytext, NULL);
    set_yylval_from_yytoken(REALLITERAL);
    return REALLITERAL;
}

[0-9]+[lL] {
    make_token(LONGLITERAL);
    yytoken->ival = strtol(yytext, NULL, 10);
    set_yylval_from_yytoken(LONGLITERAL);
    return LONGLITERAL;
}

[0-9]+ {
    make_token(INTEGERLITERAL);
    yytoken->ival = strtol(yytext, NULL, 10);
    set_yylval_from_yytoken(INTEGERLITERAL);
    return INTEGERLITERAL;
}

/* ===== Character literal ===== */

\'([^\\\n]|\\.)\' {
    make_token(CHARACTERLITERAL);

    char c;
    if (yytext[1] == '\\') {
        switch (yytext[2]) {
            case 'n': c = '\n'; break;
            case 't': c = '\t'; break;
            case '\\': c = '\\'; break;
            case '\'': c = '\''; break;
            default:
                lexical_error("invalid character escape");
                /* recover by not returning a token */
                break;
        }
    } else {
        c = yytext[1];
    }

    yytoken->ival = c;
    set_yylval_from_yytoken(CHARACTERLITERAL);
    return CHARACTERLITERAL;
}

\'([^\\\n]|\\.)* {
    lexical_error("unterminated character literal");
    /* discard and continue */
}

/* ===== String literal (your original accumulator) ===== */

\" {
    BEGIN(STRING);

    make_token(STRINGLITERAL);

    /* overwrite text with dynamic buffer */
    free(yytoken->text);
    yytoken->text = malloc(2);
    strcpy(yytoken->text, "\"");

    yytoken->sval = malloc(1);
    yytoken->sval[0] = '\0';
}

<STRING>\" {
    /* append closing quote to raw text */
    yytoken->text = realloc(yytoken->text, strlen(yytoken->text)+2);
    strcat(yytoken->text, "\"");

    BEGIN(INITIAL);

    /* IMPORTANT: now yytoken->text holds the full string lexeme */
    set_yylval_from_yytoken(STRINGLITERAL);
    return STRINGLITERAL;
}

<STRING>\\n {
    yytoken->text = realloc(yytoken->text, strlen(yytoken->text)+3);
    strcat(yytoken->text, "\\n");

    yytoken->sval = realloc(yytoken->sval, strlen(yytoken->sval)+2);
    strcat(yytoken->sval, "\n");
}

<STRING>\\t {
    yytoken->text = realloc(yytoken->text, strlen(yytoken->text)+3);
    strcat(yytoken->text, "\\t");

    yytoken->sval = realloc(yytoken->sval, strlen(yytoken->sval)+2);
    strcat(yytoken->sval, "\t");
}

<STRING>\\\\ {
    yytoken->text = realloc(yytoken->text, strlen(yytoken->text)+3);
    strcat(yytoken->text, "\\\\");

    yytoken->sval = realloc(yytoken->sval, strlen(yytoken->sval)+2);
    strcat(yytoken->sval, "\\");
}

<STRING>\\\" {
    yytoken->text = realloc(yytoken->text, strlen(yytoken->text)+3);
    strcat(yytoken->text, "\\\"");

    yytoken->sval = realloc(yytoken->sval, strlen(yytoken->sval)+2);
    strcat(yytoken->sval, "\"");
}

<STRING>\n {
    lexical_error("unterminated string literal");
    BEGIN(INITIAL);
}

<STRING>. {
    char temp[2];
    temp[0] = yytext[0];
    temp[1] = '\0';

    yytoken->text = realloc(yytoken->text, strlen(yytoken->text)+2);
    strcat(yytoken->text, temp);

    yytoken->sval = realloc(yytoken->sval, strlen(yytoken->sval)+2);
    strcat(yytoken->sval, temp);
}

<STRING><<EOF>> {
    lexical_error("unterminated string literal");
    BEGIN(INITIAL);
    return 0;
}

/* ===== Multi-char operators ===== */

"==="    { make_token(REF_EQ);       set_yylval_from_yytoken(REF_EQ);       return REF_EQ; }
"!=="    { make_token(REF_NEQ);      set_yylval_from_yytoken(REF_NEQ);      return REF_NEQ; }
"=="     { make_token(EQ);           set_yylval_from_yytoken(EQ);           return EQ; }
"!="     { make_token(NEQ);          set_yylval_from_yytoken(NEQ);          return NEQ; }
">="     { make_token(GTE);          set_yylval_from_yytoken(GTE);          return GTE; }
"<="     { make_token(LTE);          set_yylval_from_yytoken(LTE);          return LTE; }
"..<"    { make_token(RANGE_UNTIL);  set_yylval_from_yytoken(RANGE_UNTIL);  return RANGE_UNTIL; }
".."     { make_token(RANGE);        set_yylval_from_yytoken(RANGE);        return RANGE; }
"&&"     { make_token(AND);          set_yylval_from_yytoken(AND);          return AND; }
"||"     { make_token(OR);           set_yylval_from_yytoken(OR);           return OR; }
"++"     { make_token(INCR);         set_yylval_from_yytoken(INCR);         return INCR; }
"--"     { make_token(DECR);         set_yylval_from_yytoken(DECR);         return DECR; }
"+="     { make_token(ADD_ASSIGN);   set_yylval_from_yytoken(ADD_ASSIGN);   return ADD_ASSIGN; }
"-="     { make_token(SUB_ASSIGN);   set_yylval_from_yytoken(SUB_ASSIGN);   return SUB_ASSIGN; }
"*="     { make_token(MUL_ASSIGN);   set_yylval_from_yytoken(MUL_ASSIGN);   return MUL_ASSIGN; }
/* NOTE: your original had DIV_ASSIGN but no rule for it; keep if you have it */
"/="     { make_token(DIV_ASSIGN);   set_yylval_from_yytoken(DIV_ASSIGN);   return DIV_ASSIGN; }
"%="     { make_token(MOD_ASSIGN);   set_yylval_from_yytoken(MOD_ASSIGN);   return MOD_ASSIGN; }
"->"     { make_token(ARROW);        set_yylval_from_yytoken(ARROW);        return ARROW; }
"::"     { make_token(DOUBLE_COLON); set_yylval_from_yytoken(DOUBLE_COLON); return DOUBLE_COLON; }
"?."     { make_token(SAFE_CALL);    set_yylval_from_yytoken(SAFE_CALL);    return SAFE_CALL; }
"?:"     { make_token(ELVIS);        set_yylval_from_yytoken(ELVIS);        return ELVIS; }

/* ===== Single-char tokens ===== */

"+"  { make_token(ADD);        set_yylval_from_yytoken(ADD);        return ADD; }
"-"  { make_token(SUB);        set_yylval_from_yytoken(SUB);        return SUB; }
"*"  { make_token(MUL);        set_yylval_from_yytoken(MUL);        return MUL; }
"/"  { make_token(DIV);        set_yylval_from_yytoken(DIV);        return DIV; }
"%"  { make_token(MOD);        set_yylval_from_yytoken(MOD);        return MOD; }
"="  { make_token(ASSIGNMENT); set_yylval_from_yytoken(ASSIGNMENT); return ASSIGNMENT; }
"!"  { make_token(NOT);        set_yylval_from_yytoken(NOT);        return NOT; }
"?"  { make_token(QUEST);      set_yylval_from_yytoken(QUEST);      return QUEST; }
";"  { make_token(SEMI);       set_yylval_from_yytoken(SEMI);       return SEMI; }
":"  { make_token(COLON);      set_yylval_from_yytoken(COLON);      return COLON; }
","  { make_token(COMMA);      set_yylval_from_yytoken(COMMA);      return COMMA; }
"."  { make_token(DOT);        set_yylval_from_yytoken(DOT);        return DOT; }
"("  { make_token(LPAREN);     set_yylval_from_yytoken(LPAREN);     return LPAREN; }
")"  { make_token(RPAREN);     set_yylval_from_yytoken(RPAREN);     return RPAREN; }
"{"  { make_token(LCURL);      set_yylval_from_yytoken(LCURL);      return LCURL; }
"}"  { make_token(RCURL);      set_yylval_from_yytoken(RCURL);      return RCURL; }
"["  { make_token(LSQUARE);    set_yylval_from_yytoken(LSQUARE);    return LSQUARE; }
"]"  { make_token(RSQUARE);    set_yylval_from_yytoken(RSQUARE);    return RSQUARE; }
"<"  { make_token(LANGLE);     set_yylval_from_yytoken(LANGLE);     return LANGLE; }
">"  { make_token(RANGLE);     set_yylval_from_yytoken(RANGLE);     return RANGLE; }

/* ===== Catch-all ===== */

. {
    lexical_error("illegal character");
    /* discard and continue */
}

<<EOF>> {
    return 0;   /* IMPORTANT: bison expects 0 at EOF */
}

%%