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
#include <stdint.h>
#include <stdarg.h>
#include <inttypes.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
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

/* Stats: nr of created cligen objects */
uint64_t _co_created = 0;
uint64_t _co_count = 0;

/*! Return number of created and existing cligen objects
 *
 * @param[out]  created  Number of created CLIgen objects (ever)
 * @param[out]  nr       Number of existing CLIgen objects (created - freed)
 */
int
co_stats_global(uint64_t *created,
		uint64_t *nr)
{
    *created = _co_created;
    *nr =   _co_count;
    return 0;
}

/*! Return the alloced memory of a single CLIgen object
 * @param[in]   y    YANG object
 * @param[out]  szp  Size of this YANG obj
 * @retval      0    OK
 * (baseline: )
 */
static int
co_stats_one(cg_obj   *co,
	     size_t   *szp)
{
    size_t              sz = 0;
    struct cg_callback *cc;
    struct cg_varspec  *cgs;
    
    sz += sizeof(struct cg_obj);
    sz += co->co_pt_len*sizeof(struct parse_tree*);
    if (co->co_command)
	sz += strlen(co->co_command) + 1;
    if (co->co_prefix)
	sz += strlen(co->co_prefix) + 1;
    for (cc = co->co_callbacks; cc; cc=cc->cc_next)
	sz += co_callback_size(cc);
    if (co->co_cvec)
	sz += cvec_size(co->co_cvec);
    if (co->co_filter)
	sz += cvec_size(co->co_filter);
    if (co->co_helpstring)
	sz += strlen(co->co_helpstring) + 1;
    if (co->co_value)
	sz += strlen(co->co_value) + 1;
    /* XXX union */
    if (co->co_type == CO_VARIABLE){
	cgs = &co->u.cou_var;
	if (cgs->cgs_show)
	    sz += strlen(cgs->cgs_show) + 1;
	if (cgs->cgs_expand_fn_str)
	    sz += strlen(cgs->cgs_expand_fn_str) + 1;
	if (cgs->cgs_expand_fn_vec)
	    sz += cvec_size(cgs->cgs_expand_fn_vec);
	if (cgs->cgs_translate_fn_str)
	    sz += strlen(cgs->cgs_translate_fn_str) + 1;
	if (cgs->cgs_choice)
	    sz += strlen(cgs->cgs_choice) + 1;
	if (cgs->cgs_rangecvv_low)
	    sz += cvec_size(cgs->cgs_rangecvv_low);
    	if (cgs->cgs_rangecvv_upp)
	    sz += cvec_size(cgs->cgs_rangecvv_upp);
	if (cgs->cgs_regex)
	    sz += cvec_size(cgs->cgs_regex);
    }
    if (szp)
	*szp = sz;
    return 0;
}

/*! Return statistics of a CLIgen object recursively
 * @param[in]   co   CLIgen object
 * @param[out]  szp  Size of this co recursively
 * @retval      0    OK
 * @retval     -1    Error
 */
int
co_stats(cg_obj   *co,
	 uint64_t *nrp,
	 size_t   *szp)
{
    int         retval = -1;
    size_t      sz = 0;
    parse_tree *pt;
    int         i;
    
    if (co == NULL){
	errno = EINVAL;
	goto done;
    }
    *nrp += 1;
    co_stats_one(co, &sz);
    if (szp)
	*szp += sz;
    for (i=0; i<co->co_pt_len; i++){
	if ((pt = co->co_ptvec[i]) != NULL){
	    pt_stats(pt, nrp, szp);
	}
    }
    retval = 0;
 done:
    return retval;
}

/* Access macro */
cg_obj* 
co_up(cg_obj *co) 
{
    return co->co_prev;
}

int
co_up_set(cg_obj *co,
	  cg_obj *cop) 
{
    co->co_prev = cop;
    return 0;
}

/*! return top-of-tree (ancestor) */
cg_obj* 
co_top(cg_obj *co0) 
{
    cg_obj *co = co0;
    cg_obj *co1;

    while ((co1 = co_up(co)) != NULL)
	co = co1;
    return co;
}

static int 
co_pt_realloc(cg_obj *co)
{
    int retval = -1;
    if (co->co_pt_len == 0){
	co->co_pt_len++;
	//	if ((co->co_ptvec = realloc(co->co_ptvec, (co->co_pt_len)*sizeof(parse_tree *))) == 0)
	if ((co->co_ptvec = calloc(co->co_pt_len, sizeof(parse_tree *))) == 0)
	    goto done;
    }
    retval = 0;
 done:
    return retval;
}

