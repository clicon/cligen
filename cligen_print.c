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

#include "cligen_buf.h"
#include "cligen_var.h"
#include "cligen_cvec.h"
#include "cligen_gen.h"
#include "cligen_print.h"

#define VARIABLE_PRE  '<'
#define VARIABLE_POST '>'


/* Static prototypes */
static int pt2cbuf(cbuf *cb, parse_tree pt, int level, int brief);

/*! Print the syntax specification of a variable syntax spec to a cligen buf
 *
 * @param co    [in]   cligen object from where to print
 * @param cmd   [out]  string to print to
 * @param len   [in]   Length of string
 * @param brief [in]   If set, just <varname>, otherwise clispec parsable format
 * @retval      0      Everything OK
 * @retval      -1     Error
 * @code
    cbuf *cb = cbuf_new();
    cov2cbuf(cb, co, brief);
    cbuf_free(cb);
 * @endcode
 *
 * Brief output omits help-strings and variable options except names. Eg:
 * brief=0:  a("help string") <x:int32>("variable"), cb();
 * brief=1:  a <x>;
 */
int 
cov2cbuf(cbuf *cb, cg_obj *co, int brief)
{
    int            retval = -1;

    if (co->co_choice){
	if (strchr(co->co_choice, '|'))
	    cprintf(cb, "(%s)", co->co_choice);
	else
	    cprintf(cb, "%s", co->co_choice);
    }
    else{
	if (brief)
	    cprintf(cb, "%c%s%c", VARIABLE_PRE, 
		    co->co_show ? co->co_show : co->co_command, 
		    VARIABLE_POST);   
	else{
	    cprintf(cb, "%c%s:%s", VARIABLE_PRE, co->co_command, cv_type2str(co->co_vtype));
	    if (co->co_range){
		if (cv_isint(co->co_vtype))
		    cprintf(cb, " range[");
		else
		    cprintf(cb, " length[");
		if (co->co_rangecv_low){
		    cv2cbuf(co->co_rangecv_low, cb);
		    cprintf(cb, ":");
		}
		if (co->co_rangecv_high){
		    cv2cbuf(co->co_rangecv_high, cb);
		}
		cprintf(cb, "]");
	    }
	    if (co->co_show)
		cprintf(cb, " show:\"%s\"", co->co_show);
	    if (co->co_expand_fn_str){
		cprintf(cb, " %s(\"", co->co_expand_fn_str);
		if (co->co_expand_fn_arg)
		    cv2cbuf(co->co_expand_fn_arg, cb);
		cprintf(cb, "\")");
	    }
	    if (co->co_regex)
		cprintf(cb, " regexp:\"%s\"", co->co_regex);
	    cprintf(cb, "%c", VARIABLE_POST);
	}
    }
    retval = 0;
//  done:

    return retval;
}


/* is a terminal command, and therefore should be printed with a ';' */
static int
terminal(cg_obj *co)
{
    return ((co->co_pt.pt_len>0 && co->co_pt.pt_vec[0] == NULL) || 
	    co->co_pt.pt_len == 0);
}

static int 
co2cbuf(cbuf *cb, cg_obj *co, int marginal, int brief)
{
    int retval = -1;
    struct cg_callback *cc;

    assert(co->co_command!=NULL);
    switch (co->co_type){
    case CO_COMMAND:
	cprintf(cb, "%s", co->co_command);
	break;
    case CO_REFERENCE:
	cprintf(cb, "@%s", co->co_command);
	break;
    case CO_VARIABLE:
	cov2cbuf(cb, co, brief);
	break;
    }
    if (brief == 0){
	if (co->co_help)
	    cprintf(cb, "(\"%s\")", co->co_help);
	if (co->co_hide)
	    cprintf(cb, ", hide");
	for (cc = co->co_callbacks; cc; cc=cc->cc_next){
	    if (cc->cc_fn_str){
		cprintf(cb, ", %s(", cc->cc_fn_str);
		if (cc->cc_arg)
		    cprintf(cb, "%s", cc->cc_arg);
		cprintf(cb, ")");
	    }
	}
    }
    if (terminal(co))
	cprintf(cb, ";");
    if (co->co_pt.pt_len>1)
	cprintf(cb, "{\n");
    else
	if (co->co_pt.pt_len==1 && co->co_pt.pt_vec[0] != NULL)
	    cprintf(cb, " ");
	else
	    cprintf(cb, "\n");
    if (pt2cbuf(cb, co->co_pt, marginal+3, brief) < 0)
	goto done;
    if (co->co_pt.pt_len>1){
	cprintf(cb, "%*s", marginal, ""); 
	cprintf(cb, "}\n");
    }
    retval = 0;
  done:
    return retval;
}

static int 
pt2cbuf(cbuf *cb, parse_tree pt, int marginal, int brief)
{
    int retval = -1;
    int i;

    for (i=0; i<pt.pt_len; i++){
	if (pt.pt_vec[i] == NULL){
	    continue;
	}
	if (pt.pt_len > 1)
	    cprintf(cb, "%*s", marginal, "");
	if (co2cbuf(cb, pt.pt_vec[i], marginal, brief) < 0)
	    goto done;
    }
    retval = 0;
  done:
    return retval;
}

/*! Print CLIgen parse-tree to file, brief or detailed.
 *
 * @param f     [in] File to print to
 * @param pt    [in] Cligen parse-tree consisting of cg objects and variables
 * @param brief [in] Print brief output, otherwise clispec parsable format
 *
 * The output may not be identical to the input syntax. 
 * For example [dd|ee] is printed as:
 *   dd;
 *   ee:
 * Brief output omits help-strings and variable options except names. Eg:
 * brief=0:  a("help string") <x:int32>("variable"), cb();
 * brief=1:  a <x>;
 */
int 
cligen_print(FILE *f, parse_tree pt, int brief)
{
    int   retval = -1;
    cbuf *cb = NULL;

    if ((cb = cbuf_new()) == NULL){
	fprintf(stderr, "cbuf_new: %s\n", strerror(errno));
	goto done;
    }
    if (pt2cbuf(cb, pt, 0, brief) < 0)
	goto done;
    fprintf(f, "%s", cbuf_get(cb));
    retval = 0;
  done:
    if (cb)
	cbuf_free(cb);
    return retval;
}
