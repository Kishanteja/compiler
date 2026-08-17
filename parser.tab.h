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

#ifndef YY_YY_PARSER_TAB_H_INCLUDED
# define YY_YY_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 32 "parser.y"

#include "ast.hpp"
#include "symtab.hpp"

#line 54 "parser.tab.h"

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    INTEGER = 258,                 /* INTEGER  */
    FLOAT = 259,                   /* FLOAT  */
    STRING = 260,                  /* STRING  */
    VOID = 261,                    /* VOID  */
    BOOL = 262,                    /* BOOL  */
    NAME = 263,                    /* NAME  */
    INT_NUM = 264,                 /* INT_NUM  */
    FLOAT_NUM = 265,               /* FLOAT_NUM  */
    STR_CONST = 266,               /* STR_CONST  */
    BOOL_CONST = 267,              /* BOOL_CONST  */
    READ = 268,                    /* READ  */
    WRITE = 269,                   /* WRITE  */
    RETURN = 270,                  /* RETURN  */
    ASSIGN_OP = 271,               /* ASSIGN_OP  */
    PLUS = 272,                    /* PLUS  */
    MINUS = 273,                   /* MINUS  */
    MUL = 274,                     /* MUL  */
    DIV = 275,                     /* DIV  */
    OR = 276,                      /* OR  */
    AND = 277,                     /* AND  */
    NOT = 278,                     /* NOT  */
    EQUAL = 279,                   /* EQUAL  */
    NOT_EQUAL = 280,               /* NOT_EQUAL  */
    GREATER = 281,                 /* GREATER  */
    GREATER_EQUAL = 282,           /* GREATER_EQUAL  */
    LESS = 283,                    /* LESS  */
    LESS_EQUAL = 284,              /* LESS_EQUAL  */
    QUESTION_MARK = 285,           /* QUESTION_MARK  */
    COLON = 286,                   /* COLON  */
    SEMICOLON = 287,               /* SEMICOLON  */
    COMMA = 288,                   /* COMMA  */
    LEFT_ROUND_BRACKET = 289,      /* LEFT_ROUND_BRACKET  */
    RIGHT_ROUND_BRACKET = 290,     /* RIGHT_ROUND_BRACKET  */
    LEFT_CURLY_BRACKET = 291,      /* LEFT_CURLY_BRACKET  */
    RIGHT_CURLY_BRACKET = 292,     /* RIGHT_CURLY_BRACKET  */
    IF = 293,                      /* IF  */
    ELSE = 294,                    /* ELSE  */
    WHILE = 295,                   /* WHILE  */
    DO = 296,                      /* DO  */
    UMINUS = 297,                  /* UMINUS  */
    LOWER_THAN_ELSE = 298          /* LOWER_THAN_ELSE  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 37 "parser.y"
 ASTNode *node; char *str; 

#line 117 "parser.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_PARSER_TAB_H_INCLUDED  */