/*! Access function to get a CLIgen objects parse-tree head
 * @param[in]  co  CLIgen parse object
 * @retval     pt   parse-tree
 * @retval     NULL Error or no such parsetree
 */
parse_tree *
co_pt_get(cg_obj *co)
{
    if (co == NULL){
       errno = EINVAL;
       return NULL;
    }
    return co->co_ptvec?co->co_ptvec[0]:NULL;
}

/*! Set a CLIgen objects parse-tree head
 * @param[in]  co  CLIgen parse object
 * @param[in]  pt  CLIgen parse tree
 * @retval     0   OK
 * @retval    -1   Error
 */
int
co_pt_set(cg_obj     *co,
	  parse_tree *pt)
{
    if (co == NULL){
       errno = EINVAL;
       return -1;
    }
    if (co->co_pt_len == 0){
	if (co_pt_realloc(co) < 0)
	    return -1;
    }
    else {
	if (co->co_ptvec[0])
	    pt_free(co->co_ptvec[0], 1);
    }
    co->co_ptvec[0] = pt; 
    return 0;
}

/*! Clear a CLIgen objects parse-tree head (dont free old)
 * @param[in]  co  CLIgen parse object
 * @param[in]  pt  CLIgen parse tree
 * @retval     0   OK
 * @retval    -1   Error
 * @see co_pt_set
 */
int
co_pt_clear(cg_obj *co)
{
    if (co == NULL){
       errno = EINVAL;
       return -1;
    }
    if (co->co_pt_len == 0){
	if (co_pt_realloc(co) < 0)
	    return -1;
    }
    co->co_ptvec[0] = NULL; 
    return 0;
}

void
co_flags_set(cg_obj  *co,
	     uint32_t flag)
{
    co->co_flags |= flag;
}

void
co_flags_reset(cg_obj  *co,
	       uint32_t flag)
{
    co->co_flags &= ~flag;
}

int
co_flags_get(cg_obj  *co,
	     uint32_t flag)
{
    return (co->co_flags & flag) ? 1 : 0;
}

int
co_sets_get(cg_obj *co)
{
    parse_tree *pt;

    if ((pt = co_pt_get(co)) != NULL)
	return pt_sets_get(pt);
    else
	return 0;
}

void
co_sets_set(cg_obj *co,
	    int     sets)

{
    parse_tree *pt;
    if ((pt = co_pt_get(co)) != NULL)
	pt_sets_set(pt, sets);
}

char*
co_prefix_get(cg_obj *co)
{
    return co->co_prefix;
}

int
co_prefix_set(cg_obj *co,
	      char   *prefix)
{
    if (co->co_prefix != NULL){
	free(co->co_prefix);
	co->co_prefix = NULL;
    }
    if (prefix &&
	(co->co_prefix = strdup(prefix)) == NULL)
	return -1;
    return 0;
}

/*! Assign a preference to a cligen variable object
 * Prefer more specific commands/variables  if you have to choose from several. 
 * @param[in] co   Cligen obe
 * @retval    pref Preference: positive integer
 * The preference is (higher more preferred):
 * command / keyword
 * (ip|mac) 
 * decimal64
 * int8 with range , uint8 with range 
 * ...
 * int64 with range , uint64 with range 
 * int8 , uint8
 * ...
 * int64 , uint64
 * interface
 * regexp
 * string 
 * rest
 * Note in a choice: <int32>|<uint16>, uint16 is preferred.
 * XXX: It does not cover: <int32 range:0-12>|<int32 range:6-18>
 */
