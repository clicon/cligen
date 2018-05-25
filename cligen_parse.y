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


  Choice:
*/
%start file

%union {
  int intval;
  char *string;
}

%token MY_EOF
%token V_RANGE V_LENGTH V_CHOICE V_KEYWORD V_REGEXP V_FRACTION_DIGITS V_SHOW V_TREENAME V_TRANSLATE
%token DOUBLEPARENT /* (( */
%token DQ           /* " */
%token DQP          /* ") */
%token PDQ          /* (" */

%token <string> NAME    /* in variables: <NAME type:NAME> */
%token <string> NUMBER  /* In variables */
%token <string> DECIMAL /* In variables */
%token <string> CHAR

%type <string> charseq
%type <string> choices
%type <string> numdec
%type <string> arg1
%type <string> typecast

%lex-param     {void *_ya} /* Add this argument to parse() and lex() function */
%parse-param   {void *_ya}

%{
/* Here starts user C-code */

/* typecast macro */
#define _YA ((cliyacc *)_ya)

/* add _ya to error paramaters */
#define YY_(msgid) msgid 

#include "cligen_config.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>

#include "cligen_buf.h"
#include "cligen_var.h"
#include "cligen_cvec.h"
#include "cligen_gen.h"
#include "cligen_syntax.h"
#include "cligen_handle.h"
#include "cligen_parse.h"

static int debug = 0;

extern int cligen_parseget_lineno  (void);

int 
cligen_parse_debug(int d)
{
    debug = d;
    return 0;
}


/*! CLIGEN parse error routine
 * Also called from yacc generated code *
 * @param[in]  ya  CLIgen yacc parse struct
 */
void cligen_parseerror(void *_ya,
		       char *s) 
{ 
  fprintf(stderr, "%s%s%d: Error: %s: at or before: '%s'\n", 
	  _YA->ya_name,
	   ":" ,
	  _YA->ya_linenum ,
	  s, 
	  cligen_parsetext); 
  return;
}

#define cligen_parseerror1(ya, s) cligen_parseerror(ya, s)

/*! Create a CLIgen variable (cv) and store it in the current variable object 
 * Note that only one such cv can be stored.
 * @param[in]  ya  CLIgen yacc parse struct
 */
static cg_var *
create_cv(cliyacc *ya,
	  char    *type,
	  char    *str)
{
    cg_var   *cv = NULL;

    if ((cv = cv_new(CGV_STRING)) == NULL){
	fprintf(stderr, "malloc: %s\n", strerror(errno));
	goto done;
    }
    if (type){
	if (cv_type_set(cv, cv_str2type(type)) == CGV_ERR){
	    fprintf(stderr, "%s:%d: error: No such type: %s\n",
		    ya->ya_name, ya->ya_linenum, type);
	    cv_free(cv); cv = NULL;
	    goto done;
	}
    }
    if (cv_parse(str, cv) < 0){ /* parse str into cgv */
	cv_free(cv); cv = NULL;
	goto done;
    }
  done:
    return cv;
}

/*!
 * @param[in]  ya  CLIgen yacc parse struct
 */
static int
cgy_flag(cliyacc *ya,
	 char    *var)
{
    struct cgy_stack    *cs = ya->ya_stack;
    cg_var              *cv;
    int                  retval = -1;

    if (debug)
	fprintf(stderr, "%s: %s 1\n", __FUNCTION__, var);
    if (cs){ /* XXX: why cs? */
	if (ya->ya_cvec == NULL){
	    if ((ya->ya_cvec = cvec_new(0)) == NULL){
		fprintf(stderr, "%s: cvec_new:%s\n", __FUNCTION__, strerror(errno));
		goto done;
	    }
	}
	if ((cv = cvec_add(ya->ya_cvec, CGV_INT32)) == NULL){
	    fprintf(stderr, "%s: realloc:%s\n", __FUNCTION__, strerror(errno));
	    goto done;
	}
	cv_name_set(cv, var);
	cv_int32_set(cv, 1);
    }
    retval = 0;
  done:
    return retval;
}

/*! Set a new treename. In fact registers the previous tree and creates a new .
 * Note that one could have used an assignment: treename = <name>; for this but
 * I decided to create special syntax for this so that assignments can use any
 * variable names.
 * @param[in]  ya   CLIgen yacc parse struct
 * @param[in]  name Name of tree
 */
static int
cgy_treename(cliyacc *ya,
	     char    *name)
{
    cg_obj            *co = NULL;
    cg_obj            *cot;
    struct cgy_list   *cl; 
    int                retval = -1;
    int                i;
    parse_tree        *pt;

    /* 1. Get the top object (cache it?) */
    for (cl=ya->ya_list; cl; cl = cl->cl_next){
	co = cl->cl_obj;
	break;
    }
    cot = co_top(co);
    pt = &cot->co_pt;
    /* If anything anything parsed */
    if (pt->pt_len){ 
	/* 2. Add the old parse-tree with old name*/
	for (i=0; i<pt->pt_len; i++){
	    if ((co=pt->pt_vec[i]) != NULL)
		co_up_set(co, NULL);
	}
	if (cligen_tree_add(ya->ya_handle, ya->ya_treename, *pt) < 0)
	    goto done;
	/* 3. Create new parse-tree XXX */
	memset(pt, 0, sizeof(*pt));
    }

    /* 4. Set the new name */
    if (ya->ya_treename)
	free(ya->ya_treename);
    if ((ya->ya_treename = strdup(name)) == NULL){
	fprintf(stderr, "%s: strdup: %s\n", __FUNCTION__, strerror(errno));
	goto done;
    }
    retval = 0;
 done:
    return retval;
}

/*! Variable assignment
 * Only string type supported for now
 * @param[in]  ya  CLIgen yacc parse struct
 */
static int
cgy_assignment(cliyacc *ya,
	       char    *var,
	       char    *val)
{
    struct cgy_stack *cs = ya->ya_stack;
    int              retval = -1;
    cg_var          *cv;
    char            *treename_keyword;
    cligen_handle    h = ya->ya_handle;

    if (debug)
	fprintf(stderr, "%s: %s=%s\n", __FUNCTION__, var, val);
    if (cs == NULL){
	fprintf(stderr, "%s: Error, stack should not be NULL\n", __FUNCTION__);
    }
     if (cs->cs_next != NULL){ /* local */
	 if (ya->ya_cvec == NULL)
	     if ((ya->ya_cvec = cvec_new(0)) == NULL){
		 fprintf(stderr, "%s: cvec_new:%s\n", __FUNCTION__, strerror(errno));
		 goto done;
	     }
	 if ((cv = cvec_add(ya->ya_cvec, CGV_STRING)) == NULL){
	    fprintf(stderr, "%s: realloc:%s\n", __FUNCTION__, strerror(errno));
	    goto done;
	}
	cv_name_set(cv, var);
	if (cv_parse(val, cv) < 0)
	    goto done;
    }
    else{ /* global */
	treename_keyword = cligen_treename_keyword(h);
	if (strcmp(var, treename_keyword) == 0){
	    if (cgy_treename(ya, val) < 0)
		goto done;
	}
	else {
	    if ((cv = cvec_add(ya->ya_globals, CGV_STRING)) == NULL){
		fprintf(stderr, "%s: realloc:%s\n", __FUNCTION__, strerror(errno));
		goto done;
	    }
	    cv_name_set(cv, var);
	    if (cv_parse(val, cv) < 0)  /* May be wrong type */
		goto done;
	}
    }
    retval = 0;
  done:
    return retval;
}

/*!
 * @param[in]  ya  CLIgen yacc parse struct
 */
int
cgy_callback(cliyacc *ya,
	     char    *cb_str)
{
    struct cgy_stack    *cs = ya->ya_stack;
    struct cg_callback *cc, **ccp;

    if (debug)
	fprintf(stderr, "%s: %s\n", __FUNCTION__, cb_str);
    if (cs == NULL)
	return 0;
    ccp = &ya->ya_callbacks;
    while (*ccp != NULL)
	ccp = &((*ccp)->cc_next);
    if ((cc = malloc(sizeof(*cc))) == NULL){
	fprintf(stderr, "%s: malloc: %s\n", __FUNCTION__, strerror(errno));
	cligen_parseerror1(ya, "Allocating cligen callback"); 
	return -1;
    }
    memset(cc, 0, sizeof(*cc));
    cc->cc_fn_str = cb_str;
    *ccp = cc;
    return 0;
}

/*! Create a callback argument  and store it in the current callback
 * @param[in]  ya  CLIgen yacc parse struct
 */
static int
cgy_callback_arg(cliyacc *ya, 
		 char    *type, 
		 char    *arg)
{
    int                 retval = -1;
    struct cg_callback *cc;
    struct cg_callback *cclast;
    cg_var             *cv = NULL;

    cclast = NULL;
    for (cc=ya->ya_callbacks; cc; cc=cc->cc_next)
	cclast = cc;
    if (cclast){
	if ((cv = create_cv(ya, type, arg)) == NULL)
	    goto done;
	if (cclast->cc_cvec)
	    cvec_append_var(cclast->cc_cvec, cv);
	else
	    cclast->cc_cvec = cvec_from_var(cv);
    }
    retval = 0;
  done:
    if (cv)
	cv_free(cv);
    return retval;
}

/*!
 * @param[in]  ya  CLIgen yacc parse struct
 */
static int
expand_arg(cliyacc *ya,
	   char    *type,
	   char    *arg)
{
   int      retval = -1;
    cg_var *cv = NULL;

    if ((cv = create_cv(ya, type, arg)) == NULL)
	goto done;
    if (ya->ya_var->co_expand_fn_vec)
	cvec_append_var(ya->ya_var->co_expand_fn_vec, cv);
    else
	ya->ya_var->co_expand_fn_vec = cvec_from_var(cv);
    retval = 0;
  done:
    if (cv)
	cv_free(cv);
    return retval;
}

/*!
 * @param[in]  ya  CLIgen yacc parse struct
 */
static int
expand_fn(cliyacc *ya,
	  char    *fn)
{
    ya->ya_var->co_expand_fn_str = fn;
    return 0;
}

static int
cg_translate(cliyacc *ya,
	     char    *fn)
{
    ya->ya_var->co_translate_fn_str = fn;
    return 0;
}

static int
cgy_list_push(cg_obj           *co,
	      struct cgy_list **cl0)
{
    struct cgy_list *cl;

    if (debug)
	fprintf(stderr, "%s\n", __FUNCTION__);
    if ((cl = malloc(sizeof(*cl))) == NULL) {
	fprintf(stderr, "%s: malloc: %s\n", __FUNCTION__, strerror(errno));
	return -1;
    }
    cl->cl_next = *cl0;
    cl->cl_obj = co;
    *cl0 = cl;
    return 0;
}

/*! Delete whole list 
 * @param[in]  ya  CLIgen yacc parse struct
 */
static int
cgy_list_delete(struct cgy_list **cl0)
{
    struct cgy_list *cl;

    while ((cl = *cl0) != NULL){
	*cl0 = cl->cl_next;
	free(cl);
    }
    return 0;
}

/*! Create new tmp variable cligen object 
 * It must be filled in by later functions.
 * The reason is, the variable must be completely parsed by successive syntax
 * (eg <type:> stuff) and we cant insert it into the object tree until that is done.
 * And completed by the '_post' function
 * @param[in]  ya  CLIgen yacc parse struct
 * @retval tmp variable object
 * @see cgy_var_post
 */
static cg_obj *
cgy_var_create(cliyacc *ya)
{
    cg_obj *co;

    /* Create unassigned variable object */
    if ((co = cov_new(CGV_ERR, NULL)) == NULL){
	fprintf(stderr, "%s: malloc: %s\n", __FUNCTION__, strerror(errno));
	cligen_parseerror1(ya, "Allocating cligen object"); 
	return NULL;
    }
    if (debug)
	fprintf(stderr, "%s: pre\n", __FUNCTION__);
    return co;
}

/*! Set name and type on a (previously created) variable
 * @param[in]  ya  CLIgen yacc parse struct
 * @see cgy_var_create
 */
static int
cgy_var_name_type(cliyacc *ya,
	    char    *name,
	    char    *type)
{
    ya->ya_var->co_command = name; 
    if ((ya->ya_var->co_vtype = cv_str2type(type)) == CGV_ERR){
	cligen_parseerror1(ya, "Invalid type"); 
	fprintf(stderr, "%s: Invalid type: %s\n", __FUNCTION__, type);
	return -1;
    }
    return 0;
}

/*! Complete variable cligen object after parsing is complete,
 * And insert it into object hierarchies. 
 * That is, insert a variable in each hieracrhy.
 * @param[in]  ya  CLIgen yacc parse struct
 * @retval 0 on OK
 * @retval -1 on error
 */
static int
cgy_var_post(cliyacc *ya)
{
    struct cgy_list *cl; 
    cg_obj          *coc; /* variable copy object */
    cg_obj          *coparent; /* parent */
    cg_obj          *co;  /* new obj/sister */
    cg_obj          *coy = ya->ya_var;

#if 0
    if (coy->co_vtype == CGV_ERR) /* unassigned */
	coy->co_vtype = cv_str2type(coy->co_command);
#endif
    if (debug)
	fprintf(stderr, "%s: cmd:%s vtype:%d\n", __FUNCTION__, 
		coy->co_command,
		coy->co_vtype );
    if (coy->co_vtype == CGV_ERR){
	cligen_parseerror1(ya, "Wrong or unassigned variable type"); 	
	return -1;
    }
#if 0 /* XXX dont really know what i am doing but variables dont behave nice in choice */
    if (ya->ya_opt){     /* get coparent from stack */
	if (ya->ya_stack == NULL){
	    fprintf(stderr, "Option allowed only within () or []\n");
	    return -1;
	}
	cl = ya->ya_stack->cs_list;
    }
    else
#endif
	cl = ya->ya_list;
    for (; cl; cl = cl->cl_next){
	coparent = cl->cl_obj;
	if (cl->cl_next){
	    if (co_copy(coy, coparent, &coc) < 0) /* duplicate coy to coc */
		return -1;
	}
	else
	    coc = coy; /* Dont copy if last in list */
	co_up_set(coc, coparent);
	if ((co = co_insert(&coparent->co_pt, coc)) == NULL) /* coc may be deleted */
	    return -1;
	cl->cl_obj = co;
    }
    return 0;
}

/*! Create a new command object. 
 * Actually, create a new for every tree in the list
 * and replace the old with the new object.
 * @param[in]  ya  CLIgen yacc parse struct
 * @param[in]  cmd the command string
 * @retval     0   OK
 * @retval    -1   Error
 */
static int
cgy_cmd(cliyacc *ya,
	char    *cmd)
{
    struct cgy_list *cl; 
    cg_obj *cop; /* parent */
    cg_obj *conew; /* new obj */
    cg_obj *co; /* new/sister */

    for (cl=ya->ya_list; cl; cl = cl->cl_next){
	cop = cl->cl_obj;
	if (debug)
	    fprintf(stderr, "%s: %s parent:%s\n",
		    __FUNCTION__, cmd, cop->co_command);
	if ((conew = co_new(cmd, cop)) == NULL) { 
	    cligen_parseerror1(ya, "Allocating cligen object"); 
	    return -1;
	}
	if ((co = co_insert(&cop->co_pt, conew)) == NULL)  /* co_new may be deleted */
	    return -1;
	cl->cl_obj = co; /* Replace parent in cgy_list */
    }
    return 0;
}

/*! Create a REFERENCE node that references another tree.
 * This is evaluated in runtime by pt_expand().
 * @param[in]  ya  CLIgen yacc parse struct
 * @see also db2tree() in clicon/apps/cli_main.c on how to create such a tree
 * @see pt_expand_treeref()/pt_callback_reference() how it is expanded
 */
static int
cgy_reference(cliyacc *ya, 
	      char    *name)
{
    struct cgy_list *cl; 
    cg_obj          *cop;   /* parent */
    cg_obj          *cot;   /* tree */

    for (cl=ya->ya_list; cl; cl = cl->cl_next){
	/* Add a treeref 'stub' which is expanded in pt_expand to a sub-tree */
	cop = cl->cl_obj;
	if ((cot = co_new(name, cop)) == NULL) { 
	    cligen_parseerror1(ya, "Allocating cligen object"); 
	    return -1;
	}
	cot->co_type    = CO_REFERENCE;
	if ((cot = co_insert(&cop->co_pt, cot)) == NULL)  /* cot may be deleted */
	    return -1;
	/* Replace parent in cgy_list: not allowed after ref?
	   but only way to add callbacks to it.
	*/
	cl->cl_obj = cot;
    }
    return 0;
}


/*! Add comment
 * Assume comment is malloced and not freed by parser 
 * @param[in]  ya  CLIgen yacc parse struct
 */
static int
cgy_comment(cliyacc *ya,
	    char    *comment)
{
    struct cgy_list *cl; 
    cg_obj          *co; 

    for (cl = ya->ya_list; cl; cl = cl->cl_next){
	co = cl->cl_obj;
	if (co->co_help == NULL) /* Why would it already have a comment? */
	    if ((co->co_help = strdup(comment)) == NULL){
		cligen_parseerror1(ya, "Allocating comment"); 
		return -1;
	    }
    }
    return 0;
}

/*!
 * @param[in]  ya  CLIgen yacc parse struct
 */
static char *
cgy_choice_merge(cliyacc *ya,
		 char    *str,
		 char    *app)
{
    int len;
    char *s;

    len = strlen(str)+strlen(app) + 2;
    if ((s = realloc(str, len)) == NULL) {
	fprintf(stderr, "%s: realloc: %s\n", __FUNCTION__, strerror(errno));
	return NULL;
    }
    strncat(s, "|", len-1);
    strncat(s, app, len-1);
    return s;
}

/*! Post-processing of commands, eg at ';':
 *  a cmd;<--
 * But only if parsing succesful.
 * 1. Add callback and args to every list
 * 2. Add empty child unless already empty child
 * @param[in]  ya  CLIgen yacc parse struct
 */
int
cgy_terminal(cliyacc *ya)
{
    struct cgy_list    *cl; 
    cg_obj             *co; 
    int                 i;
    struct cg_callback *cc, **ccp;
    int                 retval = -1;
    
    for (cl = ya->ya_list; cl; cl = cl->cl_next){
	co  = cl->cl_obj;
		
	if (ya->ya_callbacks){ /* callbacks */
	    ccp = &co->co_callbacks;
	    while (*ccp != NULL)
		ccp = &((*ccp)->cc_next);
	    if (co_callback_copy(ya->ya_callbacks, ccp) < 0)
		goto done;
	}
	/* variables: special case hide, auth */
	if (ya->ya_cvec){
#ifdef notyet /* XXX: where did auth code go? */
	    if ((cv = cvec_find_var(ya->ya_cvec, "auth")) != NULL)
		co->co_auth = strdup(cv_string_get(cv));
#endif
	    if (cvec_find_var(ya->ya_cvec, "hide") != NULL)
		co->co_hide = 1;
	    /* generic variables */
	    if ((co->co_cvec = cvec_dup(ya->ya_cvec)) == NULL){
		fprintf(stderr, "%s: cvec_dup: %s\n", __FUNCTION__, strerror(errno));
		goto done;
	    }
	}
	/* misc */
	for (i=0; i<co->co_max; i++)
	    if (co->co_next[i]==NULL)
		break;
	if (i == co->co_max) /* Insert empty child if ';' */
	    co_insert(&co->co_pt, NULL);
    }
    /* cleanup */
    while ((cc = ya->ya_callbacks) != NULL){
	if (cc->cc_cvec)	
	    cvec_free(cc->cc_cvec);
	if (cc->cc_fn_str)	
	    free(cc->cc_fn_str);
	ya->ya_callbacks = cc->cc_next;
	free(cc);
    }
    if (ya->ya_cvec){
	cvec_free(ya->ya_cvec);
	ya->ya_cvec = NULL;
    }
    retval = 0;
  done:
    return retval;  
}

/*! Take the whole cgy_list and push it to the stack 
 * @param[in]  ya  CLIgen yacc parse struct
 */
static int
ctx_push(cliyacc *ya)
{
    struct cgy_list *cl; 
    struct cgy_stack *cs; 

    if (debug)
	fprintf(stderr, "%s\n", __FUNCTION__);
    if ((cs = malloc(sizeof(*cs))) == NULL) {
	fprintf(stderr, "%s: malloc: %s\n", __FUNCTION__, strerror(errno));
	return -1;
    }
    memset(cs, 0, sizeof(*cs));
    cs->cs_next = ya->ya_stack;
    ya->ya_stack = cs;
    for (cl = ya->ya_list; cl; cl = cl->cl_next){
    	if (cgy_list_push(cl->cl_obj, &cs->cs_list) < 0) 
	    return -1;
    }
    return 0;
}

/*! Peek context from stack and replace the object list with it
 * Typically done in a choice, eg "(c1|c2)" at c2.
 * Dont pop context
 * @param[in]  ya  CLIgen yacc parse struct
 */
static int
ctx_peek_swap(cliyacc *ya)
{
    struct cgy_stack *cs; 
    struct cgy_list  *cl; 
    cg_obj           *co; 

    if (debug)
	fprintf(stderr, "%s\n", __FUNCTION__);
    if ((cs = ya->ya_stack) == NULL){
#if 1
	cligen_parseerror1(ya, "No surrounding () or []"); 
	return -1; /* e.g a|b instead of (a|b) */
#else
	cgy_list_delete(&ya->ya_list);
	return 0;
#endif
    }
    for (cl = ya->ya_list; cl; cl = cl->cl_next){
	co = cl->cl_obj;
	if (cgy_list_push(co, &cs->cs_saved) < 0)
	    return -1;
    }
    cgy_list_delete(&ya->ya_list);
    for (cl = cs->cs_list; cl; cl = cl->cl_next){
	co = cl->cl_obj;
	if (cgy_list_push(co, &ya->ya_list) < 0)
	    return -1;
    }
    return 0;
}

/*! Peek context from stack and replace the object list with it
 * Typically done in a choice, eg "(c1|c2)" at c2.
 * Dont pop context
 * @param[in]  ya  CLIgen yacc parse struct
 */
static int
ctx_peek_swap2(cliyacc *ya)
{
    struct cgy_stack *cs; 
    struct cgy_list  *cl; 
    cg_obj           *co; 

    if (debug)
	fprintf(stderr, "%s\n", __FUNCTION__);
    if ((cs = ya->ya_stack) == NULL){
#if 1
	cligen_parseerror1(ya, "No surrounding () or []"); 
	return -1; /* e.g a|b instead of (a|b) */
#else
	cgy_list_delete(&ya->ya_list);
	return 0;
#endif
    }
    cgy_list_delete(&ya->ya_list);
    for (cl = cs->cs_list; cl; cl = cl->cl_next){
	co = cl->cl_obj;
	if (cgy_list_push(co, &ya->ya_list) < 0)
	    return -1;
    }
    return 0;
}

static int
delete_stack_element(struct cgy_stack *cs)
{
    cgy_list_delete(&cs->cs_list);
    cgy_list_delete(&cs->cs_saved);
    free(cs);

    return 0;
}

/*! Pop context from stack and add it to object list
 * Typically done after an option, eg "cmd [opt]"
 * "cmd <push> [opt] <pop>". At pop, all objects saved at push
 * are added to the object list.
 * @param[in]  ya  CLIgen yacc parse struct
 */
static int
ctx_pop_add(cliyacc *ya)
{
    struct cgy_stack *cs; 
    struct cgy_list  *cl; 
    cg_obj           *co; 

    if (debug)
	fprintf(stderr, "%s\n", __FUNCTION__);
    if ((cs = ya->ya_stack) == NULL){
	fprintf(stderr, "%s: cgy_stack empty\n", __FUNCTION__);
	return -1; /* shouldnt happen */
    }
    ya->ya_stack = cs->cs_next;
    /* We could have saved some heap work by moving the cs_list,... */
    for (cl = cs->cs_list; cl; cl = cl->cl_next){
	co = cl->cl_obj;
	if (cgy_list_push(co, &ya->ya_list) < 0)
	    return -1;
    }
    for (cl = cs->cs_saved; cl; cl = cl->cl_next){
	co = cl->cl_obj;
	if (cgy_list_push(co, &ya->ya_list) < 0)
	    return -1;
    }
    delete_stack_element(cs);
    return 0;
}

/*! Pop context from stack and discard it.
 * Typically done after a grouping, eg "cmd (opt1|opt2)"
 * @param[in]  ya  CLIgen yacc parse struct
 */
static int
ctx_pop(cliyacc *ya)
{
    struct cgy_stack *cs; 
    struct cgy_list  *cl; 
    cg_obj           *co; 

    if (debug)
	fprintf(stderr, "%s\n", __FUNCTION__);
    if ((cs = ya->ya_stack) == NULL){
	fprintf(stderr, "%s: cgy_stack empty\n", __FUNCTION__);
	return -1; /* shouldnt happen */
    }
    ya->ya_stack = cs->cs_next;
    for (cl = cs->cs_saved; cl; cl = cl->cl_next){
	co = cl->cl_obj;
	if (cgy_list_push(co, &ya->ya_list) < 0)
	    return -1;
    }
    delete_stack_element(cs);
    return 0;
}

/*!
 * @param[in]  ya  CLIgen yacc parse struct
 */
static int
cg_regexp(cliyacc *ya,
	  char    *rx)
{
    ya->ya_var->co_regex = rx;  
    ya->ya_var->co_vtype=CGV_STRING;
    return 0;
}

/*! Given an optional min and a max, create low and high limits on cv values
 * Supported for ints, decimal64 and strings.
 *  <x:int length[min:max]> or <x:int length[max]> 
 * NOTE: decimal64 fraction-digits must be given before range:
 *   <x:decimal64 fraction-digits:4 range[20.0]>
 * if you want any other fraction-digit than 2
 * @param[in]  ya  CLIgen yacc parse struct
 */
static int
cg_minmax(cliyacc     *ya, 
	  char        *minstr, 
	  char        *maxstr,
	  cg_obj      *yv,
	  enum cv_type cvtype)
{
    int     retval = -1;
    char   *reason = NULL;
    cg_var *cv;
    int     cvret;

    if (minstr != NULL){
	if ((cv = cv_new(cvtype)) == NULL){
	    fprintf(stderr, "cv_new %s\n", strerror(errno));
	    goto done;
	}
	if (cv_name_set(cv, "range_low") == NULL){
	    fprintf(stderr, "cv_name_set %s\n", strerror(errno));
	    goto done;
	}
	if (yv->co_vtype == CGV_DEC64) /* XXX: Seems misplaced? / too specific */
	    cv_dec64_n_set(cv, yv->co_dec64_n);
	if ((cvret = cv_parse1(minstr, cv, &reason)) < 0){
	    fprintf(stderr, "cv_parse1 %s\n", strerror(errno));
	    goto done;
	}
	if (cvret == 0){ /* parsing failed */
	    cligen_parseerror1(ya, reason); 
	    free(reason);
	    goto done;
	}
	yv->co_rangecv_low = cv;
    }
    if ((cv = cv_new(cvtype)) == NULL){
	fprintf(stderr, "cv_new %s\n", strerror(errno));
	goto done;
    }
    if (cv_name_set(cv, "range_high") == NULL){
	fprintf(stderr, "cv_name_set %s\n", strerror(errno));
	goto done;
    }
    if (yv->co_vtype == CGV_DEC64) /* XXX: Seems misplaced? / too specific */
	cv_dec64_n_set(cv, yv->co_dec64_n);
    if ((cvret = cv_parse1(maxstr, cv, &reason)) < 0){
	fprintf(stderr, "cv_parse1 %s\n", strerror(errno));
	goto done;
    }
    if (cvret == 0){ /* parsing failed */
	cligen_parseerror1(ya, reason); 
	free(reason);
	goto done;
    }
    yv->co_rangecv_high = cv;

    ya->ya_var->co_range++;
    retval = 0;
  done:
    return retval;
}

/*! Add Length
 * <x:string length[min:max]> 
 *   Note that the co_range structure fields are re-used for string length restrictions.
 *   but the range type is uint64, not depending on cv type as int:s
 * @param[in]  ya  CLIgen yacc parse struct
 */
static int
cg_length(cliyacc *ya,
	  char    *minstr,
	  char    *maxstr)
{
    cg_obj *yv;

    if ((yv = ya->ya_var) == NULL){
	fprintf(stderr, "No var obj");
	return -1;
    }
    return cg_minmax(ya, minstr, maxstr, yv, CGV_UINT64);
}

/*!
 * @param[in]  ya  CLIgen yacc parse struct
 */
static int
cg_range(cliyacc *ya,
	 char    *minstr,
	 char    *maxstr)
{
    cg_obj *yv;

    if ((yv = ya->ya_var) == NULL){
	fprintf(stderr, "No var obj");
	return -1;
    }
    return cg_minmax(ya, minstr, maxstr, yv, yv->co_vtype);
}

/*!
 * @param[in]  ya  CLIgen yacc parse struct
 */
 static int
cg_dec64_n(cliyacc *ya,
	   char    *fraction_digits)
{
    cg_obj *yv;
    char   *reason = NULL;

    if ((yv = ya->ya_var) == NULL){
	fprintf(stderr, "No var obj");
	return -1;
    }
    if (parse_uint8(fraction_digits, &yv->co_dec64_n, NULL) != 1){
	cligen_parseerror1(ya, reason); 
	return -1;
    }
    return 0;
}

/*!
 * @param[in]  ya  CLIgen yacc parse struct
 */
int
cgy_init(cliyacc *ya,
	 cg_obj  *co_top)
{
    if (debug)
	fprintf(stderr, "%s\n", __FUNCTION__);
    /* Add top-level object */
    if (cgy_list_push(co_top, &ya->ya_list) < 0)
	return -1;
    if (ctx_push(ya) < 0)
	return -1;
    return 0;
}

/*!
 * @param[in]  ya  CLIgen yacc parse struct
 */
int
cgy_exit(cliyacc *ya)
{
    struct cgy_stack *cs; 

    if (debug)
	fprintf(stderr, "%s\n", __FUNCTION__);

    ya->ya_var = NULL;
    cgy_list_delete(&ya->ya_list);
    if((cs = ya->ya_stack) != NULL){
	delete_stack_element(cs);
#if 0
	fprintf(stderr, "%s:%d: error: lacking () or [] at or before: '%s'\n", 
		ya->ya_name,
		ya->ya_linenum,
		ya->ya_parse_string
	    );
	return -1;
#endif
    }
    return 0;
}

%} 
 
