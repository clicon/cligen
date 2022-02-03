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

#include "cligen_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "cligen_buf.h"
#include "cligen_cv.h"
#include "cligen_cvec.h"
#include "cligen_callback.h"

cgv_fnstype_t *
co_callback_fn_get(cg_callback *cc)
{
    return cc->cc_fn_vec;
}

int
co_callback_fn_set(cg_callback   *cc,
		   cgv_fnstype_t *fn)
{
    cc->cc_fn_vec = fn;
    return 0;
}

cg_callback *
co_callback_next(cg_callback *cc)
{
    return cc->cc_next;
}

/*! Copy a linked list of cg_obj callback objects
 *
 * Copy a linked list of cg_obj callback objects, including function pointer, 
 * function name,
 *
 * @param[in]  cc0  The object to copy from
 * @param[out] ccn  Pointer to the object to copy to (is allocated)
 * @param[in]  cgv  if given, is a string that overrides the arg in cc.
 * @retval     0      OK
 * @retval     -1     Error
 */
int
co_callback_copy(cg_callback  *cc0, 
		 cg_callback **ccn)
{
    int                  retval = -1;
    struct cg_callback  *cc;
    struct cg_callback  *cc1 = NULL;
    struct cg_callback **ccp;

    ccp = ccn;
    for (cc = cc0; cc; cc=cc->cc_next){
	if ((cc1 = malloc(sizeof(*cc1))) == NULL)
	    goto done;
	memset(cc1, 0, sizeof(*cc1));
	cc1->cc_fn_vec = cc->cc_fn_vec;
	if (cc->cc_fn_str)
	    if ((cc1->cc_fn_str = strdup(cc->cc_fn_str)) == NULL)
		goto done;
	if (cc->cc_cvec && ((cc1->cc_cvec = cvec_dup(cc->cc_cvec)) == NULL))
	    goto done;
	*ccp = cc1;
	ccp = &cc1->cc_next;
	cc1 = NULL;
    }
    retval = 0;
 done:
    if (cc1)
	free(cc1);
    return retval;
}

/*! Free a single callback structure
 *
 * @param[in]  cc   Callback object
 */
static int
co_callback_one_free(cg_callback *cc)
{
    if (cc->cc_cvec)	
	cvec_free(cc->cc_cvec);
    if (cc->cc_fn_str)	
	free(cc->cc_fn_str);
    free(cc);
    return 0;
}

/*! Free a linked list of callbacks
 *
 * @param[in]  ccp   Pointer to head of callback linked list
 */
int
co_callbacks_free(cg_callback **ccp)
{
   cg_callback *cc;
   
   while ((cc = *ccp) != NULL){
	*ccp = co_callback_next(cc);
	co_callback_one_free(cc);
   }
    return 0;
}


/*! Print a CLIgen object callback to a CLIgen buffer
 */
int
co_callback2cbuf(cbuf        *cb,
		 cg_callback *cc)
{
    int     i;
    cg_var *cv;
    
    if (cc->cc_fn_str){
	cprintf(cb, ", %s(", cc->cc_fn_str);
	if (cc->cc_cvec){
	    cv = NULL;
	    i = 0;
	    while ((cv = cvec_each(cc->cc_cvec, cv)) != NULL) {
		if (i++)
		    cprintf(cb, ",");
		cprintf(cb, "\"");
		cv2cbuf(cv, cb);
		cprintf(cb, "\"");
	    }
	}
	cprintf(cb, ")");
    }
    return 0;
}

size_t
co_callback_size(cg_callback *cc)
{
    size_t sz = 0;

    sz = sizeof(struct cg_callback);
    if (cc->cc_fn_str)
	sz += strlen(cc->cc_fn_str) + 1;
    if (cc->cc_cvec)
	sz += cvec_size(cc->cc_cvec);
    return sz;
}
