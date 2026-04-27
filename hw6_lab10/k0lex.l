%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "k0gram.tab.h"
#include "token.h"    
#include "tree.h" 


int lineno = 1;
char *current_filename;

int comment_depth = 0;


int g_lex_errors = 0;


struct old_token {
   int category;
   char *text;
   int lineno;
   char *filename;
   int ival;
   double dval;
   char *sval;
};

struct old_token *yytoken;


void lexical_error(const char *msg) {
   fprintf(stderr, "%s:%d: lexical error: %s\n",
           current_filename ? current_filename : "<stdin>", lineno, msg);
   g_lex_errors++;
}


char *copy_string(const char *s) {
   char *p = malloc(strlen(s)+1);
   if (!p) exit(1);
   strcpy(p, s);
   return p;
}

/* extra print thing inside lexer to show info is present before parsing */
static void printtok(token_t *t) {
   static int checked = 0, debug = 0;
   if (!checked) { checked = 1; debug = (getenv("K0_LEX_DEBUG") != NULL); }
   if (!debug || !t) return;
   fprintf(stderr, "LEX TOKEN(%d) \"%s\" [line %d]\n",
           t->code, t->lexeme ? t->lexeme : "", t->line);
}

static void set_yylval_from_yytoken(int category) {
   yylval.tok = token_create(category,
                             (yytoken && yytoken->text) ? yytoken->text : yytext,
                             lineno);
   printtok(yylval.tok);
}

static void free_yytoken(void) {
   if (!yytoken) return;
   free(yytoken->text);
   free(yytoken->sval);
   free(yytoken);
   yytoken = NULL;
}


void make_token(int category) {
   yytoken = malloc(sizeof(struct old_token));
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
%option noinput nounput
%x COMMENT STRING

%%

\n              { lineno++; }
[ \t\r\f]+      ;

"#".*           ;
"//".*          ;



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
    return 0;
}



"fun"      { make_token(FUN);      set_yylval_from_yytoken(FUN);      free_yytoken(); return FUN; }
"val"      { make_token(VAL);      set_yylval_from_yytoken(VAL);      free_yytoken(); return VAL; }
"var"      { make_token(VAR);      set_yylval_from_yytoken(VAR);      free_yytoken(); return VAR; }
"if"       { make_token(IF);       set_yylval_from_yytoken(IF);       free_yytoken(); return IF; }
"else"     { make_token(ELSE);     set_yylval_from_yytoken(ELSE);     free_yytoken(); return ELSE; }
"while"    { make_token(WHILE);    set_yylval_from_yytoken(WHILE);    free_yytoken(); return WHILE; }
"for"      { make_token(FOR);      set_yylval_from_yytoken(FOR);      free_yytoken(); return FOR; }
"return"   { make_token(RETURN);   set_yylval_from_yytoken(RETURN);   free_yytoken(); return RETURN; }
"break"    { make_token(BREAK);    set_yylval_from_yytoken(BREAK);    free_yytoken(); return BREAK; }
"continue" { make_token(CONTINUE); set_yylval_from_yytoken(CONTINUE); free_yytoken(); return CONTINUE; }
"when"     { make_token(WHEN);     set_yylval_from_yytoken(WHEN);     free_yytoken(); return WHEN; }
"in"       { make_token(IN);       set_yylval_from_yytoken(IN);       free_yytoken(); return IN; }
"do"       { make_token(DO);       set_yylval_from_yytoken(DO);       free_yytoken(); return DO; }
"import"   { make_token(IMPORT);   set_yylval_from_yytoken(IMPORT);   free_yytoken(); return IMPORT; }
"const"    { make_token(CONST);    set_yylval_from_yytoken(CONST);    free_yytoken(); return CONST; }
"as"       { make_token(AS);       set_yylval_from_yytoken(AS);       free_yytoken(); return AS; }

