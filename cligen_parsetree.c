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

  * Notes:
  * - The main prefix is "pt_" in this API, but a few have "cligen_parsetree_" pt_free 
  *    have both, consider changing to "cligen_pt_" ?
  * - There is some inconsistency wheen freeing sub-objects in this API.
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
#include "cligen_pt_head.h"
#include "cligen_callback.h"
#include "cligen_object.h"
#include "cligen_io.h"
#include "cligen_result.h"
#include "cligen_read.h"
#include "cligen_parse.h"
#include "cligen_handle.h"
#include "cligen_getline.h"

/* Private definition of parsetree. Public is defined in cligen_parsetree.h 
 * @see parse_tree_list which is the upper level of a parse-tree
 */
struct parse_tree{
    struct cg_obj     **pt_vec;    /* vector of pointers to parse-tree nodes */
    int                 pt_len;    /* length of vector */
#if 1 /* Would be nice to remove, but some functions use them */
    char               *pt_name;   /* Cache of ph_name */
#endif
    char                pt_set;    /* Parse-tree is a SET */ 
};

static int
pt_stats_one(parse_tree *pt,
	     size_t     *szp)
{
    size_t              sz = 0;

    sz += sizeof(struct parse_tree);
    sz += pt->pt_len*sizeof(struct cg_obj*);
    if (pt->pt_name)
	sz += strlen(pt->pt_name) + 1;
    if (szp)
	*szp = sz;
    return 0;
}

/*! Return statistics of a CLIgen objects of this parsetree recursively
 *
 * @param[in]   pt   Parsetree object
 * @param[out]  nrp  Number of CLIgen objects recursively
 * @param[out]  szp  Size of this pt + onjects recursively
 * @retval      0    OK
 * @retval     -1    Error
 */
int
pt_stats(parse_tree *pt,
	 uint64_t   *nrp,
	 size_t     *szp)
{
    cg_obj *co;
    size_t  sz = 0;
    int     i;
    
    *nrp += 1;
    pt_stats_one(pt, &sz);
    if (szp)
	*szp += sz;
    for (i=0; i<pt_len_get(pt); i++){
	if ((co = pt_vec_i_get(pt, i)) != NULL)	
	    co_stats(co, nrp, szp);
    }
    return 0;
}

/*! Access function to get the i:th CLIgen object child of a parse-tree
 * @param[in]  pt  Parse tree
 * @param[in]  i   Which object to return
 */
cg_obj *
pt_vec_i_get(parse_tree *pt,
	     int         i)
{
    struct cg_obj **ptvec;
    if (pt == NULL || i<0 || i>pt->pt_len){
       errno = EINVAL;
       return NULL;
    }
    if (pt->pt_vec != NULL){
	ptvec = pt->pt_vec;
	return ptvec[i];
    }
    else
	return NULL;
}

/*! Clear the i:th CLIgen object child of a parse-tree (without freeing existing)
 * @param[in]  pt  Parse tree
 * @param[in]  i   Which object to return
 */
int
pt_vec_i_clear(parse_tree *pt,
	       int         i)
{
    if (pt == NULL){
       errno = EINVAL;
       return -1;
    }
    if (i<0 || i >= pt_len_get(pt)){
       errno = EINVAL;
       return -1;
    }
    if (pt->pt_vec == NULL){
	errno = EFAULT;
	return -1;
    }
    pt->pt_vec[i] = NULL;
    return 0;
}

/*! Insert the i:th CLIgen object child of a parse-tree
 * @param[in]  pt  Parse tree
 * @param[in]  i   Which position to insert
 * @param[in]  co  Object to insert (can be NULL)
 */
int
pt_vec_i_insert(parse_tree *pt,
		int         i,
		cg_obj     *co)
{
    int       retval = -1;
    size_t    size;
    
    if (pt == NULL){
       errno = EINVAL;
       goto done;
    }
    if (i<0 || i > pt_len_get(pt)){
       errno = EINVAL;
       return -1;
    }
    if (pt_realloc(pt) < 0)
	goto done;
    if ((size = (pt_len_get(pt) - (i+1))*sizeof(cg_obj*)) != 0)
	memmove(&pt->pt_vec[i+1], 
		&pt->pt_vec[i], 
		size);
    pt->pt_vec[i] = co;
    retval = 0;
 done:
    return retval;
}

int
pt_vec_append(parse_tree *pt,
	      cg_obj     *co)
{
    return pt_vec_i_insert(pt, pt_len_get(pt), co);
}

/*! Remove and delete a single cligen object from a parsetree
 */
