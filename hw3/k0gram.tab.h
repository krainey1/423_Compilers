/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_K0GRAM_TAB_H_INCLUDED
# define YY_YY_K0GRAM_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    FUN = 258,                     /* FUN  */
    VAL = 259,                     /* VAL  */
    VAR = 260,                     /* VAR  */
    IF = 261,                      /* IF  */
    ELSE = 262,                    /* ELSE  */
    WHILE = 263,                   /* WHILE  */
    FOR = 264,                     /* FOR  */
    RETURN = 265,                  /* RETURN  */
    BREAK = 266,                   /* BREAK  */
    CONTINUE = 267,                /* CONTINUE  */
    WHEN = 268,                    /* WHEN  */
    IN = 269,                      /* IN  */
    DO = 270,                      /* DO  */
    IMPORT = 271,                  /* IMPORT  */
    CONST = 272,                   /* CONST  */
    INTEGERLITERAL = 273,          /* INTEGERLITERAL  */
    LONGLITERAL = 274,             /* LONGLITERAL  */
    REALLITERAL = 275,             /* REALLITERAL  */
    DOUBLELITERAL = 276,           /* DOUBLELITERAL  */
    BOOLEANLITERAL = 277,          /* BOOLEANLITERAL  */
    CHARACTERLITERAL = 278,        /* CHARACTERLITERAL  */
    STRINGLITERAL = 279,           /* STRINGLITERAL  */
    NULLLITERAL = 280,             /* NULLLITERAL  */
    IDENT = 281,                   /* IDENT  */
    ASSIGNMENT = 282,              /* ASSIGNMENT  */
    ADD_ASSIGN = 283,              /* ADD_ASSIGN  */
    SUB_ASSIGN = 284,              /* SUB_ASSIGN  */
    MUL_ASSIGN = 285,              /* MUL_ASSIGN  */
    DIV_ASSIGN = 286,              /* DIV_ASSIGN  */
    MOD_ASSIGN = 287,              /* MOD_ASSIGN  */
    ADD = 288,                     /* ADD  */
    SUB = 289,                     /* SUB  */
    MUL = 290,                     /* MUL  */
    DIV = 291,                     /* DIV  */
    MOD = 292,                     /* MOD  */
    INCR = 293,                    /* INCR  */
    DECR = 294,                    /* DECR  */
    EQ = 295,                      /* EQ  */
    NEQ = 296,                     /* NEQ  */
    GTE = 297,                     /* GTE  */
    LTE = 298,                     /* LTE  */
    REF_EQ = 299,                  /* REF_EQ  */
    REF_NEQ = 300,                 /* REF_NEQ  */
    AND = 301,                     /* AND  */
    OR = 302,                      /* OR  */
    NOT = 303,                     /* NOT  */
    SAFE_CALL = 304,               /* SAFE_CALL  */
    ELVIS = 305,                   /* ELVIS  */
    QUEST = 306,                   /* QUEST  */
    RANGE = 307,                   /* RANGE  */
    RANGE_UNTIL = 308,             /* RANGE_UNTIL  */
    AS = 309,                      /* AS  */
    LPAREN = 310,                  /* LPAREN  */
    RPAREN = 311,                  /* RPAREN  */
    LCURL = 312,                   /* LCURL  */
    RCURL = 313,                   /* RCURL  */
    LSQUARE = 314,                 /* LSQUARE  */
    RSQUARE = 315,                 /* RSQUARE  */
    LANGLE = 316,                  /* LANGLE  */
    RANGLE = 317,                  /* RANGLE  */
    COMMA = 318,                   /* COMMA  */
    DOT = 319,                     /* DOT  */
    COLON = 320,                   /* COLON  */
    SEMI = 321,                    /* SEMI  */
    ARROW = 322,                   /* ARROW  */
    DOUBLE_COLON = 323             /* DOUBLE_COLON  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_K0GRAM_TAB_H_INCLUDED  */
