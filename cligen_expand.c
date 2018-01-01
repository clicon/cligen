/*
  CLI generator. Take idl as input and generate a tree for use in cli.

  ***** BEGIN LICENSE BLOCK *****
 
  Copyright (C) 2001-2018 Olof Hagsand

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
#include <assert.h>
#include <netinet/in.h>

#include "cligen_buf.h"
#include "cligen_var.h"
#include "cligen_cvec.h"
#include "cligen_gen.h"
#include "cligen_handle.h"
#include "cligen_print.h"
#include "cligen_expand.h"
#include "cligen_syntax.h"

/* Callback function for expand variables */


/*! Copy and expand a cligen object.
 * this object could actually give rise to several if it is a variable
 * with expand (co_exp) or choice (co_choice) set.
 * Set co_ref to point back to the original.
 * @param[in]  co     Original cg_obj
 * @param[in]  parent Parent of original co object
 * @param[out] conp   New, shadow object
 * @see co_copy XXX: maybe this could call co_copy?
 */
static int
co_expand_sub(cg_obj  *co, 
	      cg_obj  *parent, 
	      cg_obj **conp)
{
    cg_obj *con;

    if ((con = malloc(sizeof(cg_obj))) == NULL){
	fprintf(stderr, "%s: malloc: %s\n", __FUNCTION__, strerror(errno));
	return -1;
    }
    memcpy(con, co, sizeof(cg_obj));
    /* Replace all pointers */
    co_up_set(con, parent);
    if (co->co_command)
	if ((con->co_command = strdup(co->co_command)) == NULL){
	    fprintf(stderr, "%s: strdup: %s\n", __FUNCTION__, strerror(errno));
	    return -1;
	}
    if (co->co_cvec)
	con->co_cvec = cvec_dup(co->co_cvec);
    if (co->co_userdata && co->co_userlen){
	if ((con->co_userdata = malloc(co->co_userlen)) == NULL){
	    fprintf(stderr, "%s: malloc: %s\n", __FUNCTION__, strerror(errno));
	    return -1;
	}
	memcpy(con->co_userdata, co->co_userdata, co->co_userlen);
    }
    if (co_callback_copy(co->co_callbacks, &con->co_callbacks) < 0)
	return -1;
    if (co->co_help)
	if ((con->co_help = strdup(co->co_help)) == NULL){
	    fprintf(stderr, "%s: strdup: %s\n", __FUNCTION__, strerror(errno));
	    return -1;
	}
    if (co->co_mode){
	if ((con->co_mode = strdup(co->co_mode)) == NULL){
	    fprintf(stderr, "%s: strdup: %s\n", __FUNCTION__, strerror(errno));
	    return -1;
	}
    }
    if (co->co_type == CO_VARIABLE){
	if (co->co_expand_fn_str)
	    if ((con->co_expand_fn_str = strdup(co->co_expand_fn_str)) == NULL){
		fprintf(stderr, "%s: strdup: %s\n", __FUNCTION__, strerror(errno));
		return -1;
	    }
	if (co->co_show)
	    if ((con->co_show = strdup(co->co_show)) == NULL){
		fprintf(stderr, "%s: strdup: %s\n", __FUNCTION__, strerror(errno));
		return -1;
	    }
	if (co->co_expand_fn_vec)
	    if ((con->co_expand_fn_vec = cvec_dup(co->co_expand_fn_vec)) == NULL)
		return -1;
	if (co->co_rangecv_low)
	    if ((con->co_rangecv_low = cv_dup(co->co_rangecv_low)) == NULL)
		return -1;
	if (co->co_rangecv_high)
	    if ((con->co_rangecv_high = cv_dup(co->co_rangecv_high)) == NULL)
		return -1;
	if (co->co_choice)
	    if ((con->co_choice = strdup(co->co_choice)) == NULL){
		fprintf(stderr, "%s: strdup: %s\n", __FUNCTION__, strerror(errno));
		return -1;
	    }
	if (co->co_regex)
	    if ((con->co_regex = strdup(co->co_regex)) == NULL){
		fprintf(stderr, "%s: strdup: %s\n", __FUNCTION__, strerror(errno));
		return -1;
	    }
    } /* CO_VARIABLE */
    con->co_ref = co;
    *conp = con;
    return 0;
}

