/*
  CVS Version: $Id: cligen_print.c,v 1.19 2013/06/16 11:55:55 olof Exp $ 

  Copyright (C) 2001-2013 Olof Hagsand

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
/*! \file */ 
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

#define VARIABLE_PRE  '<'
#define VARIABLE_POST '>'


/* Static prototypes */
static int pt_print(FILE *f, parse_tree pt, int level, int brief);

/*! 
 * \brief Print the syntax specification of a variable syntax spec to string
 *
 * That is, the inverse of parsing.
 * But it is not complete, it actually only prints the name within <>
 * and no comments.
 * Example, a string variable with name foo is printed as <foo>
 * Used as help during completion, syntax prints, etc.
 * Input args:
 *  detail:   If set show clispec parsable format, else just <varname>
 */
int 
cov_print(cg_obj *co, char *cmd, int len, int brief)
{
    char          *cmd2;

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

	    cmd2 = strdup(cmd);
	    if (co->co_range){
		snprintf(cmd, len, "%s range[%" PRId64 ":%" PRId64 "]", 
			 cmd2, co->co_range_low, co->co_range_high);
		free(cmd2);
		cmd2 = strdup(cmd);
	    }
	    snprintf(cmd, len, "%s%c", cmd2, VARIABLE_POST);
	    free(cmd2);
	}
    }

    return 0;
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

/*! 
 * \brief  Print CLIgen parse-tree to file, brief or detailed.
 */
int 
cligen_print(FILE *f, parse_tree pt, int brief)
{
    return pt_print(f, pt, 0, brief);
}