"true"     { make_token(BOOLEANLITERAL); set_yylval_from_yytoken(BOOLEANLITERAL); free_yytoken(); return BOOLEANLITERAL; }
"false"    { make_token(BOOLEANLITERAL); set_yylval_from_yytoken(BOOLEANLITERAL); free_yytoken(); return BOOLEANLITERAL; }
"null"     { make_token(NULLLITERAL);    set_yylval_from_yytoken(NULLLITERAL);    free_yytoken(); return NULLLITERAL; }



[a-zA-Z_][a-zA-Z0-9_]* {
    make_token(IDENT);
    set_yylval_from_yytoken(IDENT);
    free_yytoken();
    return IDENT;
}



([0-9]+"."[0-9]*([eE][+-]?[0-9]+)?)[dD] {
    make_token(DOUBLELITERAL);
    yytoken->dval = strtod(yytext, NULL);
    set_yylval_from_yytoken(DOUBLELITERAL);
    free_yytoken();
    return DOUBLELITERAL;
}

([0-9]+"."[0-9]*([eE][+-]?[0-9]+)?|[0-9]+[eE][+-]?[0-9]+) {
    make_token(REALLITERAL);
    yytoken->dval = strtod(yytext, NULL);
    set_yylval_from_yytoken(REALLITERAL);
    free_yytoken();
    return REALLITERAL;
}

[0-9]+[lL] {
    make_token(LONGLITERAL);
    yytoken->ival = strtol(yytext, NULL, 10);
    set_yylval_from_yytoken(LONGLITERAL);
    free_yytoken();
    return LONGLITERAL;
}

[0-9]+ {
    make_token(INTEGERLITERAL);
    yytoken->ival = strtol(yytext, NULL, 10);
    set_yylval_from_yytoken(INTEGERLITERAL);
    free_yytoken();
    return INTEGERLITERAL;
}



\'([^\\\n]|\\.)\' {
    make_token(CHARACTERLITERAL);
    set_yylval_from_yytoken(CHARACTERLITERAL);
    free_yytoken();
    return CHARACTERLITERAL;
}

\'([^\\\n]|\\.)* {
    lexical_error("unterminated character literal");
}



\" {
    BEGIN(STRING);

    make_token(STRINGLITERAL);

    free(yytoken->text);
    yytoken->text = malloc(2);
    strcpy(yytoken->text, "\"");

    yytoken->sval = malloc(1);
    yytoken->sval[0] = '\0';
}

<STRING>\" {
    yytoken->text = realloc(yytoken->text, strlen(yytoken->text)+2);
    strcat(yytoken->text, "\"");

    BEGIN(INITIAL);

    set_yylval_from_yytoken(STRINGLITERAL);
    free_yytoken();
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



"==="    { make_token(REF_EQ);       set_yylval_from_yytoken(REF_EQ);       free_yytoken(); return REF_EQ; }
"!=="    { make_token(REF_NEQ);      set_yylval_from_yytoken(REF_NEQ);      free_yytoken(); return REF_NEQ; }
"=="     { make_token(EQ);           set_yylval_from_yytoken(EQ);           free_yytoken(); return EQ; }
"!="     { make_token(NEQ);          set_yylval_from_yytoken(NEQ);          free_yytoken(); return NEQ; }
">="     { make_token(GTE);          set_yylval_from_yytoken(GTE);          free_yytoken(); return GTE; }
"<="     { make_token(LTE);          set_yylval_from_yytoken(LTE);          free_yytoken(); return LTE; }
"..<"    { make_token(RANGE_UNTIL);  set_yylval_from_yytoken(RANGE_UNTIL);  free_yytoken(); return RANGE_UNTIL; }
".."     { make_token(RANGE);        set_yylval_from_yytoken(RANGE);        free_yytoken(); return RANGE; }
"&&"     { make_token(AND);          set_yylval_from_yytoken(AND);          free_yytoken(); return AND; }
"||"     { make_token(OR);           set_yylval_from_yytoken(OR);           free_yytoken(); return OR; }
"++"     { make_token(INCR);         set_yylval_from_yytoken(INCR);         free_yytoken(); return INCR; }
"--"     { make_token(DECR);         set_yylval_from_yytoken(DECR);         free_yytoken(); return DECR; }
"+="     { make_token(ADD_ASSIGN);   set_yylval_from_yytoken(ADD_ASSIGN);   free_yytoken(); return ADD_ASSIGN; }
"-="     { make_token(SUB_ASSIGN);   set_yylval_from_yytoken(SUB_ASSIGN);   free_yytoken(); return SUB_ASSIGN; }
"*="     { make_token(MUL_ASSIGN);   set_yylval_from_yytoken(MUL_ASSIGN);   free_yytoken(); return MUL_ASSIGN; }
"/="     { make_token(DIV_ASSIGN);   set_yylval_from_yytoken(DIV_ASSIGN);   free_yytoken(); return DIV_ASSIGN; }
"%="     { make_token(MOD_ASSIGN);   set_yylval_from_yytoken(MOD_ASSIGN);   free_yytoken(); return MOD_ASSIGN; }
"->"     { make_token(ARROW);        set_yylval_from_yytoken(ARROW);        free_yytoken(); return ARROW; }
"::"     { make_token(DOUBLE_COLON); set_yylval_from_yytoken(DOUBLE_COLON); free_yytoken(); return DOUBLE_COLON; }
"?."     { make_token(SAFE_CALL);    set_yylval_from_yytoken(SAFE_CALL);    free_yytoken(); return SAFE_CALL; }
"?:"     { make_token(ELVIS);        set_yylval_from_yytoken(ELVIS);        free_yytoken(); return ELVIS; }



