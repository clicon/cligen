/*
  Copyright (C) 2001-2014 Olof Hagsand

  This file is part of CLIgen.

  CLIgen is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  CLIgen is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with CLIgen; see the file COPYING.
*/

%{


#include "cligen_config.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <netinet/in.h>

#include "cligen_parse.tab.h" /* generated */

#include "cligen_buf.h"
#include "cligen_var.h"
#include "cligen_cvec.h"
#include "cligen_gen.h"
#include "cligen_parse.h"

/* Redefine main lex function so that you can send arguments to it */
#define YY_DECL int cligen_parselex(void *_ya)

/* typecast macro */
#define _YA ((struct cligen_parse_yacc_arg *)_ya)

#define     MAXBUF  4*4*64*1024

#define MAX(x,y) ((x)>(y)?(x):(y))
#define MIN(x,y) ((x)<(y)?(x):(y))

#undef cligen_parsewrap
int 
cligen_parsewrap(void)
{
  return 1;
}

/* like strdup but strip \:s */
static char *
stripdup(char *s0)
{
  char *s1;
  char *s;

  if ((s1 = strdup(s0)) == NULL){
    fprintf(stderr, "%s: strdup: %s\n", __FUNCTION__, strerror(errno));
    return NULL;
  }
  while ((s = index(s1, '\\')) != NULL)
    memmove(s, s+1, strlen(s)); 
  return s1;
}

/*
  See block_chart for lex states.
*/

%}

%s OPTION
%s HELP
%s COMMENT
%s NAMEORTYPE
%s VARIABLE
%s ESCAPE
%s STRING