/*! Transform string variables to commands
 * Expansion of choice or expand takes a variable (<expand> <choice>)
 * and transform them to a set of commands: <string>...<string>
 * @param[in]  co        The variable to transform to a command
 * @param[in]  cmd       Command name
 * @param[out] helptext  Helptext of command
 */
int
transform_var_to_cmd(cg_obj *co, 
		     char   *cmd, 
		     char   *helptext)
{
    if (co->co_command)
	free(co->co_command);
    co->co_command = cmd; 
    if (helptext){
	if (co->co_help)
	    free(co->co_help);
	co->co_help = helptext; 
    }
    if (co->co_expand_fn)
	co->co_expand_fn = NULL;
    if (co->co_expandv_fn)
	co->co_expandv_fn = NULL;
    if (co->co_expand_fn_str){
	free(co->co_expand_fn_str);
	co->co_expand_fn_str = NULL;
    }
    if (co->co_expand_fn_vec){
	cvec_free(co->co_expand_fn_vec);
	co->co_expand_fn_vec = NULL;
    }
    if (co->co_show){
	free(co->co_show);
	co->co_show = NULL;
    }
    if (co->co_rangecv_low){
	cv_free(co->co_rangecv_low);
	co->co_rangecv_low = NULL;
    }
    if (co->co_rangecv_high){
	cv_free(co->co_rangecv_high);
	co->co_rangecv_high = NULL;
    }
    if (co->co_choice){
	free(co->co_choice);
	co->co_choice = NULL;
    }
    if (co->co_regex){
	free(co->co_regex);
	co->co_regex = NULL;
    }
    co->co_type = CO_COMMAND;
    return 0;
}

/*! Recursively copy callback structure to all terminal nodes in the parse-tree.
 * XXX: Actually copies to every node, even if not terminal
 * Problem is that the argument is modified according to reference rule
 * @param[in]  pt   Parse-tree
 * @param[in]  cc0  This is the parameter (calling) callback. eg fn in @sub,fn().
 *
 * The function installs the calling callback in all executable non-terminal nodes.
 * That is, it only install callbacks in non-terminal with NULL child which is 
 * where a ';' is in the syntax, eg:
 *	   a ;{} # Here a is executable
 *	   b {}  # But b is not
 * Somewhat strange semantics though:
 * - Always replace (or add if empty) original callback in co0
 * - Use local argument-list _unless_ there is none, then use callback list from cc0
 */
static int
pt_callback_reference(parse_tree          pt, 
		      struct cg_callback *cc0)
{
    int                 i;
    cg_obj             *co;
    parse_tree         *ptc;
    int                 retval = -1;
    struct cg_callback *cc;

    for (i=0; i<pt.pt_len; i++){    
	if ((co = pt.pt_vec[i]) == NULL)
	    continue;
	ptc = &co->co_pt;
	/* Filter out non-executable non-terminals. */
	if (ptc->pt_len && ptc->pt_vec[0] == NULL){
	    /* Copy the callback from top */
	    if ((cc = co->co_callbacks) == NULL){
		if (co_callback_copy(cc0, &co->co_callbacks) < 0)
		    return -1;
	    }
	    else {
		cc->cc_fn = cc0->cc_fn; /* iterate */
		cc->cc_fn_vec = cc0->cc_fn_vec; /* iterate */
		if (cc0->cc_fn_str){
		    if (cc->cc_fn_str)
			free (cc->cc_fn_str);
		    cc->cc_fn_str = strdup(cc0->cc_fn_str);
		}
	    }
	}
	if (pt_callback_reference(co->co_pt, cc0) < 0)
	    goto done;
    }
    retval = 0;
  done:
    return retval;
}

/*! Truncate parse tree
 */
static int
pt_reference_trunc(parse_tree pt)
{
    int     i;
    cg_obj *co;
    int     retval = -1;

    for (i=0; i<pt.pt_len; i++){    
	if ((co = pt.pt_vec[i]) == NULL)
	    continue;
	if (co->co_nonterminal)
	    if (co->co_max && co->co_next[0] != NULL){ /* Add empty sub */
		co_insert(&co->co_pt, NULL);
	    }
	if (pt_reference_trunc(co->co_pt) < 0)
	    goto done;
    }
    retval = 0;
  done:
    return retval;
}