static int
cov_pref(cg_obj *co)
{
    int pref = 0;

    switch (co->co_vtype){
    case CGV_ERR:
	pref = 0; /* Illegal */
	break;
	/* ints in range 22-60 */
    case CGV_INT8:
	if (co->co_rangelen)
	    pref = 60;
	else
	    pref = 52;
	break;
    case CGV_INT16:
	if (co->co_rangelen)
	    pref = 58;
	else
	    pref = 50;
	break;
    case CGV_INT32:
	if (co->co_rangelen)
	    pref = 56;
	else
	    pref = 48;
	break;
    case CGV_INT64:
	if (co->co_rangelen)
	    pref = 54;
	else
	    pref = 46;
	break;
    case CGV_UINT8:
	if (co->co_rangelen)
	    pref = 59;
	else
	    pref = 51;
	break;
    case CGV_UINT16:
	if (co->co_rangelen)
	    pref = 57;
	else
	    pref = 49;
	break;
    case CGV_UINT32:
	if (co->co_rangelen)
	    pref = 55;
	else
	    pref = 47;
	break;
    case CGV_UINT64:
	if (co->co_rangelen)
	    pref = 53;
	else
	    pref = 45;
	break;
    case CGV_DEC64:
	pref = 62;
	break;
    case CGV_BOOL:
	pref = 12;
	break;
    case CGV_REST:
	pref = 1;
	break;
    case CGV_STRING:
	if (co->co_expand_fn_str != NULL)
	    pref = 8;
	else if (co->co_regex)
	    pref = 7;
	else
	    pref = 5;
	break;
    case CGV_INTERFACE:
	pref = 10;
	break;
    case CGV_IPV4ADDR:
    case CGV_IPV4PFX:
	pref = 70;
	break;
    case CGV_IPV6ADDR:
    case CGV_IPV6PFX:
	pref = 71;
	break;
    case CGV_MACADDR:
	pref = 72;
	break;
    case CGV_URL:
	pref = 20;
	break;
    case CGV_UUID:
	pref = 73;
	break;
    case CGV_TIME:
	pref = 74;
	break;
    case CGV_VOID: /* N/A */
	break;
    case CGV_EMPTY: 
	break;
    }

    return pref;
}

/*! Assign a preference to a cligen object
 *
 * @param[in]  co    cligen_object
 * @param[in]  exact if this match is exact (only applies to CO_COMMAND)
 * @retval     pref  Preference: positive integer
 *
 * The higher the better
 * if you have to choose from several.
 * The preference is:
 * command > ip|mac > int > interface > string > expand > rest
 * 'expand' is a command with not exact match that is derived from a <expand> or <choice>
 */
int
co_pref(cg_obj *co, 
	int     exact)
{
    int pref = 0;;

    if (co->co_preference > 0)
	pref = co->co_preference;
    else
	switch (co->co_type){
	case CO_COMMAND:
	    /* Give full preference to exact command match, low to partial (prefix) command match */
	    if (exact == 0)
		pref = 3;
	    else
		pref = 100;
	    break;
	case CO_VARIABLE:
	    pref = cov_pref(co);
	    break;
	case CO_REFERENCE:
	case CO_EMPTY:
	    break;
	}
    return pref;
}

/*! Just malloc a CLIgen object. No other allocations allowed */
cg_obj *
co_new_only()
{
    cg_obj *co;

    if ((co = malloc(sizeof(cg_obj))) == NULL)
	return NULL;
    memset(co, 0, sizeof(cg_obj));
    _co_count++;
    _co_created++;
    return co;
}

/*! Create new cligen parse-tree command object
 *
 * That is, a cligen parse-tree object with type == CO_COMMAND (not variable)
 * @param[in]  cmd   Initial command value
 * @param[in]  prev  parent object (or NULL)
 * @retval     NULL  Error
 * @retval     co    Created cligen object. Free with co_free()
 * @see cov_new
 * @see co_free
 */
cg_obj *
co_new(char   *cmd, 
       cg_obj *parent)
{
    cg_obj     *co;
    parse_tree *pt;

    if ((co = co_new_only()) == NULL)
	return NULL;
    co->co_type    = CO_COMMAND;
    if (cmd)
	co->co_command = strdup(cmd);
    co_up_set(co, parent);
    /* parse-tree created implicitly */
    if ((pt = pt_new()) == NULL){
	free(co);
	return NULL;
    }
    if (co_pt_set(co, pt) < 0){
	free(pt);
	free(co);
	return NULL;
    }
    return co;
}

/*! Create new cligen parse-tree variable object
 *
 * That is, a cligen parse-tree object with type == CO_VARIABLE
 * @param[in]  cvtype  Cligen variable type
 * @param[in]  parent  parent object (or NULL)
 * @retval     NULL    Error
 * @retval     co      Created cligen object. Free with co_free()
 * @see co_new
 * @see co_free
 */
cg_obj *
cov_new(enum cv_type cvtype, 
	cg_obj      *parent)
{
    cg_obj     *co;
    parse_tree *pt;

    if ((co = co_new_only()) == NULL)
	return NULL;
    co->co_type    = CO_VARIABLE;
    co->co_vtype   = cvtype;
    if (parent)
	co_up_set(co, parent);
    co->co_dec64_n = CGV_DEC64_N_DEFAULT;
    /* parse-tree created implicitly */
    if ((pt = pt_new()) == NULL){
	free(co);
	return NULL;
    }
    if (co_pt_set(co, pt) < 0){
	free(pt);
	free(co);
	return NULL;
    }
    return co;
}

