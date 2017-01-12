/*
  CLI generator. Take idl as input and generate a tree for use in cli.

  ***** BEGIN LICENSE BLOCK *****
 
  Copyright (C) 2001-2017 Olof Hagsand

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


/*
 * co_expand_sub
 * copy and expand a cligen object.
 * this object could actually give rise to several if it is a variable
 * with expand (co_exp) or choice (co_choice) set.
 * Set co_ref to point back to the original.
 * Arguments:
 *   co:     original cg_obj
 *   parent: parent of original co object
 *   conp:   new, shadow object
 * (see also co_copy XXX: maybe this could call co_copy?)
 */
static int
co_expand_sub(cg_obj *co, cg_obj *parent, cg_obj **conp)
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
	if (co->co_expand_fn_arg)
	    if ((con->co_expand_fn_arg = cv_dup(co->co_expand_fn_arg)) == NULL)
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

/*
 * transform_var_to_cmd
 * expansion of choice or expand need to take a variable (<expand> <choice>)
 * and transform them to a set of commands: <string>...<string>
 */
int
transform_var_to_cmd(cg_obj *co, char *cmd, char *comment)
{
    if (co->co_command)
	free(co->co_command);
    co->co_command = cmd; 
    if (comment){
	if (co->co_help)
	    free(co->co_help);
	co->co_help = comment; 
    }
    if (co->co_expand_fn)
	co->co_expand_fn = NULL;
    if (co->co_expand_fn_str){
	free(co->co_expand_fn_str);
	co->co_expand_fn_str = NULL;
    }
    if (co->co_show){
	free(co->co_show);
	co->co_show = NULL;
    }
    if (co->co_expand_fn_arg){
	cv_free(co->co_expand_fn_arg);
	co->co_expand_fn_arg = NULL;
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
pt_callback_reference(parse_tree pt, struct cg_callback *cc0)
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
		cc->cc_fnv = cc0->cc_fnv; /* iterate */
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



/*! Take a top-object parse-tree (pt), and expand all tree references in its children. 
 * 
 * One level only. Parse-tree is expanded itself (not copy).
 * XXX: CIRCULAR COPY. You copy from S already expanded tree! Gaah
 *
 * @param[in]     h       Handle needed to resolve tree-references (\@tree)
 * @param[in]     coprev  Parent, if any
 * @param[in,out] pt      parse-tree to expand
 */
int
pt_expand_1(cligen_handle h, 
	    cg_obj       *coprev, 
	    parse_tree   *pt)
{
    int              i, k;
    cg_obj          *co;
    parse_tree      *ptt0;             /* treeref orig */
    parse_tree       ptt = {0, };     /* treeref copy */
    cg_obj          *cot;             /* treeref object */
    char            *treeref;
    cg_obj          *coprev2;

    if (pt->pt_vec == NULL)
	return 0;
  again:
    for (i=0; i<pt->pt_len; i++){ /*  */
	if ((co = pt->pt_vec[i]) != NULL){
	    if (co->co_type == CO_REFERENCE && !co->co_refdone){
		 /* Expansion is made in-line so we need to know if already 
		    expanded */
		treeref = co->co_command;

		/* Find the referring tree */
		if ((ptt0 = cligen_tree_find(h, treeref)) == NULL){
		    fprintf(stderr, "CLIgen subtree '%s' not found\n", 
			    treeref);
		    return -1;
		}

		/* make a copy of ptt0 -> ptt */
		coprev2 = co_up(co);

		if (pt_copy(*ptt0, coprev2, &ptt) < 0){ /* From ptt0 -> ptt */
		    fprintf(stderr, "%s: Copying parse-tree\n", __FUNCTION__);
		    return -1;
		}
		/* Recursively add extra NULLs in non-terminals */
		if (co->co_hide && /* XXX: hide to trunk? */
		    pt_reference_trunc(ptt) < 0)
		    return -1;

		/* Recursively install callback all through the referenced tree */
		if (co->co_callbacks && 
		    pt_callback_reference(ptt, co->co_callbacks) < 0)
		    return -1;
		/* Copy top-levels into original parse-tree */
		for (k=0; k<ptt.pt_len; k++)
		    if ((cot = ptt.pt_vec[k]) != NULL){
			cot->co_treeref++;
			if (co_insert(pt, cot) == NULL) /* XXX alphabetically */
			    return -1;
		    }
		/* Due to loop above, all co in vec should be moved, it should
		   be safe to remove */
		free(ptt.pt_vec);
		if (coprev && coprev->co_ref) /* coprev2 ? */
		    coprev->co_ref->co_pt = coprev->co_pt;
		/* Find more unmarked references. XXX: This may be recursive,
		 but then the designer of the syntax is an idiot,... */
		co->co_refdone = 1;
		goto again; 
	    }
	}
    }
     return 0;
}


/*
 * pt_expand_2
 * Take a pattern and expand all <variables> with option 'choice' or
 * 'expand'. The pattern is expanded by examining the objects they point 
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
	    cvec         *cvec,
	    parse_tree   *ptn, 
	    int           hide) 
{
    int          i, k, nr;
    cg_obj      *co;
    cg_obj      *con;
    char       **commands;
    char       **helptexts;
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
	      char *ccmd, *cp, *c;
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
		if (cp)
		  free(cp);
	      }
	    }
	    else
		/*
		 * expand variable - call expand callback and insert expanded
		 * commands in place of the variable
		 */
	    if (co->co_type == CO_VARIABLE && co->co_expand_fn != NULL){
	      commands = NULL;
	      helptexts = NULL;
	      nr = 0;
	      if ((*co->co_expand_fn)(
		     cligen_userhandle(h)?cligen_userhandle(h):h, 
		     co->co_expand_fn_str, 
		     cvec,
		     co->co_expand_fn_arg, 
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
	      if (commands)
		  free(commands);
	      if (helptexts)
		  free(helptexts);
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

/* 
 * pt_expand_cleanup_1
 * Go through tree and clean & delete all extra memory from pt_expand_1()
 * More specifically, delete all expanded subtrees co_ref
 */
int
pt_expand_cleanup_1(parse_tree *pt)
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
		if (pt_expand_cleanup_1(&co->co_pt) < 0)
		    return -1;
	}
    }
    return 0;
}

/* 
 * pt_expand_cleanup_2
 * Go through tree and clean & delete all extra memory from pt_expand_2()
 * More specifically, delete all co_values and co_pt_exp.
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


/*
 * help functions to delete hanging memory
 * It is allocated in match_pattern_node, and deallocated 
 * after every call to pt_expand - because we do not now in
 * match_pattern_node when it will be used.
 */
int
pt_expand_add(cg_obj *co, parse_tree ptn)
{
    if (co->co_pt_exp.pt_vec != NULL){
	if (cligen_parsetree_free(co->co_pt_exp, 0) < 0)
	    return -1;
    }
    co->co_pt_exp = ptn;
    return 0;
}


/*
 * reference_path_match
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
reference_path_match(cg_obj *co1, parse_tree pt0, cg_obj **co0p)
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