%%

file          : lines MY_EOF{if(debug)printf("file->lines\n"); YYACCEPT;} 
              ;

lines        : lines line {
                  if(debug)printf("lines->lines line\n");
                 } 
              |   { if(debug)printf("lines->\n"); } 
              ;

line          : decltop line1	{ if (debug) printf("line->decltop line1\n"); }	
              | assignment ';'  { if (debug) fprintf(stderr, "line->assignment ;\n"); }

              ;

line1        :  line2  { if (debug) printf("line1->line2\n"); }
              |  ',' options line2 { if (debug) printf("line1->',' options line2\n"); }
              ;

line2        : ';' { if (debug) printf("line2->';'\n"); if (cgy_terminal(_ya) < 0) YYERROR;if (ctx_peek_swap2(_ya) < 0) YYERROR; } 
              | '{' {if (ctx_push(_ya) < 0) YYERROR; } 
                lines 
                '}' { if (debug) printf("line2->'{' lines '}'\n");if (ctx_pop(_ya) < 0) YYERROR;if (ctx_peek_swap2(_ya) < 0) YYERROR; }
              | ';' { if (cgy_terminal(_ya) < 0) YYERROR; } 
                '{' { if (ctx_push(_ya) < 0) YYERROR; }
                lines
                '}' { if (debug) printf("line2->';' '{' lines '}'\n");if (ctx_pop(_ya) < 0) YYERROR;if (ctx_peek_swap2(_ya) < 0) YYERROR; }
              ;

