/*
  ***** BEGIN LICENSE BLOCK *****
 
  Copyright (C) 2001-2020 Olof Hagsand

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
#include "cligen_parsetree_head.h"
#include "cligen_object.h"
#include "cligen_io.h"
#include "cligen_handle.h"
#include "cligen_read.h"
#include "cligen_parse.h"
#include "cligen_history.h"
#include "cligen_getline.h"

/*
 * Access functions 
 */
char*
cligen_ph_name_get(parse_tree_head *ph)
{
    if (ph == NULL){
       errno = EINVAL;
       return NULL;
    }
    return ph->ph_name;
}

int
cligen_ph_name_set(parse_tree_head *ph,
	    char            *name)
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

parse_tree*
cligen_ph_parsetree_get(parse_tree_head *ph)
{
    if (ph == NULL){
       errno = EINVAL;
       return NULL;
    }
    return ph->ph_parsetree;
}

/*! Access function to the working point in a tree, shortcut to implement edit modes.
 * @param[in] h     CLIgen handle
 * @param[in] name  Name of tree
 * @retval    wp    Working point cligen object (shortcut - a sub of this tree)
 * @retval    NULL  No such tree/error
 */
cg_obj *
cligen_ph_workpoint_get(parse_tree_head *ph)
{
    return ph->ph_workpt;

}

/*! Access function to the working point in a tree, shortcut to implement edit modes.
 * @param[in]  pt   Parse tree
 * @param[in]  wp   Working point identified by a cligen object(actually its parse-tree sub vector)
 */
int
cligen_ph_workpoint_set(parse_tree_head *ph,
			cg_obj          *wp)
{
    ph->ph_workpt = wp;
    return 0;
}

/*! Find a parsetree head by its name,
 * @param[in] h       CLIgen handle
 * @param[in] name    Name of tree
 * @retval    ph      Parse-tree header
 * @retval    NULL    Not found
 * @note name of parse-tree is assigned when you do cligen_tree_add
 */
parse_tree_head *
cligen_ph_find(cligen_handle h,
	       char         *name)
{
    parse_tree_head      *ph = NULL;
    char                 *phname;
    
    for (ph = cligen_parsetree_head_get(h); ph; ph = ph->ph_next){
	if ((phname = cligen_ph_name_get(ph)) == NULL)
	    continue;
	if (strcmp(phname, name) == 0)
	    break;
    }
    return ph;
}

/*! Find a parsetree by its name, 
 * @param[in] h       CLIgen handle
 * @param[in] name    Name of tree
 * @retval    pt      Parse-tree
 * @retval    NULL    Not found
 * @note name of parse-tree is assigned when you do cligen_tree_add
 */
parse_tree *
cligen_tree_find(cligen_handle h, 
		 char         *name)
{
    parse_tree_head      *ph;

    if ((ph = cligen_ph_find(h, name)) != NULL)
	return ph->ph_parsetree;
    return NULL;
}

/*! Access function to the working point in a tree, shortcut to implement edit modes.
 * @param[in] h     CLIgen handle
 * @param[in] name  Name of tree
 * @retval    wp    Working point cligen object (shortcut - a sub of this tree)
 * @retval    NULL  No such tree/error
 */
parse_tree *
cligen_tree_workpt_pt(cligen_handle h,
		      char         *name)
{
    parse_tree_head *ph;
    cg_obj          *cow;

    if ((ph = cligen_ph_find(h, name)) != NULL){
	if ((cow = ph->ph_workpt) != NULL)
	    return co_pt_get(cow);
	else
	    return ph->ph_parsetree;
    }
    return NULL;
}

/*! Add a new parsetree last in list
 * @param[in] h       CLIgen handle
 * @param[in] name    name of parse-tree
 * @param[in] pt      parse-tree
 * @retval    0       OK
 * @retval   -1       Error
 * Note, if this is the first tree, it is activated by default
 */