%%
<INITIAL>[ \t]            
<INITIAL>,                { BEGIN(OPTION); return *yytext; }
<INITIAL>#                { _YA->ya_lex_state =INITIAL; BEGIN(COMMENT); }
<INITIAL>\n               { _YA->ya_linenum++; }
<INITIAL>=                { BEGIN(OPTION); return *yytext; }
<INITIAL>;                { return *yytext; }
<INITIAL><<EOF>>          { return MY_EOF; }
<INITIAL>\<               { BEGIN(NAMEORTYPE); return *yytext; }
<INITIAL>\(               { return *yytext; }
<INITIAL>\)               { return *yytext; }
<INITIAL>\|               { return *yytext; }
<INITIAL>\(\"             { BEGIN(HELP); return PDQ; /* parenth double-q*/  }
<INITIAL>\[               { return *yytext; }
<INITIAL>\]               { return *yytext; }
<INITIAL>\{               { return *yytext; }
<INITIAL>\}               { return *yytext; }
<INITIAL>\@               { return *yytext; }
<INITIAL>([^@ \t,#\n=;\\<\(\)\[\]\|\{\}]|\\.)+  { 
                            cligen_parselval.string = stripdup(yytext);
                            return NAME; }
<INITIAL>.                { return -1; }

<HELP>\n                  { _YA->ya_linenum++; }
<HELP><<EOF>>             { return MY_EOF; }
<HELP>\"\)                { BEGIN(INITIAL); return DQP; /* double-quote parenthes */}
<HELP>\\                  { _YA->ya_lex_state = HELP; BEGIN(ESCAPE); }
<HELP>.                   { cligen_parselval.string = strdup(yytext);
                            return CHAR;}

<NAMEORTYPE>\>            { BEGIN(INITIAL); return *yytext; }
<NAMEORTYPE>\n            { _YA->ya_linenum++; }
<NAMEORTYPE>\:            { return *yytext; }
<NAMEORTYPE>[ \t]+        { BEGIN(VARIABLE); return ' '; }
<NAMEORTYPE>([^ \t>:]|\\.)+  { 
                            cligen_parselval.string = strdup(yytext);
                            return NAME; }

<VARIABLE>[ \t]+          { return ' '; }
<VARIABLE>\n              { _YA->ya_linenum++; }
<VARIABLE>\>              { BEGIN(INITIAL); return *yytext; }
<VARIABLE>\:              { return *yytext; }
<VARIABLE>\|              { return *yytext; }
<VARIABLE>\(              { return *yytext; }
<VARIABLE>\)              { return *yytext; }
<VARIABLE>\[              { return *yytext; }
<VARIABLE>\]              { return *yytext; }
<VARIABLE>\"              { _YA->ya_lex_string_state =VARIABLE;BEGIN(STRING); return DQ; }
<VARIABLE>show            { return V_SHOW; }
<VARIABLE>range           { return V_RANGE; }
<VARIABLE>length          { return V_LENGTH; }
<VARIABLE>fraction-digits { return V_FRACTION_DIGITS; }
<VARIABLE>choice          { return V_CHOICE; }
<VARIABLE>keyword         { return V_KEYWORD; }
<VARIABLE>regexp          { return V_REGEXP; }
<VARIABLE>[-+]?[0-9]+\.[0-9]+ { cligen_parselval.string = strdup(yytext); return DECIMAL;}
<VARIABLE>[-+]?[0-9]+     { cligen_parselval.string = strdup(yytext); return NUMBER;}
<VARIABLE>([^ \t\n>:\|\"\(\)\[\]]|\\.)+ { 
                                 cligen_parselval.string = strdup(yytext);
                                 return NAME; }

<OPTION>[ \t]+            
<OPTION>,                { return *yytext; }
<OPTION>#                 { _YA->ya_lex_state =INITIAL; BEGIN(COMMENT); }
<OPTION>\n                { _YA->ya_linenum++; }
<OPTION>=                 { return *yytext; }
<OPTION>;                { BEGIN(INITIAL); return *yytext;}
<OPTION><<EOF>>           { return MY_EOF; }
<OPTION>\(                { return *yytext; }
<OPTION>\)                { return *yytext; }
<OPTION>\{                { BEGIN(INITIAL); return *yytext;}
<OPTION>\"                { _YA->ya_lex_string_state =INITIAL;BEGIN(STRING); return DQ; }
<OPTION>([^ \t,#\n=;\(\)\{\}\"]|\\.)+   { 
                                 cligen_parselval.string = strdup(yytext);
                                 return NAME; }
<OPTION>.                 { return -1;}

<STRING>\n                { _YA->ya_linenum++; }
<STRING>\\                { _YA->ya_lex_state = STRING; BEGIN(ESCAPE); }
<STRING>\"                { BEGIN(_YA->ya_lex_string_state); return DQ; }
<STRING>.                 { cligen_parselval.string = strdup(yytext);
                            return CHAR;}

<ESCAPE>.                 { BEGIN(_YA->ya_lex_state); 
                             cligen_parselval.string = strdup(yytext); 
                             return CHAR; }

<COMMENT>\n               { _YA->ya_linenum++; BEGIN(_YA->ya_lex_state);}
<COMMENT><<EOF>>          { return MY_EOF; }
<COMMENT>.               


%%

/*
 * cgl_init
 * Initialize scanner.
 */
int
cgl_init(struct cligen_parse_yacc_arg *ya)
{
  BEGIN(INITIAL);
  ya->ya_lexbuf = yy_scan_string (ya->ya_parse_string);
#if 1 /* XXX: just to use unput to avoid warning  */
  if (0)
    yyunput(0, ""); 
#endif

  return 0;
}

/*
 * cgl_exit
 * free buffers
 * Even within Flex version 2.5 (this is assumed), freeing buffers is different.
 */
int
cgl_exit(struct cligen_parse_yacc_arg *ya)
{
    yy_delete_buffer(ya->ya_lexbuf);
#if defined(YY_FLEX_SUBMINOR_VERSION) && YY_FLEX_SUBMINOR_VERSION >= 9
    cligen_parselex_destroy();  /* modern */
#else
    yy_init = 1;      /* This does not quite free all buffers */
#endif
    return 0;
}