/*! Recursively copy a cligen object.
 *
 * @param[in]  co     The object to copy from
 * @param[in]  parent The parent of the new object, need not be same as parent of co
 * @param[in]  flags  Copy flagst
 * @param[out] conp   Pointer to the object to copy to (is allocated)
 * @retval     0      OK
 * @retval     -1     Error
 * @see co_expand_sub
 * @see co_copy1  For non-recursive
 */
int
co_copy(cg_obj  *co, 
	cg_obj  *parent,
	uint32_t flags,
	cg_obj **conp)
{
    int         retval = -1;
    cg_obj     *con = NULL;
    parse_tree *pt;
    parse_tree *ptn;

    if ((con = co_new_only()) == NULL)
	goto done;
    memcpy(con, co, sizeof(cg_obj));
    con->co_ptvec = NULL;
    con->co_pt_len = 0;
    con->co_ref = NULL;
    /* If called from pt_expand_treeref: the copy (of a tree instance) points to the original tree
     */
    if (flags & CO_COPY_FLAGS_TREEREF)
	con->co_treeref_orig = co;
    co_flags_reset(con, CO_FLAGS_MARK);
    /* Replace all pointers */
    co_up_set(con, parent);
    if (co->co_command)
	if ((con->co_command = strdup(co->co_command)) == NULL)
	    goto done;
    if (co->co_prefix)
	if ((con->co_prefix = strdup(co->co_prefix)) == NULL)
	    goto done;
    if (co_callback_copy(co->co_callbacks, &con->co_callbacks) < 0)
	goto done;
    if (co->co_cvec)
	con->co_cvec = cvec_dup(co->co_cvec);
    if (co->co_filter)
	con->co_filter = cvec_dup(co->co_filter);
    if ((pt = co_pt_get(co)) != NULL){
	if ((ptn = pt_dup(pt, con, flags)) == NULL) /* sets a new pt under con */
	    goto done;
	if (co_pt_set(con, ptn) < 0)
	    goto done;
    }
    if (co->co_helpstring)
	if ((con->co_helpstring = strdup(co->co_helpstring)) == NULL)
	    goto done;
    con->co_value = NULL;
    if (co_value_set(con, co->co_value) < 0)
	goto done;
    if (co->co_type == CO_VARIABLE){
	if (co->co_expand_fn_str)
	    if ((con->co_expand_fn_str = strdup(co->co_expand_fn_str)) == NULL)
		goto done;
	if (co->co_translate_fn_str)
	    if ((con->co_translate_fn_str = strdup(co->co_translate_fn_str)) == NULL)
		goto done;
	if (co->co_show)
	    if ((con->co_show = strdup(co->co_show)) == NULL)
		goto done;
	if (co->co_rangecvv_low)
	    if ((con->co_rangecvv_low = cvec_dup(co->co_rangecvv_low)) == NULL)
		goto done;
	if (co->co_rangecvv_upp)
	    if ((con->co_rangecvv_upp = cvec_dup(co->co_rangecvv_upp)) == NULL)
		goto done;
	if (co->co_expand_fn_vec)
	    if ((con->co_expand_fn_vec = cvec_dup(co->co_expand_fn_vec)) == NULL)
		goto done;
	if (co->co_choice){
	    if ((con->co_choice = strdup(co->co_choice)) == NULL)
		goto done;
	}
	if (co->co_regex){
	    if ((con->co_regex = cvec_dup(co->co_regex)) == NULL)
		goto done;
	}
    } /* VARIABLE */
    *conp = con;
    retval = 0;
 done:
    return retval;
}

/*! Copy a single cligen object, non-recursively
 *
 * CLIgen objects are sometimes created (eg expand) and point into an existing parse-tree
 * structure. To free such a co, the regular co_copy cannot be used since it would
 * free a pt structure still being use by other co:s.
 * working
 * @param[in]  co     The object to copy from
 * @param[in]  parent The parent of the new object, need not be same as parent of co
 * @param[in]  recursive  If set copy recursive, otherwise, only first level object
 * @param[in]  flags  Copy flags
 * @param[out] conp   Pointer to the object to copy to (is allocated)
 * @retval     0      OK
 * @retval     -1     Error
 * @see co_copy   recursive copy: If recurse is 1, this function is equivalent
 * @note mark & refdone flags are cleared
 * @note co_ref is cleared in the recursive case. (This seems ad-hoc)
 */
