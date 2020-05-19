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
#include <ctype.h>
#ifdef HAVE_STRVERSCMP
#define _GNU_SOURCE
#define __USE_GNU
#include <string.h>
#undef _GNU_SOURCE
#undef __USE_GNU
#else /* HAVE_STRVERSCMP */
#include <string.h>
#endif /* HAVE_STRVERSCMP */
#include <errno.h>
#include "cligen_buf.h"
#include "cligen_cv.h"
#include "cligen_cvec.h"
#include "cligen_parsetree.h"
#include "cligen_object.h"
#include "cligen_io.h"
#include "cligen_read.h"
#include "cligen_parse.h"
#include "cligen_handle.h"
#include "cligen_getline.h"

#if 0
struct parse_tree{
    struct cg_obj     **pt_vec;    /* vector of pointers to parse-tree nodes */
    int                 pt_len;    /* length of vector */
    char               *pt_name;
    char                pt_set;    /* Parse-tree is a SET */ 
};
#endif

/*! Access function to get a CLIgen objects child tree vector
 * @param[in]  co  CLIgen parse object
 */
co_vec_t
pt_vec_get(parse_tree *pt)
{
    if (pt == NULL){
       errno = EINVAL;
       return NULL;
    }
    return pt->pt_vec;
}

/*! Access function to get the i:th CLIgen object child of a parse-tree
 * @param[in]  pt  Parse tree
 * @param[in]  i   Which object to return
 */
cg_obj *
pt_vec_i_get(parse_tree *pt,
	     int         i)
{
    co_vec_t    cov;
	
    if (pt == NULL){
       errno = EINVAL;
       return NULL;
    }
    if ((cov = pt_vec_get(pt)) == NULL)
	return NULL;
    else
	return pt_vec_get(pt)[i];
}

/*! Access function to get a CLIgen objects child tree vector
 * @param[in]  co  CLIgen parse object
 */
int
pt_len_get(parse_tree *pt)
{
    if (pt == NULL){
       errno = EINVAL;
       return -1;
    }
    return pt->pt_len;
}

/*! Access function to set a CLIgen objects child tree vector
 * @param[in]  pt  CLIgen parse tree
 * @param[in]  ptv Vector of CLIgen objects
 */
int
pt_vec_set(parse_tree *pt,
	   co_vec_t    ptv)
{
    if (pt == NULL){
       errno = EINVAL;
       return -1;
    }
    pt->pt_vec = ptv;
    return 0;
}

/*! Allocate a new parsetree
 * @see pt_free
 */
parse_tree *
pt_new(void)
{
    parse_tree *pt = NULL;

    if ((pt = malloc(sizeof(parse_tree))) == NULL)
	return NULL;
    memset(pt, 0, sizeof(parse_tree));
    return pt;
}

/*! Enlarge the child-vector (pattern) of a parse-tree
 *
 * @param[in] pt  Cligen object vector
 * @retval    0   OK
 * @retval   -1   Error
 * Suppose we have a pattern pt, with lists of cg_obj's 1..4:
 * pt -> .-.-.-.
 *       | | | |
 *       v v v v
 *       1 2 3 4
 * and a new cg_obj 5,
 * then reallocate pt for another list of cg_objs and copy it into the structure:
 * pt -> .-.-.-.-.
 *       | | | | |
 *       v v v v v
 *       1 2 3 4 5
 */
int 
pt_realloc(parse_tree *pt)
{
    pt->pt_len++;
    /* Allocate larger cg_obj vector */
    if ((pt->pt_vec = realloc(pt->pt_vec, (pt->pt_len)*sizeof(cg_obj *))) == 0){
	fprintf(stderr, "%s: realloc", __FUNCTION__);
	return -1;
    }
    pt->pt_vec[pt->pt_len - 1] = NULL; /* init field */
    return 0;
}

/*! Recursively copy a parse-tree.
 *
 * No common pointers between the two structures
 *
 * @param[in]  pt     Original parse-tree
 * @param[in]  parent The parent of the new parsetree. Need not be same as parent of the orignal
 * @param[out] ptn    New parse-tree (need to be created on entry)
 * @retval     0      OK
 * @retval    -1      Error
 * @see pt_dup
 */
