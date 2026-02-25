%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "y.tab.h"

int lineno = 1;
char *current_filename;

int comment_depth = 0;

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



void lexical_error(const char *msg) {
   fprintf(stderr, "%s:%d: lexical error: %s\n",
           current_filename, lineno, msg);
   exit(1);
}

char *copy_string(const char *s) {
   char *p = malloc(strlen(s)+1);
   if (!p) exit(1);
   strcpy(p, s);
   return p;
}

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
}



"fun"      { make_token(FUN); return FUN; }
"val"      { make_token(VAL); return VAL; }
"var"      { make_token(VAR); return VAR; }
"if"       { make_token(IF); return IF; }
"else"     { make_token(ELSE); return ELSE; }
"while"    { make_token(WHILE); return WHILE; }
"for"      { make_token(FOR); return FOR; }
"return"   { make_token(RETURN); return RETURN; }
"break"    { make_token(BREAK); return BREAK; }
"continue" { make_token(CONTINUE); return CONTINUE; }
"when"     { make_token(WHEN); return WHEN; }
"in"       { make_token(IN); return IN; }
"do"       { make_token(DO); return DO; }
"import"   { make_token(IMPORT); return IMPORT; }
"const"    { make_token(CONST); return CONST; }
"as"       { make_token(AS); return AS; }
"true"     { make_token(BOOLEANLITERAL); return BOOLEANLITERAL; }
"false"    { make_token(BOOLEANLITERAL); return BOOLEANLITERAL; }
"null"     { make_token(NULLLITERAL); return NULLLITERAL; }



[a-zA-Z_][a-zA-Z0-9_]* {
    make_token(IDENT);
    return IDENT;
}

([0-9]+"."[0-9]*([eE][+-]?[0-9]+)?)[dD] {
    make_token(DOUBLELITERAL);
    yytoken->dval = strtod(yytext, NULL);
    return DOUBLELITERAL;
}



([0-9]+"."[0-9]*([eE][+-]?[0-9]+)?|[0-9]+[eE][+-]?[0-9]+) {
    make_token(REALLITERAL);
    yytoken->dval = strtod(yytext, NULL);
    return REALLITERAL;
}

[0-9]+[lL] {
    make_token(LONGLITERAL);
    yytoken->ival = strtol(yytext, NULL, 10);
    return LONGLITERAL;
}

[0-9]+ {
    make_token(INTEGERLITERAL);
    yytoken->ival = strtol(yytext, NULL, 10);
    return INTEGERLITERAL;
}



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
        }
    } else {
        c = yytext[1];
    }

    yytoken->ival = c;
    return CHARACTERLITERAL;
}

\'([^\\\n]|\\.)* {
    lexical_error("unterminated character literal");
}




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
}




"==="    { make_token(REF_EQ); return REF_EQ; }
"!=="    { make_token(REF_NEQ); return REF_NEQ; }
"=="     { make_token(EQ); return EQ; }
"!="     { make_token(NEQ); return NEQ; }
">="     { make_token(GTE); return GTE; }
"<="     { make_token(LTE); return LTE; }
"..<"    { make_token(RANGE_UNTIL); return RANGE_UNTIL; }
".."     { make_token(RANGE); return RANGE; }
"&&"     { make_token(AND); return AND; }
"||"     { make_token(OR); return OR; }
"++"     { make_token(INCR); return INCR; }
"--"     { make_token(DECR); return DECR; }
"+="     { make_token(ADD_ASSIGN); return ADD_ASSIGN; }
"-="     { make_token(SUB_ASSIGN); return SUB_ASSIGN; }
"*="     { make_token(MUL_ASSIGN); return MUL_ASSIGN; }
"/="     { make_token(DIV_ASSIGN); return DIV_ASSIGN; }
"%="     { make_token(MOD_ASSIGN); return MOD_ASSIGN; }
"->"     { make_token(ARROW); return ARROW; }
"::"     { make_token(DOUBLE_COLON); return DOUBLE_COLON; }
"?."     { make_token(SAFE_CALL); return SAFE_CALL; }
"?:"     { make_token(ELVIS); return ELVIS; }



"+"  { make_token(ADD); return ADD; }
"-"  { make_token(SUB); return SUB; }
"*"  { make_token(MUL); return MUL; }
"/"  { make_token(DIV); return DIV; }
"%"  { make_token(MOD); return MOD; }
"="  { make_token(ASSIGNMENT); return ASSIGNMENT; }
"!"  { make_token(NOT); return NOT; }
"?"  { make_token(QUEST); return QUEST; }
";"  { make_token(SEMI); return SEMI; }
":"  { make_token(COLON); return COLON; }
","  { make_token(COMMA); return COMMA; }
"."  { make_token(DOT); return DOT; }
"("  { make_token(LPAREN); return LPAREN; }
")"  { make_token(RPAREN); return RPAREN; }
"{"  { make_token(LCURL); return LCURL; }
"}"  { make_token(RCURL); return RCURL; }
"["  { make_token(LSQUARE); return LSQUARE; }
"]"  { make_token(RSQUARE); return RSQUARE; }
"<"  { make_token(LANGLE); return LANGLE; }
">"  { make_token(RANGLE); return RANGLE; }



. {
    lexical_error("illegal character");
}



<<EOF>> {
    return -1;
}

%%