options       : options ',' option {if (debug)printf("options->options , option\n");} 
              | option             {if (debug)printf("options->option\n");} 
              ;
option        : callback    {if (debug)printf("option->callback\n");} 
              | flag        {if (debug)printf("option->flag\n");} 
              | assignment  {if (debug)printf("option->assignment\n");} 
              ;


assignment    : NAME '=' DQ charseq DQ {cgy_assignment(_ya, $1,$4);free($1); free($4);}
              ; 

flag          : NAME {cgy_flag(_ya, $1);free($1);}
              ; 

callback   : NAME  {if (cgy_callback(_ya, $1) < 0) YYERROR;} '(' arglist ')'
           ;

arglist    : arglist1
           | 
           ;

arglist1   : arglist1 ',' arg
           | arg
           ;

arg        : typecast arg1 { if ($2 && cgy_callback_arg(_ya, $1, $2) < 0) YYERROR;
		    if ($1) free($1); if ($2) free($2);
              }
           ;

arg1       : DQ DQ { $$=NULL; }
           | DQ charseq DQ { $$=$2; }
           | NAME  { $$=$1; }
           ;

typecast   : '(' NAME ')' { $$ = $2; }
           | { $$ = NULL; }
           ;

decltop        : decllist  {if (debug)fprintf(stderr, "decltop->decllist\n");}
               | declcomp  {if (debug)fprintf(stderr, "decltop->declcomp\n");}
               ;