int
co_copy1(cg_obj  *co, 
	 cg_obj  *parent,
	 int      recursive,
	 uint32_t flags,
	 cg_obj **conp)
{
    int         retval = -1;
    cg_obj     *con = NULL;
    parse_tree *pt;
    parse_tree *ptn;

    if ((con = co_new_only()) == NULL)
	goto done;
    memcpy(con, co, sizeof(cg_obj));
    con->co_ptvec = NULL;
    con->co_pt_len = 0;

    /* If called from pt_expand_treeref: the copy (of a tree instance) points to the original tree
     */
    if (flags & CO_COPY_FLAGS_TREEREF)
	con->co_treeref_orig = co;
    co_flags_reset(con, CO_FLAGS_MARK);
    /* Replace all pointers */
    co_up_set(con, parent);
    if (co->co_command)
	if ((con->co_command = strdup(co->co_command)) == NULL)
	    goto done;
    if (co->co_prefix)
	if ((con->co_prefix = strdup(co->co_prefix)) == NULL)
	    goto done;
    if (co_callback_copy(co->co_callbacks, &con->co_callbacks) < 0)
	goto done;
    if (co->co_cvec)
	con->co_cvec = cvec_dup(co->co_cvec);
    if (co->co_filter)
	con->co_filter = cvec_dup(co->co_filter);
    if ((pt = co_pt_get(co)) != NULL){
	/* Here this function differs from co_copy */
	if (recursive){
	    if ((ptn = pt_dup(pt, con, 0x0)) == NULL) /* sets a new pt under con */
		goto done;
	    if (co_pt_set(con, ptn) < 0)
		goto done;
	}
	else {
	    if (co_pt_set(con, pt) < 0)
		goto done;
	}
    }
    if (co->co_helpstring)
	if ((con->co_helpstring = strdup(co->co_helpstring)) == NULL)
	    goto done;
    con->co_value = NULL;
    if (co_value_set(con, co->co_value) < 0)
	goto done;
    if (co->co_type == CO_VARIABLE){
	if (co->co_expand_fn_str)
	    if ((con->co_expand_fn_str = strdup(co->co_expand_fn_str)) == NULL)
		goto done;
	if (co->co_translate_fn_str)
	    if ((con->co_translate_fn_str = strdup(co->co_translate_fn_str)) == NULL)
		goto done;
	if (co->co_show)
	    if ((con->co_show = strdup(co->co_show)) == NULL)
		goto done;
	if (co->co_rangecvv_low)
	    if ((con->co_rangecvv_low = cvec_dup(co->co_rangecvv_low)) == NULL)
		goto done;
	if (co->co_rangecvv_upp)
	    if ((con->co_rangecvv_upp = cvec_dup(co->co_rangecvv_upp)) == NULL)
		goto done;
	if (co->co_expand_fn_vec)
	    if ((con->co_expand_fn_vec = cvec_dup(co->co_expand_fn_vec)) == NULL)
		goto done;
	if (co->co_choice){
	    if ((con->co_choice = strdup(co->co_choice)) == NULL)
		goto done;
	}
	if (co->co_regex){
	    if ((con->co_regex = cvec_dup(co->co_regex)) == NULL)
		goto done;
	}
    } /* VARIABLE */
    *conp = con;
    retval = 0;
 done:
    return retval;
}

/*! Compare two strings, extends strcmp 
 * Basically strcmp but there are some complexities which one may enable.
 * Also handles NULL (NULL < all strings)
 * @param[in]  s1
 * @param[in]  s2
 * @retval  0  equal
 * @retval <0  str1 is less than str2
 * @retval >0  str1 is greater than str2
 *
 * strcmp orders:  1 10 2
 * wheras strverscmp orders: 1 2 10
 * but:
 * strcmp orders:  1b 16 6b  
 * wheras strverscmp orders: 1b 6b 16
 * If we use strverscmp we also must use it in e.g. complete
 */
static inline int
str_cmp(char *s1, 
	char *s2)
{
    if (s1 == NULL && s2 == NULL) 
	return 0;
    if (s1 == NULL) /* empty string first */
	return -1;
    if (s2 == NULL)
	return 1;
    /*
     * XXX: the cligen handler code uses NULL here which is wrong, but those
     * options are for now global settings.
     */
#ifdef  HAVE_STRVERSCMP
    if (cligen_lexicalorder(NULL))
	return strverscmp(s1, s2); /* can't combine lexicalorder and ignorecase */
    else
	return cligen_ignorecase(NULL) ? strcasecmp(s1, s2) : strcmp(s1, s2);
#else /* HAVE_STRVERSCMP */
    return cligen_ignorecase(NULL) ? strcasecmp(s1, s2) : strcmp(s1, s2);
#endif /* HAVE_STRVERSCMP */
}