"+"  { make_token(ADD);        set_yylval_from_yytoken(ADD);        free_yytoken(); return ADD; }
"-"  { make_token(SUB);        set_yylval_from_yytoken(SUB);        free_yytoken(); return SUB; }
"*"  { make_token(MUL);        set_yylval_from_yytoken(MUL);        free_yytoken(); return MUL; }
"/"  { make_token(DIV);        set_yylval_from_yytoken(DIV);        free_yytoken(); return DIV; }
"%"  { make_token(MOD);        set_yylval_from_yytoken(MOD);        free_yytoken(); return MOD; }
"="  { make_token(ASSIGNMENT); set_yylval_from_yytoken(ASSIGNMENT); free_yytoken(); return ASSIGNMENT; }
"!"  { make_token(NOT);        set_yylval_from_yytoken(NOT);        free_yytoken(); return NOT; }
"?"  { make_token(QUEST);      set_yylval_from_yytoken(QUEST);      free_yytoken(); return QUEST; }
";"  { make_token(SEMI);       set_yylval_from_yytoken(SEMI);       free_yytoken(); return SEMI; }
":"  { make_token(COLON);      set_yylval_from_yytoken(COLON);      free_yytoken(); return COLON; }
","  { make_token(COMMA);      set_yylval_from_yytoken(COMMA);      free_yytoken(); return COMMA; }
"."  { make_token(DOT);        set_yylval_from_yytoken(DOT);        free_yytoken(); return DOT; }
"("  { make_token(LPAREN);     set_yylval_from_yytoken(LPAREN);     free_yytoken(); return LPAREN; }
")"  { make_token(RPAREN);     set_yylval_from_yytoken(RPAREN);     free_yytoken(); return RPAREN; }
"{"  { make_token(LCURL);      set_yylval_from_yytoken(LCURL);      free_yytoken(); return LCURL; }
"}"  { make_token(RCURL);      set_yylval_from_yytoken(RCURL);      free_yytoken(); return RCURL; }
"["  { make_token(LSQUARE);    set_yylval_from_yytoken(LSQUARE);    free_yytoken(); return LSQUARE; }
"]"  { make_token(RSQUARE);    set_yylval_from_yytoken(RSQUARE);    free_yytoken(); return RSQUARE; }
"<"  { make_token(LANGLE);     set_yylval_from_yytoken(LANGLE);     free_yytoken(); return LANGLE; }
">"  { make_token(RANGLE);     set_yylval_from_yytoken(RANGLE);     free_yytoken(); return RANGLE; }



. {
    lexical_error("illegal character");
}

<<EOF>> {
    return 0;
}

%%