decllist      : decltop 
                declcomp  {if (debug)fprintf(stderr, "decllist->decltop declcomp\n");}
              | decltop '|' { if (ctx_peek_swap(_ya) < 0) YYERROR;} 
                declcomp  {if (debug)fprintf(stderr, "decllist->decltop | declcomp\n");}
              ;

declcomp      : '(' { if (ctx_push(_ya) < 0) YYERROR; } decltop ')' { if (ctx_pop(_ya) < 0) YYERROR; if (debug)fprintf(stderr, "declcomp->(decltop)\n");}
              | '[' { if (ctx_push(_ya) < 0) YYERROR; } decltop ']' { if (ctx_pop_add(_ya) < 0) YYERROR; }  {if (debug)fprintf(stderr, "declcomp->[decltop]\n");}
              | decl  {if (debug)fprintf(stderr, "declcomp->decl\n");}
              ;

decl        : cmd {if (debug)fprintf(stderr, "decl->cmd\n");}
            | cmd PDQ charseq DQP { if (debug)fprintf(stderr, "decl->cmd (\" comment \")\n");if (cgy_comment(_ya, $3) < 0) YYERROR; free($3);}
            | cmd PDQ DQP { if (debug)fprintf(stderr, "decl->cmd (\"\")\n");}
            ;

