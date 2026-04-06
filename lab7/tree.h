#ifndef TREE_H
#define TREE_H

#include "token.h"

struct typeinfo;
typedef struct typeinfo *typeptr;

typedef struct tree {
  int prodrule;          // an integer you assign (or just 0 if you don't care)
  const char *symbol;    // optional readable name (nonterminal name)
  int nkids;
  struct tree *kids[9];  // assignment says up to 9 is fine
  token_t *leaf;         // if nkids==0, may be non-null token
  int id;                // unique node id for dot output
  typeptr type;          // NEW: semantic type attached to this node
} tree_t;

tree_t *tree_node(const char *symbol, int prodrule, int nkids, ...);
tree_t *tree_leaf(token_t *tok);
void tree_print(tree_t *t, int depth);
void tree_free(tree_t *t);
void print_graph(tree_t *t, const char *filename);

#endif
