#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tree.h"
#include "symtab.h"
#include "syms.h"
#include "i_code.h"
#include "codegen.h"
#include "assemble.h"


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
    int dot_mode    = 0;
    int tree_mode   = 0;
    int symtab_mode = 0;
    int ic_mode     = 0;   /* -ic  : also write/show .ic file */
    int asm_only    = 0;   /* -S   : write .s but don't compile/run */
    const char *source_file = NULL;

    /* parse arguments */
    for (int i = 1; i < argc; i++) {
        if      (strcmp(argv[i], "-dot")    == 0) dot_mode    = 1;
        else if (strcmp(argv[i], "-tree")   == 0) tree_mode   = 1;
        else if (strcmp(argv[i], "-symtab") == 0) symtab_mode = 1;
        else if (strcmp(argv[i], "-ic")     == 0) ic_mode     = 1;
        else if (strcmp(argv[i], "-S")      == 0) asm_only    = 1;
        else                                       source_file = argv[i];
    }

    if (!source_file) {
        fprintf(stderr, "usage: %s [-dot] [-tree] [-symtab] [-ic] [-S] <file.kt>\n",
                argv[0]);
        return 1;
    }

    yyfilename       = source_file;
    current_filename = (char *)source_file;

    yyin = fopen(source_file, "r");
    if (!yyin) { perror(source_file); return 1; }

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

    if (g_semantic_errors > 0) {
        freesymtabs();
        if (g_root) tree_free(g_root);
        return 3;
    }

    /* optional debug outputs */
    if (dot_mode) {
        size_t len  = strlen(source_file);
        char *dotfile = malloc(len + 5);
        if (!dotfile) { perror("malloc"); freesymtabs(); tree_free(g_root); return 1; }
        sprintf(dotfile, "%s.dot", source_file);
        print_graph(g_root, dotfile);
        printf("dot file written to: %s\n", dotfile);
        free(dotfile);
    }

    if (tree_mode)   tree_print(g_root, 0);
    if (symtab_mode) printsymtabs();

    /* ------------------------------------------------------------------ */
    /* Intermediate code generation                                         */
    /* ------------------------------------------------------------------ */
    icode_list_t *str_seg = NULL;
    icode_list_t *dat_seg = NULL;
    icode_list_t *code    = codegen(g_root, global, &str_seg, &dat_seg);

    /* Optionally write the .ic file */
    if (ic_mode) {
        char *ic_file = make_ic_filename(source_file);
        icode_emit(code, ic_file, str_seg, dat_seg);
        printf("IC written to: %s\n", ic_file);
        free(ic_file);
    }

    // assembly gen
    char *asm_file = make_asm_filename(source_file);
    asm_gen(code, str_seg, dat_seg, asm_file);

    icode_free(code);
    icode_free(str_seg);
    icode_free(dat_seg);

    freesymtabs();
    tree_free(g_root);

    //run unless we pass .S
    if (!asm_only) {
        /* Derive executable name: strip .s extension */
        char *exe_file = strdup(asm_file);
        char *dot      = strrchr(exe_file, '.');
        if (dot) *dot = '\0';

        // gcc -o <exe> <asm_file> -no-pie 
        char cmd[1024];
        snprintf(cmd, sizeof cmd,
                 "gcc -o %s %s -no-pie 2>&1",
                 exe_file, asm_file);

        int rc = system(cmd);
        if (rc != 0) {
            fprintf(stderr, "k0: gcc failed (exit %d)\n", rc);
            free(asm_file);
            free(exe_file);
            return 4;
        }

       //run the compiled program
        snprintf(cmd, sizeof cmd, "./%s", exe_file);
        rc = system(cmd);

        free(exe_file);
    } else {
        printf("Assembly written to: %s\n", asm_file);
    }

    free(asm_file);
    return 0;
}
