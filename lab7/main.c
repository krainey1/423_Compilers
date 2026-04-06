#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tree.h"
#include "symtab.h"
#include "syms.h"

extern FILE *yyin;
extern int yyparse(void);

extern void print_graph(tree_t *t, const char *filename);

const char *yyfilename = NULL;
extern char *current_filename;

extern int g_lex_errors;      /* from lexer */
extern int g_syntax_errors;   /* from parser */
extern int g_semantic_errors; /* from syms.c */
extern tree_t *g_root;        /* from parser */

int main(int argc, char **argv) {
    int dot_mode = 0;
    int tree_mode = 0;
    int symtab_mode = 0;
    const char *source_file = NULL;

    /* parse arguments */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-dot") == 0) {
            dot_mode = 1;
        } else if (strcmp(argv[i], "-tree") == 0) {
            tree_mode = 1;
        } else if (strcmp(argv[i], "-symtab") == 0) {
            symtab_mode = 1;
        } else {
            source_file = argv[i];
        }
    }

    if (!source_file) {
        fprintf(stderr, "usage: %s [-dot] [-tree] [-symtab] <file.kt>\n", argv[0]);
        return 1;
    }

    yyfilename = source_file;
    current_filename = (char *)source_file;

    yyin = fopen(source_file, "r");
    if (!yyin) {
        perror(source_file);
        return 1;
    }

    /* assignment requirement: print filename to stdout when opened */
    printf("%s\n", source_file);

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

    /* semantic analysis */
    SymbolTable global = buildsymtabs(g_root, source_file);
    check_undeclared(g_root, global);
    annotate_expr_types(g_root, global);

    if (g_semantic_errors > 0) {
        freesymtabs();
        if (g_root) tree_free(g_root);
        return 3;
    }

    /* optional outputs */
    if (dot_mode) {
        size_t len = strlen(source_file);
        char *dotfile = (char *)malloc(len + 5);
        if (!dotfile) {
            perror("malloc");
            freesymtabs();
            tree_free(g_root);
            return 1;
        }

        sprintf(dotfile, "%s.dot", source_file);
        print_graph(g_root, dotfile);
        printf("dot file written to: %s\n", dotfile);
        free(dotfile);
    }

    if (tree_mode) {
        tree_print(g_root, 0);
    }

    if (symtab_mode) {
        printsymtabs();
    }

    /* default behavior */
    if (!dot_mode && !tree_mode && !symtab_mode) {
        printf("No errors\n");
    }

    /* debugging only:
    fprintf(stderr, "\nprintsyms:\n");
    printsyms(g_root);
    */

    freesymtabs();
    tree_free(g_root);
    return 0;
}
