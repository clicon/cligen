/*
  ***** BEGIN LICENSE BLOCK *****
 
  Copyright (C) 2001-2022 Olof Hagsand

  This file is part of CLIgen.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  Alternatively, the contents of this file may be used under the terms of
  the GNU General Public License Version 2 or later (the "GPL"),
  in which case the provisions of the GPL are applicable instead
  of those above. If you wish to allow use of your version of this file only
  under the terms of the GPL, and not to allow others to
  use your version of this file under the terms of Apache License version 2, indicate
  your decision by deleting the provisions above and replace them with the 
  notice and other provisions required by the GPL. If you do not delete
  the provisions above, a recipient may use your version of this file under
  the terms of any one of the Apache License version 2 or the GPL.

  ***** END LICENSE BLOCK *****

*/

%{


#include "cligen_config.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <netinet/in.h>
#include <ctype.h>
#ifndef isblank
#define isblank(c) (c==' ')
#endif /* isblank */

#include "cligen_parse.tab.h" /* generated */

#include "cligen_buf.h"
#include "cligen_cv.h"
#include "cligen_cvec.h"
#include "cligen_parsetree.h"
#include "cligen_callback.h"
#include "cligen_object.h"
#include "cligen_parse.h"

/* Redefine main lex function so that you can send arguments to it */
#define YY_DECL int cligen_parselex(void *_cy)

/* typecast macro */
#define _CY ((cligen_yacc *)_cy)

#undef cligen_parsewrap
int 
cligen_parsewrap(void)
{
  return 1;
}

/*! like strdup but strip \:s */
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
%s CHOICE
%s ESCAPE
%s STRING

%%
<INITIAL>[ \t]*
<INITIAL>,                { BEGIN(OPTION); return *yytext; }
<INITIAL>#                { _CY->cy_lex_state =INITIAL; BEGIN(COMMENT); }
<INITIAL>\n               { _CY->cy_linenum++; }
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
<INITIAL>"@{"             { return SETS; }
<INITIAL>\{               { return *yytext; }
<INITIAL>\}               { return *yytext; }
<INITIAL>\@               { return *yytext; }
<INITIAL>([^@ \t,#\n=;\\<\(\)\[\]\|\{\}]|\\.)+  { 
                            cligen_parselval.string = stripdup(yytext);
                            return NAME; }
<INITIAL>.                { return -1; }

<HELP>\n                  { _CY->cy_linenum++; 
                            return *yytext; }
<HELP><<EOF>>             { return MY_EOF; }
<HELP>\"\)                { BEGIN(INITIAL); return DQP; /* double-quote parenthes */}
<HELP>\\                  { _CY->cy_lex_state = HELP; BEGIN(ESCAPE); }
<HELP>[^\\"\n]+           { 
                             int i;
                             for (i=0;i<strlen(yytext);i++){
                                if (!isblank(yytext[i]))
                                   break;
                             }
                             cligen_parselval.string = yytext+i;
                             return HELPSTR;
                          }
<HELP>.                   { cligen_parselval.string = yytext;
                            return HELPSTR;}

<NAMEORTYPE>\>            { BEGIN(INITIAL); return *yytext; }
<NAMEORTYPE>\n            { _CY->cy_linenum++; }
<NAMEORTYPE>\:            { return *yytext; }
<NAMEORTYPE>[ \t]+        { BEGIN(VARIABLE); return ' '; }
<NAMEORTYPE>([^ \t>:]|\\.)+  { 
                            cligen_parselval.string = strdup(yytext);
                            return NAME; }

<VARIABLE>[ \t]+          { return ' '; }
<VARIABLE>\n              { _CY->cy_linenum++; }
<VARIABLE>\>              { BEGIN(INITIAL); return *yytext; }
<VARIABLE>\:              { return *yytext; }
<VARIABLE>\,              { return *yytext; }
<VARIABLE>\|              { return *yytext; }
<VARIABLE>\(              { return *yytext; }
<VARIABLE>\)              { return *yytext; }
<VARIABLE>\[              { return *yytext; }
<VARIABLE>\]              { return *yytext; }
<VARIABLE>!               { return *yytext; }
<VARIABLE>\"              { _CY->cy_lex_string_state =VARIABLE;BEGIN(STRING); return DQ; }
<VARIABLE>show            { return V_SHOW; }
<VARIABLE>range           { return V_RANGE; }
<VARIABLE>length          { return V_LENGTH; }
<VARIABLE>fraction-digits { return V_FRACTION_DIGITS; }
<VARIABLE>choice:         { BEGIN(CHOICE);return V_CHOICE; }
<VARIABLE>keyword         { return V_KEYWORD; }
<VARIABLE>regexp          { return V_REGEXP; }
<VARIABLE>translate       { return V_TRANSLATE; }
<VARIABLE>preference      { return V_PREFERENCE; }
<VARIABLE>[-+]?[0-9]+\.[0-9]+ { cligen_parselval.string = strdup(yytext); return DECIMAL;}
<VARIABLE>[-+]?[0-9]+     { cligen_parselval.string = strdup(yytext); return NUMBER;}
<VARIABLE>([^ \t\n>:\|\"\(\)\[\]]|\\.)+ { 
                                 cligen_parselval.string = strdup(yytext);
                                 return NAME; }

<CHOICE>[ \t]+          { return ' '; }
<CHOICE>\n              { _CY->cy_linenum++; }
<CHOICE>\|              { return *yytext; }
<CHOICE>\>              { BEGIN(INITIAL); return *yytext; }
<CHOICE>[-+]?[0-9]+\.[0-9]+ { cligen_parselval.string = strdup(yytext); return DECIMAL;}
<CHOICE>[-+]?[0-9]+     { cligen_parselval.string = strdup(yytext); return NUMBER;}
<CHOICE>([^ \t\n>\|\"\(\)\[\]]|\\.)+ { 
                                 cligen_parselval.string = strdup(yytext);
                                 return NAME; }

<OPTION>[ \t]+            
<OPTION>,                { return *yytext; }
<OPTION>#                 { _CY->cy_lex_state =INITIAL; BEGIN(COMMENT); }
<OPTION>\n                { _CY->cy_linenum++; }
<OPTION>=                 { return *yytext; }
<OPTION>;                 { BEGIN(INITIAL); return *yytext;}
<OPTION><<EOF>>           { return MY_EOF; }
<OPTION>\(                { return *yytext; }
<OPTION>\)                { return *yytext; }
<OPTION>\{                { BEGIN(INITIAL); return *yytext;}
<OPTION>\"                { _CY->cy_lex_string_state =INITIAL;BEGIN(STRING); return DQ; }
<OPTION>([^ \t,#\n=;\(\)\{\}\"]|\\.)+   { 
                                 cligen_parselval.string = strdup(yytext);
                                 return NAME; }
<OPTION>.                 { return -1;}

<STRING>\n                { _CY->cy_linenum++; }
<STRING>\"                { BEGIN(_CY->cy_lex_string_state); return DQ; }
<STRING>\\\"              { cligen_parselval.string = strdup(yytext);
                            return CHARS;}
<STRING>\\              { cligen_parselval.string = strdup(yytext);
                            return CHARS;}
<STRING>[^\\"\n]+         { cligen_parselval.string = strdup(yytext);
                            return CHARS;}

<ESCAPE>.                 { BEGIN(_CY->cy_lex_state); 
                             cligen_parselval.string = strdup(yytext); 
                             return CHARS; }

<COMMENT>\n               { _CY->cy_linenum++; BEGIN(_CY->cy_lex_state);}
<COMMENT><<EOF>>          { return MY_EOF; }
<COMMENT>[^\n]+   

%%

/*! Initialize scanner.
 */
int
cgl_init(cligen_yacc *cy)
{
  BEGIN(INITIAL);
  cy->cy_lexbuf = yy_scan_string (cy->cy_parse_string);
#if 1 /* XXX: just to use unput to avoid warning  */
  if (0)
    yyunput(0, ""); 
#endif

  return 0;
}

/*! Exit cligen lex parser. Free buffers
 * Even within Flex version 2.5 (this is assumed), freeing buffers is different.
 */
int
cgl_exit(cligen_yacc *cy)
{
    yy_delete_buffer(cy->cy_lexbuf);
    cligen_parselex_destroy();  /* modern */
    return 0;
}


