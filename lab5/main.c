#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"

extern FILE *yyin;
extern int yyparse(void);
const char *yyfilename = NULL;

extern int g_lex_errors;     // from lexer
extern int g_syntax_errors;  // from parser.y
extern tree_t *g_root;       // from parser.y


int main(int argc, char **argv) {
  int dot_mode = 0;
  const char *source_file = NULL;

  /* parse arguments: optional -dot flag then filename */
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-dot") == 0) {
      dot_mode = 1;
    } else {
      source_file = argv[i];
    }
  }

  if (!source_file) {
    fprintf(stderr, "usage: %s [-dot] <file.kt>\n", argv[0]);
    return 1;
  }

  yyfilename = source_file;
  yyin = fopen(source_file, "r");
  if (!yyin) {
    perror(source_file);
    return 1;
  }

  int parse_rc = yyparse();
  fclose(yyin);

  if (g_lex_errors > 0) {
    if (g_root) tree_free(g_root);
    return 1;
  }

  if (parse_rc != 0 || g_syntax_errors > 0) {
    if (g_root) tree_free(g_root);
    return 2;
  }

  if (dot_mode) {
    /* build output filename: <source_file>.dot */
    size_t len = strlen(source_file);
    char *dotfile = malloc(len + 5);
    sprintf(dotfile, "%s.dot", source_file);
    print_graph(g_root, dotfile);
    fprintf(stderr, "dot file written to: %s\n", dotfile);
    fprintf(stderr, "run: dot -Tpng %s >%.*s.png\n",
            dotfile, (int)len, source_file);
    free(dotfile);
  } else {
    tree_print(g_root, 0);
  }

  tree_free(g_root);
  return 0;
}