/*! Take a top-object parse-tree (pt0), and expand all tree references one level. 
 * 
 * One level only. Parse-tree is expanded itself (not copy).
 *
 * @param[in]     h       Handle needed to resolve tree-references (\@tree)
 * @param[in]     coprev  Parent, if any
 * @param[in,out] pt0     parse-tree to expand. In: original, out: expanded
 * @note The loop may need to suboptimal iterations since after every time you
 *       find a tree reference, you add new elements to the list and re-iterates
 *       from start. Since the expanded elements may be references,... 
 * @see pt_expand_treeref_cleanup
 */
int
pt_expand_treeref(cligen_handle h, 
		  cg_obj       *coprev, 
		  parse_tree   *pt0)
{
    int              i, k;
    cg_obj          *co;
    parse_tree      *ptref;           /* tree referenced by pt0 orig */
    parse_tree       pt1ref = {0, };  /* tree referenced by pt0 resolved */
    cg_obj          *cot;             /* treeref object */
    char            *treename;
    cg_obj          *coprev2;

    if (pt0->pt_vec == NULL)
	return 0;
 again:
    for (i=0; i<pt0->pt_len; i++){ /*  */
	if ((co = pt0->pt_vec[i]) == NULL)
	    continue;
	/* XXX remove reference */
	if (co->co_type == CO_REFERENCE && !co->co_refdone){
	    /* Expansion is made in-line so we need to know if already 
	       expanded */
	    treename = co->co_command;

	    /* Find the referring tree */
	    if ((ptref = cligen_tree_find(h, treename)) == NULL){
		fprintf(stderr, "CLIgen subtree '%s' not found\n", 
			treename);
		return -1;
	    }

	    /* make a copy of ptref -> pt1ref */
	    coprev2 = co_up(co);

	    if (pt_copy(*ptref, coprev2, &pt1ref) < 0){ /* From ptref -> pt1ref */
		fprintf(stderr, "%s: Copying parse-tree\n", __FUNCTION__);
		return -1;
	    }
	    /* Recursively add extra NULLs in non-terminals */
	    if (co->co_hide && /* XXX: hide to trunk? */
		pt_reference_trunc(pt1ref) < 0)
		return -1;
	    /* Recursively install callback all through the referenced tree */
	    if (co->co_callbacks && 
		pt_callback_reference(pt1ref, co->co_callbacks) < 0)
		return -1;
	    /* Copy top-levels into original parse-tree */
	    for (k=0; k<pt1ref.pt_len; k++)
		if ((cot = pt1ref.pt_vec[k]) != NULL){
		    cot->co_treeref++; /* Mark as expanded referenced tree */
		    if (co_insert(pt0, cot) == NULL) /* XXX alphabetically */
			return -1;
		}
	    /* Due to loop above, all co in vec should be moved, it should
	       be safe to remove */
	    free(pt1ref.pt_vec);
	    if (coprev && coprev->co_ref) /* coprev2 ? */
		coprev->co_ref->co_pt = coprev->co_pt;

	    co->co_refdone = 1;
	    goto again; 
	}
    }
    return 0;
}

/*! Call expand callback and insert expanded commands in place of variable
 * fixed argument callback variant
 * @note should be obsolete but kept for backwaqrd compatibility
 * @see pt_expandv_fn
 */
static int
pt_expand_fn(cligen_handle h, 
	     cg_obj       *co,     
	     cvec         *cvv,
	     parse_tree   *ptn,
	     cg_obj       *parent)
{
    int     retval = 0;
    int     k;
    int     nr = 0;
    char  **commands = NULL;
    char  **helptexts = NULL;
    cg_obj *con;

    if ((*co->co_expand_fn)(
			    cligen_userhandle(h)?cligen_userhandle(h):h, 
			    co->co_expand_fn_str, 
			    cvv,
			    cvec_i(co->co_expand_fn_vec, 0),
			    &nr, 
			    &commands, 
			    &helptexts) < 0)
	goto done;
    for (k=0; k<nr; k++){
	pt_realloc(ptn);
	if (co_expand_sub(co, parent, 
			  &ptn->pt_vec[ptn->pt_len-1]) < 0)
	    goto done;
	con = ptn->pt_vec[ptn->pt_len-1];
	if (transform_var_to_cmd(con, commands[k], 
				 helptexts?helptexts[k]:NULL) < 0)
	    goto done;
    }

    retval = 0;
 done:
    if (commands)
	free(commands);
    if (helptexts)
	free(helptexts);
    return retval;
}

