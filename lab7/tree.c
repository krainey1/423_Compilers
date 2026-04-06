#include "tree.h"
#include "type.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

int serial = 0;

/* declared in k0gram.y after %% */
const char *yyname(int sym);

tree_t *tree_node(const char *symbol, int prodrule, int nkids, ...) {
  tree_t *t = (tree_t*)calloc(1, sizeof(tree_t));
  t->symbol = symbol;
  t->prodrule = prodrule;
  t->nkids = nkids;
  t->id = serial++;
  t->type = NULL;

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
  t->id = serial++;
  t->type = NULL;
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
      printf("TOKEN(%d) \"%s\" [line %d] [type=%s]\n",
             t->leaf->code, t->leaf->lexeme, t->leaf->line, typename(t->type));
    else if (t->leaf)
      printf("TOKEN(%d) [line %d] [type=%s]\n",
             t->leaf->code, t->leaf->line, typename(t->type));
    else
      printf("EMPTY_LEAF\n");
    return;
  }

  printf("%s: %d [type=%s]\n",
         t->symbol ? t->symbol : "node",
         t->nkids,
         typename(t->type));
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

/* Escape leading/trailing double quotes so dot doesn't break */
static char *dot_escape(const char *s) {
  if (!s) return strdup("(null)");
  size_t len = strlen(s);
  char *s2 = malloc(len + 8);
  if (len >= 2 && s[0] == '"' && s[len-1] == '"') {
    sprintf(s2, "\\%s", s);
    s2[strlen(s2)-1] = '\0';
    strcat(s2, "\\\"");
  } else {
    strcpy(s2, s);
  }
  return s2;
}

static char *pretty_print_name(tree_t *t) {
  char *s2 = malloc(64);
  if (t->leaf == NULL) {
    snprintf(s2, 64, "%s#%d", t->symbol ? t->symbol : "node", t->prodrule % 10);
  } else {
    char *esc = dot_escape(t->leaf->lexeme ? t->leaf->lexeme : "");
    snprintf(s2, 64, "%s:%d", esc, t->leaf->code);
    free(esc);
  }
  return s2;
}

static void print_branch(tree_t *t, FILE *f) {
  char *name = pretty_print_name(t);
  fprintf(f, "N%d [shape=box label=\"%s\"];\n", t->id, name);
  free(name);
}

static void print_leaf(tree_t *t, FILE *f) {
  const char *catname = yyname(t->leaf->code);
  char *esc = dot_escape(t->leaf->lexeme ? t->leaf->lexeme : "");
  fprintf(f, "N%d [shape=box style=dotted label=\" %s \\n ", t->id, catname);
  fprintf(f, "text = %s \\l lineno = %d \\l\"];\n", esc, t->leaf->line);
  free(esc);
}

static void print_graph2(tree_t *t, FILE *f) {
  int i;
  if (t->leaf != NULL) {
    print_leaf(t, f);
    return;
  }
  print_branch(t, f);
  for (i = 0; i < t->nkids; i++) {
    if (t->kids[i] != NULL) {
      fprintf(f, "N%d -> N%d;\n", t->id, t->kids[i]->id);
      print_graph2(t->kids[i], f);
    } else {
      /* NULL kid: epsilon production */
      fprintf(f, "N%d -> N%d%d;\n", t->id, t->id, serial);
      fprintf(f, "N%d%d [label=\"%s\"];\n", t->id, serial, "Empty rule");
      serial++;
    }
  }
}

void print_graph(tree_t *t, const char *filename) {
  FILE *f = fopen(filename, "w");
  if (!f) { perror(filename); return; }
  fprintf(f, "digraph {\n");
  print_graph2(t, f);
  fprintf(f, "}\n");
  fclose(f);
}