cmd         : NAME { if (debug)fprintf(stderr, "cmd->NAME(%s)\n", $1);if (cgy_cmd(_ya, $1) < 0) YYERROR; free($1); } 
            | '@' NAME { if (debug)fprintf(stderr, "cmd->@NAME\n");if (cgy_reference(_ya, $2) < 0) YYERROR; free($2); } 
            | '<' { if ((_YA->ya_var = cgy_var_create(_YA)) == NULL) YYERROR; }
               variable '>'  { if (cgy_var_post(_ya) < 0) YYERROR; }
            ;

variable    : NAME          { if (cgy_var_name_type(_ya, $1, $1)<0) YYERROR; }
            | NAME ':' NAME { if (cgy_var_name_type(_ya, $1, $3)<0) YYERROR; free($3); }
            | NAME ' ' { if (cgy_var_name_type(_ya, $1, $1) < 0) YYERROR; }
              keypairs
	    | NAME ':' NAME ' ' { if (cgy_var_name_type(_ya, $1, $3) < 0) YYERROR; free($3); }
              keypairs
            ;

keypairs    : keypair 
            | keypairs ' ' keypair
            ;

numdec     : NUMBER { $$ = $1; }
           | DECIMAL 
           ;

keypair     : NAME '(' ')' { expand_fn(_ya, $1); }
            | NAME '(' exparglist ')' { expand_fn(_ya, $1); }
            | V_SHOW ':' NAME { 
		 _YA->ya_var->co_show = $3; 
	      }
            | V_RANGE '[' numdec ':' numdec ']' { 
		if (cg_range(_ya, $3, $5) < 0) YYERROR; free($3); free($5); 
	      }
            | V_RANGE '[' numdec ']' { 
		if (cg_range(_ya, NULL, $3) < 0) YYERROR; free($3); 
	      }
            | V_LENGTH '[' NUMBER ':' NUMBER ']' { 
		if (cg_length(_ya, $3, $5) < 0) YYERROR; free($3); free($5); 
	      }
            | V_LENGTH '[' NUMBER ']' { 
		if (cg_length(_ya, NULL, $3) < 0) YYERROR; free($3); 
	      }
            | V_FRACTION_DIGITS ':' NUMBER { 
		if (cg_dec64_n(_ya, $3) < 0) YYERROR; free($3); 
	      }
            | V_CHOICE ':' choices { _YA->ya_var->co_choice = $3; }
            | V_KEYWORD ':' NAME { 
		_YA->ya_var->co_keyword = $3;  
		_YA->ya_var->co_vtype=CGV_STRING; 
	      }
            | V_REGEXP  ':' DQ charseq DQ { cg_regexp(_ya, $4); }
            | V_TRANSLATE ':' NAME '(' ')' { cg_translate(_ya, $3); }
            ;

exparglist : exparglist ',' exparg
           | exparg
           ;

exparg     : DQ DQ
           | DQ charseq DQ { expand_arg(_ya, "string", $2); free($2); }
           ;

exparg     : typecast arg1 { if ($2 && cgy_callback_arg(_ya, $1, $2) < 0) YYERROR;
		    if ($1) free($1); if ($2) free($2);
              }
           ;

choices     : NUMBER { $$ = $1;}
            | NAME { $$ = $1;}
            | DECIMAL { $$ = $1;}
            | choices '|' NUMBER { $$ = cgy_choice_merge(_ya, $1, $3); free($3);}
            | choices '|' NAME { $$ = cgy_choice_merge(_ya, $1, $3); free($3);}
            | choices '|' DECIMAL { $$ = cgy_choice_merge(_ya, $1, $3); free($3);}
            ;

charseq    : charseq CHAR 
              {
		  int len = strlen($1);
		  $$ = realloc($1, len+strlen($2) +1); 
		  sprintf($$+len, "%s", $2); 
		  free($2);
                 }

           | CHAR {$$=$1;}
           ;


%%

