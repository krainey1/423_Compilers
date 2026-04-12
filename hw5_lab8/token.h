#ifndef TOKEN_H
#define TOKEN_H

typedef struct token {
  int code;        // token integer code (the %token value)
  char *lexeme;    // strdup() of yytext for leaves that have text
  int line;        // yylineno at time of lex
} token_t;

token_t *token_create(int code, const char *yytext, int line);
void token_free(token_t *t);

#endif