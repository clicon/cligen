/*
  CLI generator input/output support functions.

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

  This file includes utility functions (good-to-have) for CLIgen applications
 */
#include "cligen_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>

#include "cligen_buf.h"
#include "cligen_cv.h"
#include "cligen_cvec.h"
#include "cligen_parsetree.h"
#include "cligen_pt_head.h"
#include "cligen_callback.h"
#include "cligen_object.h"
#include "cligen_handle.h"
#include "cligen_result.h"
#include "cligen_read.h"
#include "cligen_util.h"

/*! Utility function to append string cv to a cligen vector
 * It is a utility function since it can be made using existing cv and cvec functions
 * but common since string type is the most common
 * @param[in]   cvv  cligen variable vector
 * @param[in]   name Name to set (or NULL)
 * @param[val]  val  String value
 * @retval  0  OK
 * @retval -1  Error
 */
int
cvec_add_string(cvec *cvv, 
		char *name, 
		char *val)
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
 * @param[in]  h  cligen handle
 * @retval  0  OK
 * @retval -1  Error
 */
int 
cligen_loop(cligen_handle h)
{
    int           retval = -1;
    char         *line;
    int           callback_ret = 0;
    char         *reason = NULL;
    cligen_result result;
    
    /* Run the CLI command interpreter */
    while (!cligen_exiting(h)){
	if (cliread_eval(h, &line, &callback_ret, &result, &reason) < 0)
	    goto done;
	switch (result){
	case CG_EOF: /* eof */
	    cligen_exiting_set(h, 1);
	    break;
	case CG_ERROR: /* cligen match errors */
	    printf("CLI read error\n");
	    goto done;
	case CG_NOMATCH: /* no match */
	    printf("CLI syntax error in: \"%s\": %s\n", line, reason);
	    break;
	case CG_MATCH: /* unique match */
	    if (callback_ret < 0)
		printf("CLI callback error\n");
	    break;
	default: /* multiple matches */
	    printf("Ambiguous command\n");
	    break;
	}
	if (reason){
	    free(reason);
	    reason = NULL;
	}
    }
    retval = 0;
 done:
    if (reason)
	free(reason);
    return retval;
}