int
pt_copy(parse_tree *pt, 
	cg_obj     *parent, 
	parse_tree *ptn)
{
    int        retval = -1;
    int        i;
    int        j;
    cg_obj    *co;

    if (pt == NULL || ptn == NULL){
	errno = EINVAL;
	goto done;
    }
    /* subtract treereferences, which are instances of other trees */
    for (i=0; i<pt_len_get(pt); i++){
	if ((co = pt_vec_i_get(pt,i)) && co_flags_get(co, CO_FLAGS_TREEREF))
	    ;
	else
	    ptn->pt_len++;
    }
    if ((ptn->pt_vec = (cg_obj **)malloc(pt_len_get(ptn)*sizeof(cg_obj *))) == NULL){
	fprintf(stderr, "%s: malloc: %s\n", __FUNCTION__, strerror(errno));
	goto done;
    }
    j=0;
    for (i=0; i<pt_len_get(pt); i++){
	if ((co = pt_vec_i_get(pt, i)) != NULL){
	    if (!co_flags_get(co, CO_FLAGS_TREEREF))
		if (co_copy(co, parent, &ptn->pt_vec[j++]) < 0)
		    goto done;
	}
	else
	    ptn->pt_vec[j++] = NULL;
    }
    retval = 0;
 done:
    return retval;
}

/*! Duplicate a  parse-tree redursively
 *
 * @param[in]  pt     Original parse-tree
 * @param[in]  parent The parent of the new parsetree. Need not be same as parent of the orignal
 * @retval     ptnp   New parse-tree
 * @retval     NULL   Error
 */
parse_tree *
pt_dup(parse_tree *pt,
       cg_obj     *cop)
{
    parse_tree *ptn = NULL;

    if (pt == NULL){
	errno = EINVAL;
	goto done;
    }
    if ((ptn = pt_new()) == NULL)
	goto done;
    if (pt_copy(pt, cop, ptn) < 0){
	ptn = NULL;
	goto done;
    }
 done:
    return ptn;
}

/*! Recursively merge two parse-trees: pt1 into pt0
 * @param[in,out] pt0     parse-tree 0. On exit contains pt1 too
 * @param[in]     parent  Parent of pt0
 * @param[in]     pt1     parse-tree 1. Merge this into pt0
 * @retval        0       OK
 * @retval        -1      Error
 */
int
cligen_parsetree_merge(parse_tree *pt0, 
		       cg_obj     *parent, 
		       parse_tree *pt1)
{
    cg_obj *co0=NULL;
    cg_obj *co1;
    cg_obj *co1c;
    int     i;
    int     j;
    int     retval = -1;
    int     exist;

    for (j=0; j<pt_len_get(pt1); j++){ 
	co1 = pt_vec_i_get(pt1, j);
	exist = 0;
	for (i=0; i<pt_len_get(pt0); i++){
	    co0 = pt_vec_i_get(pt0, i);
	    if (co0 == NULL && co1 == NULL){
		exist = 1;
		break;
	    }
	    if (co0 && co1 && co_eq(co0, co1)==0){
		if (co0->co_callbacks == NULL && co1->co_callbacks != NULL){
		    /* Cornercase: co0 callback is NULL and co1 callback is not 
		     * Copy from co1 to co0
		     */
		    if (co_callback_copy(co1->co_callbacks, &co0->co_callbacks) < 0)
			goto done;
		}
		exist = 1;
		break;
	    }
	} 
	if (co1==NULL){ /* empty */
	    if (exist)
		continue;
	    if (pt_realloc(pt0) < 0)
		goto done;
	    pt0->pt_vec[pt_len_get(pt0)-1] = NULL;
	    continue;
	}
	if (exist){
	    if (cligen_parsetree_merge(co_pt_get(co0), co0, co_pt_get(co1)) < 0)
		goto done;
	}
	else{
	    if (pt_realloc(pt0) < 0)
		goto done;
	    if (co_copy(co1, parent, &co1c) < 0)
		goto done;
	    pt0->pt_vec[pt_len_get(pt0)-1] = co1c;
	}
    }
    cligen_parsetree_sort(pt0, 0);
    retval = 0;
  done:
    return retval;
}

