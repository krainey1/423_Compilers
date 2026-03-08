#include <stdio.h>
#include <stdlib.h>
#include "tree.h"

extern FILE *yyin;
extern int yyparse(void);
const char *yyfilename = NULL;

extern int g_lex_errors;     // from lexer
extern int g_syntax_errors;  // from parser.y
extern tree_t *g_root;       // from parser.y


int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "usage: %s <file.k0>\n", argv[0]);
    return 1;
  }

  yyfilename = argv[1];
  yyin = fopen(argv[1], "r");
  if (!yyin) {
    perror(argv[1]);
    return 1;
  }

  int parse_rc = yyparse();
  fclose(yyin);

  if (g_lex_errors > 0) {
    if (g_root) tree_free(g_root);
    return 1; // lexical errors
  }

  if (parse_rc != 0 || g_syntax_errors > 0) {
    if (g_root) tree_free(g_root);
    return 2; // syntax errors
  }

  // Success: print syntax tree
  tree_print(g_root, 0);
  tree_free(g_root);
  return 0;
}