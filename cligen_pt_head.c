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

 * CLIgen parsetree head structure, ie top-level structure holding all info about parse-trees
 * This was all hidden in cligen_handle.c but is now a first-class object. However, the functions
 * are still strange in a way that he underlying parse-tree is sometimes treated as first-level
 * which makes the code strange.
*/
#include "cligen_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <inttypes.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <ctype.h>
#define __USE_GNU /* strverscmp */
#include <string.h>
#include <errno.h>
#include <termios.h>
#include <signal.h>
#include <sys/ioctl.h>

#include "cligen_buf.h"
#include "cligen_cv.h"
#include "cligen_cvec.h"
#include "cligen_parsetree.h"
#include "cligen_pt_head.h"
#include "cligen_callback.h"
#include "cligen_object.h"
#include "cligen_io.h"
#include "cligen_handle.h"
#include "cligen_result.h"
#include "cligen_read.h"
#include "cligen_parse.h"
#include "cligen_history.h"
#include "cligen_getline.h"
#include "cligen_print.h"
#include "cligen_handle_internal.h"

/*
 * Access functions 
 */
/*! Access function to the get name of parsetree header
 * @param[in]  ph    Parse tree header
 * @retval     name  Name
 * @retval     NULL  Error
 */
char*
cligen_ph_name_get(pt_head *ph)
{
    if (ph == NULL){
       errno = EINVAL;
       return NULL;
    }
    return ph->ph_name;
}

/*! Access function to set name of parsetree header
 * @param[in]  ph    Parse tree header
 * @param[in]  name  Name
 * @retval     0     OK
 * @retval    -1     Error
 */
int
cligen_ph_name_set(pt_head *ph,
		   char    *name)
{
    if (ph == NULL){
       errno = EINVAL;
       return -1;
    }
    if (ph->ph_name)
	free(ph->ph_name);
    if (name){
	if ((ph->ph_name = strdup(name)) == NULL)
	    return -1;
    }
    else
	ph->ph_name = NULL;
    return 0;
}

/*! Access function to the working point in a tree, shortcut to implement edit modes.
 * @param[in]  ph   Parse tree header
 * @retval     pt   Parse tree
 * @retval     NULL Error or no parse tree
 */
parse_tree*
cligen_ph_parsetree_get(pt_head *ph)
{
    if (ph == NULL){
       errno = EINVAL;
       return NULL;
    }
    return ph->ph_parsetree;
}

/*! Access function to set parsetree of parse-tree header, remove pt from its parents
 * @param[in]  ph    Parse-tree header
 * @param[in]  pt    parse-tree
 * @retval     0     OK
 * @retval    -1     Error
 */
int
cligen_ph_parsetree_set(pt_head    *ph,
			parse_tree *pt)
{
    int     retval = -1;
    int     i;
    cg_obj *co;
    
    if (ph == NULL){
       errno = EINVAL;
       goto done;
    }
    for (i=0; i<pt_len_get(pt); i++){
	if ((co = pt_vec_i_get(pt, i)) != NULL)
	    co_up_set(co, NULL);
    }
    ph->ph_parsetree = pt; /* XXX not free if exists? */
#if 1 /* This is still used in clixon */
    if (pt_name_set(pt, cligen_ph_name_get(ph)) < 0) /* XXX Is this even necessary ? */
	goto done;
#endif
    retval = 0;
 done:
    return retval;
}

/*! Access function to the working point in a tree, shortcut to implement edit modes.
 * @param[in] h     CLIgen handle
 * @param[in] name  Name of tree
 * @retval    wp    Working point cligen object (shortcut - a sub of this tree)
 * @retval    NULL  No such tree/error
 */
cg_obj *
cligen_ph_workpoint_get(pt_head *ph)
{
    return ph->ph_workpt;

}

/*! Access function to the working point in a tree, shortcut to implement edit modes.
 * @param[in]  pt   Parse tree
 * @param[in]  wp   Working point identified by a cligen object(actually its parse-tree sub vector)
 */