/*! Call expand callback and insert expanded commands in place of variable
 * variable argument callback variant
 * @see pt_expand_fn
 */
static int
pt_expand_fnv(cligen_handle h, 
	      cg_obj       *co,     
	      cvec         *cvv,
	      parse_tree   *ptn,
	      cg_obj       *parent)
{
    int     retval = -1;
    cvec   *commands = cvec_new(0);
    cvec   *helptexts = cvec_new(0);
    cg_var *cv = NULL;
    char   *helpstr;
    cg_obj *con;
    int     i;

    if ((*co->co_expandv_fn)(
			     cligen_userhandle(h)?cligen_userhandle(h):h, 
			     co->co_expand_fn_str, 
			     cvv,
			     co->co_expand_fn_vec,
			     commands, 
			     helptexts) < 0)
	goto done;
    i = 0;
    while ((cv = cvec_each(commands, cv)) != NULL) {
	if (i < cvec_len(helptexts)){
	    helpstr = strdup(cv_string_get(cvec_i(helptexts, i)));
	}
	else
	    helpstr = NULL;
	i++;
	pt_realloc(ptn);
	if (co_expand_sub(co, parent, 
			  &ptn->pt_vec[ptn->pt_len-1]) < 0)
	    goto done;
	con = ptn->pt_vec[ptn->pt_len-1];
	if (transform_var_to_cmd(con, 
				 strdup(cv_string_get(cv)),
				 helpstr) < 0)
	    goto done;
    }
    if (commands)
	cvec_free(commands);
    if (helptexts)
	cvec_free(helptexts);
    retval = 0;
 done:
    return retval;

}

/*! Expand a choice rule with actual commands
 */
static int
pt_expand_choice(cg_obj       *co,     
		 parse_tree   *ptn,
		 cg_obj       *parent)
{
    int     retval = -1;
    char   *ccmd;
    char   *cp = NULL;
    char   *c;
    cg_obj *con;

    /* parse co_command and get alternatives <alt:hej,hopp> */
    if (co->co_choice){
	cp = ccmd = strdup(co->co_choice);
	while ((c = strsep(&ccmd, ",|")) != NULL){
	    pt_realloc(ptn);
	    if (co_expand_sub(co, parent, 
			      &ptn->pt_vec[ptn->pt_len-1]) < 0)
		goto done;
	    con = ptn->pt_vec[ptn->pt_len-1];
	    if (transform_var_to_cmd(con, strdup(c), NULL) < 0) 
		goto done;
	}
    }
    retval = 0;
 done:
    if (cp)
	free(cp);
    return retval;
}
/*! Take a pattern and expand all <variables> with option 'choice' or 'expand'. 
 * The pattern is expanded by examining the objects they point 
 * to: those objects that are expand or choice variables
 * (eg <string expand:foo>) are transformed into a set of new commands
 * with a reference point back to the original.
 * @param[in]  h       cligen handle
 * @param[in]  ptr     original parse-tree consisting of a vector of cligen objects
 * @param[in]  parent  parent of original co object
 * @param[out] ptn     shadow parse-tree initially an empty pointer, its value is returned.
 */
int
pt_expand_2(cligen_handle h, 
	    parse_tree   *ptr, 
	    cvec         *cvv,
	    parse_tree   *ptn, 
	    int           hide) 
{
    int          i;
    cg_obj      *co;
    cg_obj      *parent = NULL;
    int          retval = -1;

    ptn->pt_len = 0;
    ptn->pt_vec = NULL;
    if (ptr->pt_vec == NULL)
	return 0;
    for (i=0; i<ptr->pt_len; i++){ /* Build ptn (new) from ptr (orig) */
	if ((co = ptr->pt_vec[i]) != NULL){
	    if (co_value_set(co, NULL) < 0)
		goto done;
	    if (hide && co->co_hide)
		continue;
	    /*
	     * Choice variable - Insert the static choices as commands in place
	     * of the variable
	     */
	    if (co->co_type == CO_VARIABLE && co->co_choice != NULL){
		if (pt_expand_choice(co, ptn, parent) < 0)
		    goto done;
	    }
	    else
		/* Expand variable - call expand callback and insert expanded
		 * commands in place of the variable
		 */
		if (co->co_type == CO_VARIABLE && 
		    co->co_expand_fn != NULL){
		    if (pt_expand_fn(h, co, cvv, ptn, parent) < 0)
			goto done;
		}
		else
		    if (co->co_type == CO_VARIABLE && 
			co->co_expandv_fn != NULL){
			if (pt_expand_fnv(h, co, cvv, ptn, parent) < 0)
			    goto done;
		    }
		    else{
			/* Copy vector element */
			pt_realloc(ptn);
			/* Copy cg_obj */
			if (co_expand_sub(co, parent, 
					  &ptn->pt_vec[ptn->pt_len-1]) < 0)
			    goto done;
			/* Reference old cg_obj */
			//		  con = ptn->pt_vec[ptn->pt_len-1];
		    }
	}
	else{ 
	    pt_realloc(ptn); /* empty child */
	}
    } /* for */
    cligen_parsetree_sort(*ptn, 1);
    if (cligen_logsyntax(h) > 0){
	fprintf(stderr, "%s:\n", __FUNCTION__);
	cligen_print(stderr, *ptn, 0);
    }
    retval = 0;
 done:
    return retval;
}