/*! Help function to qsort for sorting entries in pattern file.
 * @param[in]  arg1
 * @param[in]  arg2
 * @retval  0  If equal
 * @retval <0  if arg1 is less than arg2
 * @retval >0  if arg1 is greater than arg2
 */
static int
co_cmp(const void* arg1, 
       const void* arg2)
{
    cg_obj* co1 = *(cg_obj**)arg1;
    cg_obj* co2 = *(cg_obj**)arg2;

    if (co1 == NULL){
	if (co2 == NULL)
	    return 0;
	else
	    return -1;
    }
    else if (co2 == NULL)
	return 1;
    else
	return co_eq(co1, co2);
}

/*! Sort CLIgen parse-tree, optionally recursive
 * @param[in]  The CLIgen parse-tree
 * @param[in]  recursive. If set sort recursive calls
 * @retval     void
 */
void 
cligen_parsetree_sort(parse_tree *pt, 
		      int         recursive)
{
    cg_obj     *co;
    int         i;
    parse_tree *pt1;
    
    qsort(pt->pt_vec, pt_len_get(pt), sizeof(cg_obj*), co_cmp);
    for (i=0; i<pt_len_get(pt); i++){
	if ((co = pt_vec_i_get(pt, i)) == NULL)
	    continue;
	if (co_flags_get(co, CO_FLAGS_MARK) == 0){ /* not recursive */
	    co_flags_set(co, CO_FLAGS_MARK);
	    pt1 = co_pt_get(co);
	    if (pt1 && pt_vec_get(pt1) && recursive){
		cligen_parsetree_sort(pt1, 1);
	    }
	    co_flags_reset(co, CO_FLAGS_MARK);
	}
    }
}

/*! Free all parse-tree nodes of the parse-tree, 
 * @param[in]  pt         CLIgen parse-tree
 * @param[in]  recursive  If set free recursive
 * @retval     0          OK
 * @retval    -1          Error
 */
int
pt_free(parse_tree *pt, 
	int         recursive)
{
    int     i;
    cg_obj *co;

    if (pt == NULL){
	errno = EINVAL;
	return -1;
    }
    if (pt->pt_vec != NULL){ // <-- read here
	if (recursive)
	    for (i=0; i<pt_len_get(pt); i++)
		if ((co = pt_vec_i_get(pt, i)) != NULL)
		    co_free(co, recursive);
	free(pt->pt_vec);
    }
    pt->pt_len = 0;
    if (pt->pt_name){
	free(pt->pt_name);
	pt->pt_name = NULL;
    }
    free(pt);// <-- free:d here
    return 0;
}
int
cligen_parsetree_free(parse_tree *pt, 
		      int         recursive)
{
    return pt_free(pt, recursive);
}

/*! Apply a function call recursively on all cg_obj:s in a parse-tree
 *
 * Recursively traverse all cg_obj in a parse-tree and apply fn(arg) for each
 * object found. The function is called with the cg_obj and an argument as args.
 * @param[in]  pt     CLIgen parse-tree
 * @param[in]  fn     Function to apply
 * @param[in]  arg    Argument to function
 * @retval     0      OK (all applied function calls return 0)
 * @retval     -1     Error (one applied function call return -1)
 */
int
pt_apply(parse_tree   *pt, 
	 cg_applyfn_t  fn, 
	 void         *arg)
{
    cg_obj *co;
    int     i;
    int     retval = -1;

    if (pt->pt_vec == NULL)
	return 0;
    for (i=0; i<pt_len_get(pt); i++){
	if ((co = pt_vec_i_get(pt, i)) == NULL)
	    continue;
	if (fn(co, arg) < 0)
	    goto done;
	if (pt_apply(co_pt_get(co), fn, arg) < 0)
	    goto done;
    }
    retval = 0;
  done:
    return retval;
}