/*! Check if two cligen objects (cg_obj) are equal
 *
 * Two cligen objects are equal if they have:
 * - same type (variable or command)
 * - if command, they should have same command name
 * - if they are a variable, they should also have:
 *          + same variable type.
 *          + same expand, choice, range and regexp options. 
 * @param[in]  co1
 * @param[in]  co2
 * @retval  0  If equal
 * @retval <0  if co1 is less than co2
 * @retval >0  if co1 is greater than co2
 * @see str_cmp
 * XXX co_prefix is not examined
 */
int 
co_eq(cg_obj *co1,
      cg_obj *co2)
{
    int eq;

    /* eq == 0 means equal */
    eq = !(co1->co_type == co2->co_type);
    if (eq){ /* Unequal type of command, variable and reference.
		but need to check special case, 
		if the variable is a KEYWORD, then it can be eq to a command. */
	/* Let References be last (more than) everything else */
	if (co1->co_type == CO_REFERENCE){
	    eq = 1;
	    goto done;
	}
	if (co2->co_type == CO_REFERENCE){
	    eq = -1;
	    goto done;
	}
	/* EMPTY shoyuld always be first */
	if (co1->co_type == CO_EMPTY){
	    eq = -1;
	    goto done;
	}   
	if (co2->co_type == CO_EMPTY){
	    eq = 1;
	    goto done;
	}   
	/* Here one is command and one is variable */
	eq = strcmp(co1->co_command, co2->co_command);
	goto done;
    }
    switch (co1->co_type){
    case CO_COMMAND:
    case CO_REFERENCE:
	eq = str_cmp(co1->co_command, co2->co_command);
	break;
    case CO_VARIABLE:
	eq = (co1->co_vtype == co2->co_vtype)?0:(co1->co_vtype < co2->co_vtype)?-1:1;
	/* Same variable type */
	if (eq != 0)
	    goto done;
	/* Examine expand: at least one set, and then strcmp */
	if (co1->co_expand_fn_str!=NULL || co2->co_expand_fn_str!=NULL){
	    eq = str_cmp(co1->co_expand_fn_str, co2->co_expand_fn_str);
	    goto done;
	}
	/* Should we examine co_translate_fn_str? */
	/* Examine choice: at least one set, and then strcmp */
	if (co1->co_choice!=NULL || co2->co_choice!=NULL){
	    eq = str_cmp(co1->co_choice, co2->co_choice);
	    goto done;
	}
	/* Examine regexp, at least one set, and then strcmp */
	if (co1->co_regex!=NULL || co2->co_regex!=NULL){
	    cg_var *cv1, *cv2;
	    if (co1->co_regex == NULL)
		eq = -1;
	    else if (co2->co_regex == NULL)
		eq = 1;
	    else{
		int i, min;
		min = cvec_len(co1->co_regex)<cvec_len(co2->co_regex)?cvec_len(co1->co_regex):cvec_len(co2->co_regex);
		for (i=0; i<min; i++){
		    cv1 = cvec_i(co1->co_regex, i);
		    cv2 = cvec_i(co2->co_regex, i);
		    if ((eq = str_cmp(cv_string_get(cv1), cv_string_get(cv2))) != 0)
			goto done;
		}
		if (cvec_len(co1->co_regex) < cvec_len(co2->co_regex))
		    eq = -1;
		else if (cvec_len(co1->co_regex) > cvec_len(co2->co_regex))
		    eq = 1;
		else
		    eq = 0;
	    }
	    if (eq)
		goto done;
	}
	/* Examine int and range */
	if (cv_isint(co1->co_vtype) || cv_isstring(co1->co_vtype)) {
	    int i;
	    cg_var *cv1, *cv2;
	    if ((eq = co1->co_rangelen - co2->co_rangelen) != 0)
		goto done;
	    /* either both 0 or both same length */
	    for (i=0; i<co1->co_rangelen; i++){
		cv1 = cvec_i(co1->co_rangecvv_low, i);
		cv2 = cvec_i(co2->co_rangecvv_low, i);
		if ((eq = cv_cmp(cv1, cv2)) != 0)
			goto done;
		cv1 = cvec_i(co1->co_rangecvv_upp, i);
		cv2 = cvec_i(co2->co_rangecvv_upp, i);
		if ((eq = cv_cmp(cv1, cv2)) != 0)
			goto done;
	    }
	} /* range */
	break;
    case CO_EMPTY:
	eq = 0;
	break;
    }
  done:
    return eq;
}

