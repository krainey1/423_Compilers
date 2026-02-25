#ifndef YTAB_H
#define YTAB_H

/*
keywords based on kotlin spec, this seemed like a fine amount for a toy compiler
*/
/* ========== KEYWORDS ========== */
#define FUN 258              /*fun*/
#define VAL 259              /*val*/
#define VAR 260              /*var*/
#define IF 261               /*if*/
#define ELSE 262             /*else*/
#define WHILE 263           /*while*/
#define FOR 264              /*for*/
#define RETURN 265           /*return*/
#define BREAK 266            /*break*/
#define CONTINUE 267         /*continue*/
#define WHEN 268             /*when*/
#define IN 269               /*in*/
#define DO 270               /*do*/
#define IMPORT 271           /*import*/
#define CONST 272            /*const*/

/* ========== LITERALS ========== */
#define INTEGERLITERAL 280  /*[0-9]+*/
#define LONGLITERAL 281
#define REALLITERAL 282     /*[0-9]+"."[0-9]+*/
#define DOUBLELITERAL 283
#define BOOLEANLITERAL 284
#define CHARACTERLITERAL 285
#define STRINGLITERAL 286
#define NULLLITERAL 287

/* ========== IDENTIFIERS ========== */
#define IDENT 290

/* ========== OPERATORS ========== */
/* Assignment operators */
#define ASSIGNMENT 300
#define ADD_ASSIGN 301
#define SUB_ASSIGN 302
#define MUL_ASSIGN 303
#define DIV_ASSIGN 304
#define MOD_ASSIGN 305

/* Arithmetic operators */
#define ADD 310
#define SUB 311
#define MUL 312
#define DIV 313
#define MOD 314

/* Increment/Decrement */
#define INCR 320
#define DECR 321

/* Comparison operators */
#define EQ 330
#define NEQ 331
#define GTE 332
#define LTE 333

/* Referential equality */
#define REF_EQ 340
#define REF_NEQ 341

/* Logical operators */
#define AND 350
#define OR 351
#define NOT 352


/* Null-safe operators */
#define SAFE_CALL 370
#define ELVIS 371
#define QUEST 372

/* Range operators */
#define RANGE 380
#define RANGE_UNTIL 381

/* Type cast */
#define AS 390                          /*as*/

/* ========== DELIMITERS & PUNCTUATION ========== */
#define LPAREN 400
#define RPAREN 401
#define LCURL 402                       /*{*/
#define RCURL 403                       /*}*/
#define LSQUARE 404                     /*[*/
#define RSQUARE 405                     /*]*/
#define LANGLE 406                      /*<*/
#define RANGLE 407                      /*>*/
#define COMMA 408                       /*,*/
#define DOT 409                         /*.*/
#define COLON 410                       /*:*/
#define SEMI 411                        /*;*/
#define ARROW 412                       /*->*/
#define DOUBLE_COLON 413                /*::*/

/* ========== SPECIAL ========== */
#define NEWLINE 420                     /*\n*/

#endif /* YTAB_H */