int
cligen_ph_workpoint_set(pt_head *ph,
			cg_obj  *wp)
{
    ph->ph_workpt = wp;
    return 0;
}

/*! Find a parsetree head by its name,
 * @param[in] h       CLIgen handle
 * @param[in] name    Name of tree
 * @retval    ph      Parse-tree header
 * @retval    NULL    Not found
 */
pt_head *
cligen_ph_find(cligen_handle h,
	       char         *name)
{
    pt_head *ph = NULL;
    char    *phname;
    
    for (ph = cligen_pt_head_get(h); ph; ph = ph->ph_next){
	if ((phname = cligen_ph_name_get(ph)) == NULL)
	    continue;
	if (strcmp(phname, name) == 0)
	    break;
    }
    return ph;
}

/*! Free a  parsetree header
 * @param[in]   ph    Parse-tree header
 */
int
cligen_ph_free(pt_head *ph)
{
    if (ph == NULL){
       errno = EINVAL;
       return -1;
    }
    if (ph->ph_name)
	free(ph->ph_name);
    if (ph->ph_parsetree)
	pt_free(ph->ph_parsetree, 1);
    free(ph);
    return 0;
}

/*! Append a new parsetree header
 * @param[in]  h     CLIgen handle
 * @param[in]  name  Name of this tree 
 * @retval     ph    The new parsetree header
 * @retval     NULL  Error
 * Note, if this is the first tree, it is activated by default
 */
pt_head *
cligen_ph_add(cligen_handle h, 
	      char         *name)
{
    pt_head *ph;
    pt_head *phlast;
    
    if ((ph = (pt_head *)malloc(sizeof(*ph))) == NULL)
	goto done;
    memset(ph, 0, sizeof(*ph));    
    if (cligen_ph_name_set(ph, name) < 0){
	free(ph);
	ph = NULL;
	goto done;
    }
    if ((phlast = cligen_pt_head_get(h)) == NULL){
	ph->ph_active++;
	cligen_pt_head_set(h, ph);
    }
    else {
	while (phlast->ph_next)
	    phlast = phlast->ph_next;
	phlast->ph_next = ph;
    }
 done:
    return ph;
}

/*! Iterate through all parsed cligen trees 
 *
 * @param[in] h   CLIgen handle
 * @param[in] pt  Cligen parse-tree iteration variable. Must be initialized to NULL
 * @retval pt     Next parse-tree structure.
 * @retval NULL   When end of list reached.
 * @code
 *    pt_head *ph = NULL;
 *    while ((ph = cligen_ph_each(h, ph)) != NULL) {
 *	     ...
 *    }
 * @endcode
 * Note: you may not delete (or add) parse-tree-heads while iterating through them
 */
pt_head *
cligen_ph_each(cligen_handle h, 
	       pt_head      *ph0)
{
    pt_head *ph = NULL;
    
    if (ph0 == NULL)
	ph = cligen_pt_head_get(h);
    else
	ph = ph0->ph_next;
    return ph;
}

/*! Return i:th parse-tree of parsed cligen trees 
 * @param[in] h  CLIgen handle
 * @param[in] i  Order of element to get
 */
pt_head *
cligen_ph_i(cligen_handle h, 
	    int           i0)
{
    pt_head *ph;
    int      i;

    for (ph = cligen_pt_head_get(h), i=0; ph; ph = ph->ph_next, i++)
	if (i==i0)
	    return ph;
    return NULL;
}

/*! Get currently active parsetree.
 * @param[in] h       CLIgen handle
 * @see cligen_ph_active_get  prefer to use that function instead
 */
parse_tree *
cligen_pt_active_get(cligen_handle h)
{
    pt_head *ph;

    for (ph = cligen_pt_head_get(h); ph; ph = ph->ph_next)
	if (ph->ph_active)
	    return ph->ph_parsetree;
    return NULL;
}

