#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "y.tab.h"

// token struct
struct token {
   int category;
   char *text;
   int lineno;
   char *filename;
   int ival;
   double dval;
   char *sval;
};

struct tokenlist {
   struct token *t;
   struct tokenlist *next;
};

//grab from lexer
extern int yylex(void);
extern struct token *yytoken;
extern char *current_filename;
extern FILE *yyin;


//append to linked list
void append_token(struct tokenlist **head, struct tokenlist **tail) {
    struct tokenlist *node = malloc(sizeof(struct tokenlist));
    if (!node) exit(1);

    node->t = yytoken;
    node->next = NULL;

    if (*head == NULL) {
        *head = node;
        *tail = node;
    } else {
        (*tail)->next = node;
        *tail = node;
    }
}


//printing
void print_tokens(struct tokenlist *head) {

    printf("Category\tText\t\tLineno\t\tFilename\tIval/Sval\n");
    printf("-----------------------------------------------------------------\n");

    while (head) {

        struct token *t = head->t;

        printf("%d\t\t%s\t\t%d\t\t%s\t\t",
               t->category,
               t->text,
               t->lineno,
               t->filename);

        /* Print literal value if applicable */
        if (t->category == INTEGERLITERAL ||
            t->category == CHARACTERLITERAL) {
            printf("%d", t->ival);
        }
        else if (t->category == REALLITERAL) {
            printf("%f", t->dval);
        }
        else if (t->category == STRINGLITERAL) {
            printf("%s", t->sval);
        }

        printf("\n");

        head = head->next;
    }
}


//to be good and not have memory leaks :3
void free_tokens(struct tokenlist *head) {

    while (head) {
        struct tokenlist *temp = head;
        struct token *t = head->t;

        if (t->text) free(t->text);
        if (t->sval) free(t->sval);
        free(t);

        head = head->next;
        free(temp);
    }
}


//mainm gotta call yylex and make a linked list
int main(int argc, char *argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <inputfile>\n", argv[0]);
        exit(1);
    }

    yyin = fopen(argv[1], "r");
    if (!yyin) {
        perror("Cannot open file");
        exit(1);
    }

    current_filename = argv[1];

    struct tokenlist *head = NULL;
    struct tokenlist *tail = NULL;

    int tok;

    while ((tok = yylex()) != -1) {
        append_token(&head, &tail);
    }

    print_tokens(head);
    free_tokens(head);

    fclose(yyin);

    return 0;
}