int
pt_vec_i_delete(parse_tree *pt,
		int         i,
    		int         recurse)
{
    int       retval = -1;
    size_t    size;
    cg_obj   *co;
    
    if (pt == NULL){
       errno = EINVAL;
       goto done;
    }
    if (i < 0 || i >= pt_len_get(pt)){
       errno = EINVAL;
       goto done;
    }
    if (pt->pt_vec == NULL){
	errno = EFAULT;
	goto done;
    }
    co = pt->pt_vec[i];
    pt->pt_vec[i] = NULL;
    co_free(co, recurse);
    if ((size = (pt_len_get(pt) - (i+1))*sizeof(cg_obj*)) != 0)
	memmove(&pt->pt_vec[i], 
		&pt->pt_vec[i+1], 
		size);
    pt->pt_len--;
    retval = 0;
 done:
    return retval;
}

/*! Access function to get a CLIgen objects child tree vector
 * @param[in]  co  CLIgen parse object
 * @see
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

#if 1 /* Would be nice to remove, but some functions use them */
char*
pt_name_get(parse_tree *pt)
{
    if (pt == NULL){
       errno = EINVAL;
       return NULL;
    }
    return pt->pt_name;
}

int
pt_name_set(parse_tree *pt,
	    char       *name)
{
    if (pt == NULL){
       errno = EINVAL;
       return -1;
    }
    if (pt->pt_name)
	free(pt->pt_name);
    if (name){
	if ((pt->pt_name = strdup(name)) == NULL)
	    return -1;
    }
    else
	pt->pt_name = NULL;
    return 0;
}
#endif

int
pt_sets_get(parse_tree *pt)
{
    if (pt == NULL){
       errno = EINVAL;
       return -1;
    }
    return pt->pt_set;
}
int
pt_sets_set(parse_tree *pt,
	    int         sets)
{
    if (pt == NULL){
       errno = EINVAL;
       return -1;
    }
    pt->pt_set = sets;
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
    if ((pt->pt_vec = realloc(pt->pt_vec, (pt->pt_len)*sizeof(cg_obj *))) == 0)
	return -1;
    pt->pt_vec[pt->pt_len - 1] = NULL; /* init field */
    return 0;
}

/*! Recursively copy a parse-tree.
 *
 * No common pointers between the two structures
 *
 * @param[in]  pt     Original parse-tree
 * @param[in]  parent The parent of the new parsetree. Need not be same as parent of the orignal
 * @param[in]  flags  Copy flags
 * @param[out] ptn    New parse-tree (need to be already created on entry)
 * @retval     0      OK
 * @retval    -1      Error
 * @see pt_dup
 */
int
pt_copy(parse_tree *pt, 
	cg_obj     *co_parent, 
	uint32_t    flags,
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
    pt_sets_set(ptn, pt_sets_get(pt));
    /* subtract tree-references, which are instances of other trees */
    for (i=0; i<pt_len_get(pt); i++){
	if ((co = pt_vec_i_get(pt,i)) && co_flags_get(co, CO_FLAGS_TREEREF))
	    ;
	else
	    ptn->pt_len++;
    }
    if (pt_len_get(ptn) &&
	(ptn->pt_vec = (cg_obj **)malloc(pt_len_get(ptn)*sizeof(cg_obj *))) == NULL){
	fprintf(stderr, "%s: malloc: %s\n", __FUNCTION__, strerror(errno));
	goto done;
    }
    j=0;
    for (i=0; i<pt_len_get(pt); i++){
	if ((co = pt_vec_i_get(pt, i)) != NULL){
	    if (!co_flags_get(co, CO_FLAGS_TREEREF))
		if (co_copy(co, co_parent, flags, &ptn->pt_vec[j++]) < 0)
		    goto done;
	}
	else
	    ptn->pt_vec[j++] = NULL;
    }
    retval = 0;
 done:
    return retval;
}

/*! Duplicate a  parse-tree recursively
 *
 * @param[in]  pt     Original parse-tree
 * @param[in]  flags  Copy flags
 * @param[in]  parent The parent of the new parsetree. Need not be same as parent of the orignal
 * @retval     ptnp   New parse-tree
 * @retval     NULL   Error
 */
parse_tree *
pt_dup(parse_tree *pt,
       cg_obj     *cop,
       uint32_t    flags)
{
    parse_tree *ptn = NULL;

    if (pt == NULL){
	errno = EINVAL;
	goto done;
    }
    if ((ptn = pt_new()) == NULL)
	goto done;
    if (pt_copy(pt, cop, flags, ptn) < 0){
	free(ptn);
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
	    if (co_copy(co1, parent, 0x0, &co1c) < 0)
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
	    if (pt1 && recursive){
		cligen_parsetree_sort(pt1, 1);
	    }
	    co_flags_reset(co, CO_FLAGS_MARK);
	}
    }
}

/*! Free all parse-tree nodes of the parse-tree, 
 * @param[in]  pt         CLIgen parse-tree
 * @param[in]  recursive  If 0 free pt and objects only, if 1 free recursive
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
    if (pt->pt_vec != NULL){
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
    free(pt);
    return 0;
}

int
cligen_parsetree_free(parse_tree *pt, 
		      int         recursive)
{
    return pt_free(pt, recursive);
}

/*! Trunc parse-tree to specific length
 *
 * Keep "len" objects, free the rest and reallocate the vector
 * @param[in]  pt         CLIgen parse-tree
 * @retval     0          OK
 * @retval    -1          Error
 */
