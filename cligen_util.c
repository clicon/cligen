/*
  CLI generator input/output support functions.

  Copyright (C) 2001-2016 Olof Hagsand

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

  This file includes utility functions (good-to-have) for CLIgen applications
 */
#include "cligen_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>

#include "cligen_buf.h"
#include "cligen_var.h"
#include "cligen_cvec.h"
#include "cligen_gen.h"
#include "cligen_handle.h"
#include "cligen_read.h"
#include "cligen_util.h"

/*! Utility function to append string cv to a cligen vector
 * It is a utility function since it can be made using existing cv and cvec functions
 * but common since string type is the most commo
 * @param[in]   cvv  cligen variable vector
 * @param[in]   name Name to set (or NULL)
 * @param[val]  val  String value
 * @retval  0  OK
 * @retval -1  Error
 * See also 
 */
int
cvec_add_string(cvec *cvv, char *name, char *val)
{
    cg_var *cv;
    int     retval = -1;

    if ((cv = cvec_add(cvv, CGV_STRING)) == NULL)
	goto done;
    if (name)
	cv_name_set(cv, name);
    cv_string_set(cv, val);
    retval = 0;
 done:
    return retval;
}

/*! Utility function for main cligen event loop.
 * You probably should copy this function and make your own main event loop
 * @param   h  cligen handle
 * @retval  0  OK
 * @retval -1  Error
 */
int 
cligen_loop(cligen_handle h)
{
    int         retval = -1;
    char       *line;
    int         callback_ret = 0;

    /* Run the CLI command interpreter */
    while (!cligen_exiting(h)){
	switch (cliread_eval(h, &line, &callback_ret)){
	case CG_EOF: /* eof */
	    goto done;
	    break;
	case CG_ERROR: /* cligen match errors */
	    printf("CLI read error\n");
	    goto done;
	case CG_NOMATCH: /* no match */
	    printf("CLI syntax error in: \"%s\": %s\n", line, cligen_nomatch(h));
	    break;
	case CG_MATCH: /* unique match */
	    if (callback_ret < 0)
		printf("CLI callback error\n");
	    break;
	default: /* multiple matches */
	    printf("Ambigous command\n");
	    break;
	}
    }
    retval = 0;
 done:
    return retval;
}
