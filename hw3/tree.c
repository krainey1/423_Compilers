#include "tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

tree_t *tree_node(const char *symbol, int prodrule, int nkids, ...) {
  tree_t *t = (tree_t*)calloc(1, sizeof(tree_t));
  t->symbol = symbol;
  t->prodrule = prodrule;
  t->nkids = nkids;

  va_list ap;
  va_start(ap, nkids);
  for (int i = 0; i < nkids; i++) {
    t->kids[i] = va_arg(ap, tree_t*);
  }
  va_end(ap);

  return t;
}

tree_t *tree_leaf(token_t *tok) {
  tree_t *t = (tree_t*)calloc(1, sizeof(tree_t));
  t->nkids = 0;
  t->leaf = tok; // ownership: tree_free will free token
  t->symbol = "leaf";
  return t;
}

static void indent(int depth) {
  for (int i = 0; i < depth*2; i++) putchar(' ');
}

void tree_print(tree_t *t, int depth) {
  if (!t) return;

  indent(depth);

  if (t->nkids == 0) {
    if (t->leaf && t->leaf->lexeme)
      printf("TOKEN(%d) \"%s\" [line %d]\n", t->leaf->code, t->leaf->lexeme, t->leaf->line);
    else if (t->leaf)
      printf("TOKEN(%d) [line %d]\n", t->leaf->code, t->leaf->line);
    else
      printf("EMPTY_LEAF\n");
    return;
  }

  printf("%s: %d\n", t->symbol ? t->symbol : "node", t->nkids);
  for (int i = 0; i < t->nkids; i++) {
    tree_print(t->kids[i], depth+1);
  }
}

void tree_free(tree_t *t) {
  if (!t) return;
  for (int i = 0; i < t->nkids; i++) tree_free(t->kids[i]);
  if (t->nkids == 0) token_free(t->leaf);
  free(t);
}