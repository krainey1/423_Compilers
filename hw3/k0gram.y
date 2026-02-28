#include "token.h"
#include <stdlib.h>
#include <string.h>

token_t *token_create(int code, const char *yytext, int line) {
  token_t *t = (token_t*)calloc(1, sizeof(token_t));
  t->code = code;
  t->line = line;
  if (yytext) t->lexeme = strdup(yytext);
  return t;
}

void token_free(token_t *t) {
  if (!t) return;
  free(t->lexeme);
  free(t);
}