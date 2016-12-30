/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

#ifndef YY_CLIGEN_PARSE_Y_TAB_H_INCLUDED
# define YY_CLIGEN_PARSE_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int cligen_parsedebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    MY_EOF = 258,
    V_RANGE = 259,
    V_LENGTH = 260,
    V_CHOICE = 261,
    V_KEYWORD = 262,
    V_REGEXP = 263,
    V_FRACTION_DIGITS = 264,
    V_SHOW = 265,
    V_TREENAME = 266,
    DOUBLEPARENT = 267,
    DQ = 268,
    DQP = 269,
    PDQ = 270,
    NAME = 271,
    NUMBER = 272,
    DECIMAL = 273,
    CHAR = 274
  };
#endif
/* Tokens.  */
#define MY_EOF 258
#define V_RANGE 259
#define V_LENGTH 260
#define V_CHOICE 261
#define V_KEYWORD 262
#define V_REGEXP 263
#define V_FRACTION_DIGITS 264
#define V_SHOW 265
#define V_TREENAME 266
#define DOUBLEPARENT 267
#define DQ 268
#define DQP 269
#define PDQ 270
#define NAME 271
#define NUMBER 272
#define DECIMAL 273
#define CHAR 274

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{


  int intval;
  char *string;


};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE cligen_parselval;

int cligen_parseparse (void *_ya);

#endif /* !YY_CLIGEN_PARSE_Y_TAB_H_INCLUDED  */