/*! Free an individual syntax node (cg_obj).
 * @param[in]  co         CLIgen object
 * @param[in]  recursive  If set free recursive, if 0 free only cligen object, and parsetree
 * @retval     0          OK
 * @retval    -1          Error
 * Note that the co_var pointer is not freed. The application
 * needs to handle it (dont use a pointer to the stack for example).
 * Note: if you add a free here, you should probably add something in
 * co_copy and co_expand_sub
 */
int 
co_free(cg_obj *co, 
	int     recursive)
{
    parse_tree  *pt;

    if (co->co_helpstring) 
	free(co->co_helpstring);
    if (co->co_command)
	free(co->co_command);
    if (co->co_prefix)
	free(co->co_prefix);
    if (co->co_value)
	free(co->co_value);
    if (co->co_cvec)
	cvec_free(co->co_cvec);
    if (co->co_filter)
	cvec_free(co->co_filter);
    if (co->co_callbacks)
	co_callbacks_free(&co->co_callbacks);
    if (co->co_type == CO_VARIABLE){
	if (co->co_expand_fn_str)
	    free(co->co_expand_fn_str);
	if (co->co_translate_fn_str)
	    free(co->co_translate_fn_str);
	if (co->co_show)
	    free(co->co_show);
	if (co->co_expand_fn_vec)
	    cvec_free(co->co_expand_fn_vec);
	if (co->co_choice)
	    free(co->co_choice);
	if (co->co_regex)
	    cvec_free(co->co_regex);
	if (co->co_rangecvv_low)
	    cvec_free(co->co_rangecvv_low);
	if (co->co_rangecvv_upp)
	    cvec_free(co->co_rangecvv_upp);
    }
    if (recursive && (pt = co_pt_get(co)) != NULL){ 
	pt_free(pt, 1); /* recursive */ 
    }
    if (co->co_ptvec != NULL)
	free(co->co_ptvec);
    free(co);
    _co_count--;
    return 0;
}

/*! Look for a CLIgen object in a (one-level) parse-tree in interval [low,high]
 * @param[in]  pt      CLIgen parse-tree
 * @param[in]  name    Name of node
 * @param[in]  low     Lower bound
 * @param[in]  upper   Upper bound
 * @retval     co      Object found
 * @retval     NULL    Not found
 * @see co_insert Main function
 */
static cg_obj *
co_search1(parse_tree *pt, 
	   char       *name, 
	   int         low, 
	   int         upper)
{
    int     mid;
    int     cmp;
    cg_obj *co;

    if (upper < low)
	return NULL; /* not found */
    mid = (low + upper) / 2;
    if (mid >= pt_len_get(pt))  /* beyond range */
	return NULL;
    co = pt_vec_i_get(pt, mid);
    cmp = str_cmp(name, co ? co->co_command : NULL);
    if (cmp < 0)
	return co_search1(pt, name, low, mid-1);
    else if (cmp > 0)
	return co_search1(pt, name, mid+1, upper);
    else
	return co;
}

/*! Position where to insert cligen object into a parse-tree list alphabetically
 * at what position to insert co1. Insert after this position
 * @param[in]  pt        CLIgen parse-tree
 * @param[in]  co1       CLIgen object to insert
 * @param[in]  low       Lower bound
 * @param[in]  upper     Upper bound (+1)
 * @retval     position 
 * @see co_insert Main function
 */
static int
co_insert_pos(parse_tree *pt, 
	      cg_obj     *co1, 
	      int         low, 
	      int         upper)
{
    int     mid;
    int     cmp;
    cg_obj *co2; /* variable for objects in list */

    if (upper < low)
	return low; /* not found */
    mid = (low + upper) / 2;
    if (mid >= pt_len_get(pt))
	return pt_len_get(pt); 
    if (co1 == NULL)
	return 0; /* Insert in 1st pos */
    co2 = pt_vec_i_get(pt, mid);
    if (co2 == NULL)
	cmp = 1;
    else
	cmp = co_eq(co1, co2); /* -1 if co1 < co2,.. */
    if (cmp < 0)
	return co_insert_pos(pt, co1, low, mid-1);
    else if (cmp > 0)
	return co_insert_pos(pt, co1, mid+1, upper);
    else
	return mid;
}