int
pt_trunc(parse_tree *pt,
	 int         len)
{
    int     i;
    cg_obj *co;

    if (pt == NULL){
	errno = EINVAL;
	return -1;
    }
    if (len == 0 || len > pt->pt_len){
	errno = EINVAL;
	return -1;
    }
    if (len < pt->pt_len){
	for (i=len; i<pt_len_get(pt); i++){
	    if ((co = pt_vec_i_get(pt, i)) != NULL)
		co_free(co, 0);
	}
	if ((pt->pt_vec = realloc(pt->pt_vec, (len)*sizeof(cg_obj *))) == 0)
	    return -1;
	pt->pt_len = len;
    }
    return 0;
}

/*! Apply a function call recursively on all cg_obj:s in a parse-tree
 *
 * Recursively traverse all cg_obj in a parse-tree and apply fn(arg) for each
 * object found. The function is called with the cg_obj and an argument as args.
 * @param[in]  pt     CLIgen parse-tree
 * @param[in]  fn     Function to apply
 * @param[in]  depth  0: only this level, n : n levels
 * @param[in]  arg    Argument to function
 * @retval     0      OK (all applied function calls return 0)
 * @retval     -1     Error (one applied function call return -1)
 * @code
 *    parse_tree   *pt;
 *    if (pt_apply(pt, fn, INT32_MAX, (void*)42) < 0)
 *       err;
 * @endcode
 */
int
pt_apply(parse_tree   *pt, 
	 cg_applyfn_t  fn,
	 int           depth,
	 void         *arg)
{
    cg_obj *co;
    int     i;
    int     retval = -1;
    int     ret;

    if (pt->pt_vec == NULL)
	return 0;
    if (depth)
	for (i=0; i<pt_len_get(pt); i++){
	    if ((co = pt_vec_i_get(pt, i)) == NULL)
		continue;
	    if ((ret = fn(co, arg)) < 0)
		goto done;
	    if (ret == 1) /* done */
		break;
	    if (pt_apply(co_pt_get(co), fn, depth-1, arg) < 0)
		goto done;
	}
    retval = 0;
  done:
    return retval;
}

#if 0 /* DEBUG */
#include "cligen_expand.h"
int
pt_eq(cligen_handle h,
      cg_obj       *co0p,
      parse_tree   *pt0,
      cg_obj       *co1p,
      parse_tree   *pt1)
{
    int         eq = 0;
    cg_obj     *co0;
    cg_obj     *co1;
    int         i;
    parse_tree *pt0n = NULL;    /* Expanded */
    parse_tree *pt1n = NULL;    /* Expanded */
    
    if (pt0 == NULL && pt1 == NULL)
	return 0;
    if (pt0 == NULL || pt1 == NULL){
	fprintf(stderr, "pt is NULL\n");
	assert(0); /* pt is NULL */
	return 1;
    }
    if ((pt0n = pt_new()) == NULL)
	return -1;
    if ((pt1n = pt_new()) == NULL)
	return -1;
    if (pt_expand(h, co0p, pt0, NULL,
		  1, /* Include hidden commands */
		  0, /* VARS are not expanded, eg ? <tab> */
		  pt0n) < 0)      /* expansion */
	return -1;
    if (pt_expand(h, co1p, pt1, NULL,
		  1, /* Include hidden commands */
		  0, /* VARS are not expanded, eg ? <tab> */
		  pt1n) < 0)      /* expansion */
	return -1;
    if ((eq = (pt_len_get(pt0n) - pt_len_get(pt1n))) != 0){
	fprintf(stderr, "pt len diffs\n");
	assert(eq == 0); /* pt len diffs */
	return eq;
    }
    for (i=0; i<pt_len_get(pt0n); i++){
	co0 = pt_vec_i_get(pt0n, i);
	co1 = pt_vec_i_get(pt1n, i);
	if (co0 == NULL && co1 == NULL)
	    continue;
	if (co0 == NULL || co1 == NULL){
	    fprintf(stderr, "co is NULL\n");
	    eq = 1;
	    assert(eq == 0); /* co is NULL */
	    break;
	}
	/* Skip if treerefs */
	if (co0->co_type == CO_REFERENCE && co1->co_type == CO_REFERENCE)
	    ;
	else if ((eq = co_eq(co0, co1)) != 0){
	    fprintf(stderr, "co_eq fail %d\n", eq);
	    assert(eq == 0); /* co_eq fail */
	    break;
	}
	if ((eq = pt_eq(h, co0, co_pt_get(co0), co1, co_pt_get(co1))) != 0)
	    break;
    }
    if (pt0n && pt_expand_cleanup(h, pt0n) < 0)
	return -1;
    if (pt1n && pt_expand_cleanup(h, pt1n) < 0)
	return -1;
    return eq;
}
#endif /* NOTYET */
