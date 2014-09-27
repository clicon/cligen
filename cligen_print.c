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
#include "cligen_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <inttypes.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "cligen_var.h"
#include "cligen_cvec.h"
#include "cligen_gen.h"
#include "cligen_print.h"
#include "cligen_buf.h"

#define VARIABLE_PRE  '<'
#define VARIABLE_POST '>'


/* Static prototypes */
static int pt_print(FILE *f, parse_tree pt, int level, int brief);

/*! Print the syntax specification of a variable syntax spec to string
 *
 * That is, the inverse of parsing.
 * But it is not complete, it actually only prints the name within <>
 * and no comments.
 * Example, a string variable with name foo is printed as \<foo>
 * Used as help during completion, syntax prints, etc.
 * XXX: And it does not honor len properly
 * XXX: In transformation of using cbufs
 *
 * @param co   [in]    cligen object from where to print
 * @param cmd  [out]   string to print to
 * @param len  [in]    Length of string
 * @param brief [in]   If set, just \<varname>, otherwise clispec parsable format
 */
int 
cov_print(cg_obj *co, char *cmd, int len, int brief)
{
    int            retval = -1;
    char          *cmd2;
    char          *cv;
    cbuf          *cb = NULL;

    if ((cb = cbuf_new()) == NULL){
    }
    if (co->co_choice){
	if (strchr(co->co_choice, '|'))
	    snprintf(cmd, len, "(%s)", co->co_choice);
	else
	    snprintf(cmd, len, "%s", co->co_choice);
    }
    else{
	if (brief)
	    snprintf(cmd, len, "%c%s%c", VARIABLE_PRE, co->co_command, VARIABLE_POST);   
	else{
	    snprintf(cmd, len, "%c%s:%s", VARIABLE_PRE, co->co_command, cv_type2str(co->co_vtype));

	    if (co->co_range){
		if (cv_isint(co->co_vtype))
		    strncat(cmd, " range[", len);
		else
		    strncat(cmd, " length[", len);
		if (co->co_rangecv_low){
		    cv2str(co->co_rangecv_low, cmd+strlen(cmd), len);
		    strncat(cmd, ":", len);
		}
		if (co->co_rangecv_high){
		    cv2str(co->co_rangecv_high, cmd+strlen(cmd), len);
		}
		strncat(cmd, "]", len);
	    }
	    cmd2 = strdup(cmd);
	    if (co->co_expand_fn_str){
		if (co->co_expand_fn_arg)
		    cv = cv2str_dup(co->co_expand_fn_arg);
		else
		    cv = NULL;
		snprintf(cmd, len, "%s %s(\"%s\")",  /* XXX: cv2str() */
			 cmd2, co->co_expand_fn_str, cv?cv:"");
		if (cv)
		    free(cv);
		free(cmd2);
		cmd2 = strdup(cmd);
	    }
	    if (co->co_regex){
		snprintf(cmd, len, "%s regexp:\"%s\"", 
			 cmd2, co->co_regex);
		free(cmd2);
		cmd2 = strdup(cmd);
	    }
	    snprintf(cmd, len, "%s%c", cmd2, VARIABLE_POST);
	    free(cmd2);
	}
    }
    retval = 0;
//  done:
    if (cb)
	cbuf_free(cb);
    return retval;
}

/* is a terminal command, and therefore should be printed with a ';' */
static int
terminal(cg_obj *co)
{
    return ((co->co_pt.pt_len>0 && co->co_pt.pt_vec[0] == NULL) || 
	    co->co_pt.pt_len == 0);
}


/* 
 * co_print
 * print cg_objs 
 */
static int 
co_print(FILE *f, cg_obj *co, int marginal, int brief)
{
  char cmd[128];
  struct cg_callback *cc;

  assert(co->co_command!=NULL);
  switch (co->co_type){
  case CO_COMMAND:
      fprintf(f, "%s", co->co_command);
      break;
  case CO_REFERENCE:
      fprintf(f, "@%s", co->co_command);
      break;
  case CO_VARIABLE:
      cov_print(co, cmd, sizeof(cmd), brief);
      fprintf(f, "%s", cmd);
      break;
  }
  if (!brief){
      if (co->co_help)
	  fprintf(f, "(\"%s\")", co->co_help);
      for (cc = co->co_callbacks; cc; cc=cc->cc_next){
	  if (cc->cc_fn_str){
	      fprintf(f, ", %s(", cc->cc_fn_str);
	      if (cc->cc_arg)
		  cv_print(f, cc->cc_arg);
	      fprintf(f, ")");

	  }
      }
  }
  if (terminal(co))
      fprintf(f, ";");
  if (co->co_pt.pt_len>1)
      fprintf(f, "{\n");
  else
      if (co->co_pt.pt_len==1 && co->co_pt.pt_vec[0] != NULL)
	  fprintf(f, " ");
      else
	  fprintf(f, "\n");
  pt_print(f, co->co_pt, marginal+3, brief);
  if (co->co_pt.pt_len>1){
      fprintf(f, "%*s", marginal, ""); 
      fprintf(f, "}\n");
  }
  return 0;
}

/* 
 * pt_print
 * print pattern (vector of cg_objs) recursive 
 * But it is not complete, it strips info that is in the syntax.
 */
static int 
pt_print(FILE *f, parse_tree pt, int marginal, int brief)
{
    int i;

    for (i=0; i<pt.pt_len; i++){
	if (pt.pt_vec[i] == NULL){
	    continue;
	}
	if (pt.pt_len > 1)
	    fprintf(f, "%*s", marginal, "");
	co_print(f, pt.pt_vec[i], marginal, brief);
    }
    return 0;
}

/*! Print CLIgen parse-tree to file, brief or detailed.
 *
 * @param f   [in] File to print to
 * @param pt  [in] Cligen parse-tree consisting of cg objects and variables
 * @param brief [in] Print bried output, otherwise clispec parsable format
 */
int 
cligen_print(FILE *f, parse_tree pt, int brief)
{
    return pt_print(f, pt, 0, brief);
}