/*! Add a cligen object (co1) to a parsetree(pt) alphabetically.
 * This involves searching in the parsetree for the position where it should be added,
 * Then checking whether an equivalent version already exists.
 * Then modifying the parsetree by shifting it down, and adding the new object.
 * There is som complexity if co == NULL.
 * @param[in] pt   Parse-tree
 * @param[in] co1  CLIgen object
 * @param[in] recursive  Recursive delete?
 * @retval    co   object if found (old _or_ new). NOTE: you must replace calling 
 *                 cg_obj with return.
 * @retval    NULL error
 * @note co1 may be deleted in this call. Dont use co after this call,use retval
 * XXX: pt=[a b] + co1=[b] -> [a b] but children of one b is lost,..
 */
cg_obj*
co_insert1(parse_tree *pt, 
	   cg_obj     *co1,
	   int         recursive)
{
    int     pos;
    cg_obj *co2;

    /* find closest to co in parsetree, insert after pos. */
    pos = co_insert_pos(pt, co1, 0, pt_len_get(pt));
    /* check if exists */
    if (pos < pt_len_get(pt)){
	co2 = pt_vec_i_get(pt, pos); /* insert after co2 */
	if (co1 == NULL && co2==NULL)
	    return NULL;
	if (co1 && co2 && co_eq(co1, co2)==0){
	    cligen_parsetree_merge(co_pt_get(co2), co2, co_pt_get(co1));
	    co_free(co1, recursive);
	    return co2;
	}
    }
    if (pt_vec_i_insert(pt, pos, co1) < 0)
	return NULL;
    return co1;
}

/* Backward compatible */
cg_obj*
co_insert(parse_tree *pt, 
	  cg_obj     *co1)
{
    return co_insert1(pt, co1, 1);
}

/*! Given a parse tree, find the first CLIgen object that matches 
 * @param[in]  pt      CLIgen parse-tree
 * @param[in]  name    Name of node
 * @retval     co      Object found
 * @retval     NULL    Not found
 * You can only use this if the child-list is alphabetically
 * sorted. You get this automatically with co_insert(), bit some code
 * may add children w/o co_insert.
 * No recursion
 */
cg_obj *
co_find_one(parse_tree *pt,
	    char       *name)
{
    return co_search1(pt, name, 0, pt_len_get(pt));
}

/*! Set CLIgen object value
 * Allocate new string, remove old if already set.
 * @param[in]  co      CLIgen object
 * @param[in]  str     Value to set
 * @retval     0       OK
 * @retval     -1      Error
 */
int
co_value_set(cg_obj *co, 
	     char   *str)
{
    if (co->co_value){ /* This can happen in '?/TAB' since we call match twice */
	free(co->co_value);
	co->co_value = NULL;
    }
    if (str != NULL)
	if ((co->co_value = strdup(str)) == NULL){
	    fprintf(stderr, "%s: strdup: %s\n", __FUNCTION__, strerror(errno));
	    return -1;
	}
    return 0;
}

/*! co is a "terminal" command
 * 
 * It contains a child that is of type "EMPTY" OR there are no children.
 * Parsingwise, it means it is printed with a ';' 
 * @param[in]  co   CLIgen object
 * @param[out] cot  CLIgen object of empty statement, if any
 * @retval     0    No, it is not a terminal command
 * @retval     1    Yes, is a terminal command, cot may be returned if exists
 */
int
co_terminal(cg_obj *co,
	    cg_obj **cot)
{
    parse_tree *pt;
    cg_obj     *coc;
    int         ptlen;

    pt = co_pt_get(co);
    ptlen = pt_len_get(pt);
    if (ptlen == 0)
	return 1;
    coc = pt_vec_i_get(pt, 0);
    if (coc == NULL ||
	coc->co_type == CO_EMPTY){
	if (cot)
	    *cot = coc;
	return 1;
    }
    return 0;
}

/*! Create (malloc) a reason string using var-list args
 * General purpose routine but used for error handling.
 * @param[in]  fmt     Format string followed by a variable list (as in  printf) 
 * @retval     reason  Formatted reason string. Free with free()
 * @retval     NULL    Error
 */
char *
cligen_reason(const char *fmt, ...)
{
    char   *reason;
    int     res;
    int     len;
    va_list ap;

    va_start(ap, fmt);
    len = vsnprintf(NULL, 0, fmt, ap);
    len++;
    va_end(ap);
    if ((reason = malloc(len)) == NULL)
	return NULL;
    va_start(ap, fmt);
    if ((res = vsnprintf(reason, len, fmt, ap)) < 0){
	free(reason);
	reason = NULL;
    }
    va_end(ap);
    return reason;	
}