/*! Go through tree and clean & delete all extra memory from pt_expand_treeref()
 * More specifically, delete all expanded subtrees co_ref
 * @see pt_expand_treeref
 */
int
pt_expand_treeref_cleanup(parse_tree *pt)
{
    int i, j;
    cg_obj *co;

    if (pt->pt_vec == NULL)
        return 0;
    for (i=0; i<pt->pt_len; i++){
      again:
	if ((co = pt->pt_vec[i]) != NULL){
	    if (co->co_refdone)
		co->co_refdone = 0;
	    if (co->co_treeref){
		pt->pt_vec[i] = NULL;
		co_free(co, 1);
		for (j=i; j<pt->pt_len-1; j++)
		    pt->pt_vec[j] = pt->pt_vec[j+1];
		pt->pt_len--;
		if (i<pt->pt_len)
		    goto again;
		else
		    break;
	    }
	    else
		if (pt_expand_treeref_cleanup(&co->co_pt) < 0)
		    return -1;
	}
    }
    return 0;
}

/*! Go through tree and clean & delete all extra memory from pt_expand_2()
 * More specifically, delete all co_values and co_pt_exp.
 * @see pt_expand_2
 */
int
pt_expand_cleanup_2(parse_tree pt)
{
    int i;
    cg_obj *co;

    if (pt.pt_vec == NULL)
        return 0;
    for (i=0; i<pt.pt_len; i++){
	if ((co = pt.pt_vec[i]) != NULL){
	    if (co_value_set(co, NULL) < 0)
		return -1;
	    if (co->co_pt_exp.pt_vec){
		if (cligen_parsetree_free(co->co_pt_exp, 0) < 0)
		    return -1;
		memset(&co->co_pt_exp, 0, sizeof(parse_tree));
	    }
	    if (pt_expand_cleanup_2(co->co_pt) < 0)
		return -1;
	}
    }
    return 0;
}


/*! Help functions to delete hanging memory
 * It is allocated in match_pattern_node, and deallocated 
 * after every call to pt_expand - because we do not now in
 * match_pattern_node when it will be used.
 */
int
pt_expand_add(cg_obj    *co, 
	      parse_tree ptn)
{
    if (co->co_pt_exp.pt_vec != NULL){
	if (cligen_parsetree_free(co->co_pt_exp, 0) < 0)
	    return -1;
    }
    co->co_pt_exp = ptn;
    return 0;
}


/*! Return object in original tree from  object in a referenced tree
 * Given an object in a referenced tree, and the top of the original tree,
 * return the corresponding object in the original tree.
 * The figure tries to show:
     ref-tree      orig
     -----        ----- pt0
       o            o (given top)
     /   \        /   \
    o     o      o     o
     \            \
      o co1         o co0
       (given)     (we want to find this)
 */
int
reference_path_match(cg_obj    *co1, 
		     parse_tree pt0, 
		     cg_obj   **co0p)
{
    cg_obj    *co0, *co;

    if (co1 == NULL)
	return -1;
    if (co1->co_treeref){ /* at top */
	if ((co0 = co_find_one(pt0, co1->co_command)) == NULL)
	    return -1;
	*co0p = co0;
	return 0;
    }
    if (reference_path_match(co_up(co1), pt0, &co) < 0)
	return -1;
    if ((co0 = co_find_one(co->co_pt, co1->co_command)) == NULL)
	return -1;
    *co0p = co0;
    return 0;
}