int 
cligen_tree_add(cligen_handle h, 
		char         *name, 
		parse_tree   *pt)
{
    parse_tree_head           *ph;
    parse_tree_head           *phlast;

    if ((ph = (parse_tree_head *)malloc(sizeof(*ph))) == NULL){
	fprintf(stderr, "%s malloc: %s\n", __FUNCTION__, strerror(errno));
	return -1;
    }
    memset(ph, 0, sizeof(*ph));
    ph->ph_parsetree = pt;
    if (cligen_ph_name_set(ph, name) < 0) /* same as parse-tree-name */
	return -1;
    if (pt_name_set(pt, name) < 0) /* XXX Is this even necessary ? */
	return -1;
    if ((phlast = cligen_parsetree_head_get(h)) == NULL){
	ph->ph_active++;
	cligen_parsetree_head_set(h, ph);
    }
    else {
	while (phlast->ph_next)
	    phlast = phlast->ph_next;
	phlast->ph_next = ph;
    }
    return 0;
}

/*! Delete a parsetree list entry not parsetree itself
 * @param[in] h    CLIgen handle
 * @param[in] name 
 */
int 
cligen_tree_del(cligen_handle h, 
		char         *name)
{
    parse_tree_head  *ph;
    parse_tree       *pt;

    for (ph = cligen_parsetree_head_get(h); ph; ph = ph->ph_next){
	pt = ph->ph_parsetree;
	if (pt && strcmp(pt_name_get(pt), name) == 0){
	    cligen_parsetree_head_del(h, ph);
	    break;
	}
    }
    return 0;
}

/*! Iterate through all parsed cligen trees 
 *
 * @param[in] h   CLIgen handle
 * @param[in] pt  Cligen parse-tree iteration variable. Must be initialized to NULL
 * @retval pt     Next parse-tree structure.
 * @retval NULL   When end of list reached.
 * @code
 *    parse_tree_head *ph = NULL;
 *    while ((ph = cligen_ph_each(h, ph)) != NULL) {
 *	     ...
 *    }
 * @endcode
 * Note: you may not delete (or add) parse-trees while iterating through them
 * Note: the list contains all parse-trees added by cligen_tree_add()
 * XXX: suboptimal reimplement with linked
 */
parse_tree_head *
cligen_ph_each(cligen_handle    h, 
	       parse_tree_head *ph0)
{
    parse_tree_head *ph = NULL;
    
    if (ph0 == NULL)
	ph = cligen_parsetree_head_get(h);
    else
	ph = ph0->ph_next;
    return ph;
}

/*! Return i:th parse-tree of parsed cligen trees 
 * @param[in] h  CLIgen handle
 * @param[in] i  Order of element to get
 */
parse_tree_head *
cligen_ph_i(cligen_handle h, 
	    int           i0)
{
    parse_tree_head      *ph;
    int                   i;

    for (ph = cligen_parsetree_head_get(h), i=0; ph; ph = ph->ph_next, i++)
	if (i==i0)
	    return ph;
    return NULL;
}

/*! Get name of currently active parsetree.
 * @param[in] h       CLIgen handle
 */
parse_tree *
cligen_tree_active_get(cligen_handle h)
{
    parse_tree_head      *ph;

    for (ph = cligen_parsetree_head_get(h); ph; ph = ph->ph_next)
	if (ph->ph_active)
	    return ph->ph_parsetree;
    return NULL;
}

/*! Set currently active parsetree by name
 * @param[in] h       CLIgen handle
 * @retval    0
 * If parse-tree not found all are inactivated.
 */
int
cligen_tree_active_set(cligen_handle h, 
		       char         *name)
{
    parse_tree_head      *ph;

    for (ph = cligen_parsetree_head_get(h); ph; ph = ph->ph_next)
	if (ph->ph_active)
	    break;
    if (ph != NULL)
	ph->ph_active = 0;
    if ((ph = cligen_ph_find(h, name)) != NULL)
	ph->ph_active = 1;
    return 0;
}