/*! Get currently active parsetree head.
 * @param[in] h       CLIgen handle
 */
pt_head *
cligen_ph_active_get(cligen_handle h)
{
    pt_head *ph;

    for (ph = cligen_pt_head_get(h); ph; ph = ph->ph_next)
	if (ph->ph_active)
	    return ph;
    return NULL;
}

/*! Set currently active parsetree by name
 * @param[in] h       CLIgen handle
 * @retval    0
 * If parse-tree not found all are inactivated.
 */
int
cligen_ph_active_set_byname(cligen_handle h, 
			    char         *name)
{
    pt_head *ph;

    for (ph = cligen_pt_head_get(h); ph; ph = ph->ph_next)
	if (ph->ph_active)
	    break;
    if (ph != NULL)
	ph->ph_active = 0;
    if ((ph = cligen_ph_find(h, name)) != NULL)
	ph->ph_active = 1;
    return 0;
}

/*
 * CLIgen parse-tree workpoint example:
 * @code
 *  edit,wpset("t");{ 
 *    @working, wpset("t");
 *  }
 *  show, wpshow("t");
 *  up, wpup("t");
 *  top, wptop("t");
 *  treename="t";
 *  ...
 * @endcode
 */

/*! Callback: Working point tree set
 * Format of argv:
 *   <treename>
 */
int
cligen_wp_set(cligen_handle h,
	      cvec         *cvv,
	      cvec         *argv)
{
    cg_var   *cv;
    char     *treename;
    pt_head  *ph;  
    cg_obj   *co;

    cv = cvec_i(argv, 0);
    treename = cv_string_get(cv);
    if ((ph = cligen_ph_find(h, treename)) != NULL &&
	(co = cligen_co_match(h)) != NULL){
	if (co->co_treeref_orig != NULL)
	    cligen_ph_workpoint_set(ph, co->co_treeref_orig);
	else if (co->co_ref != NULL)
	    cligen_ph_workpoint_set(ph, co->co_ref);	    
    }
    return 0;
}

/*! Callback: Working point tree show
 * Format of argv:
 *   <treename>
 */
int
cligen_wp_show(cligen_handle h,
	       cvec         *cvv,
	       cvec         *argv)
{
    cg_var     *cv;
    char       *name;
    parse_tree *pt;
    cg_obj     *cow;    
    pt_head    *ph;

    cv = cvec_i(argv, 0);
    name = cv_string_get(cv);
    if ((ph = cligen_ph_find(h, name)) != NULL){
	if ((cow = cligen_ph_workpoint_get(ph)) != NULL)
	    pt = co_pt_get(cow);
	else
	    pt = cligen_ph_parsetree_get(ph);
	pt_print1(stderr, pt, 1);
    }
    return 0;
}

/*! Callback: Working point tree up to parent
 * Format of argv:
 *   <treename>
 */
int
cligen_wp_up(cligen_handle h,
	     cvec         *cvv,
	     cvec         *argv)
{
    cg_var  *cv;
    char    *treename;
    cg_obj  *co;
    pt_head *ph;  
    
    cv = cvec_i(argv, 0);
    treename = cv_string_get(cv);
    if ((ph = cligen_ph_find(h, treename)) != NULL &&
	(co = cligen_ph_workpoint_get(ph)) != NULL)
	cligen_ph_workpoint_set(ph, co_up(co));
    return 0;
}

/*! Callback: Working point tree reset to top level
 * Format of argv:
 *   <treename>
 */
int
cligen_wp_top(cligen_handle h,
	      cvec         *cvv,
	      cvec         *argv)
{
    cg_var  *cv;
    char    *treename;
    pt_head *ph;
    
    cv = cvec_i(argv, 0);
    treename = cv_string_get(cv);
    if ((ph = cligen_ph_find(h, treename)) != NULL)
	cligen_ph_workpoint_set(ph, NULL);
    return 0;
}

