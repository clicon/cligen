/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         cligen_parseparse
#define yylex           cligen_parselex
#define yyerror         cligen_parseerror
#define yydebug         cligen_parsedebug
#define yynerrs         cligen_parsenerrs

#define yylval          cligen_parselval
#define yychar          cligen_parsechar

/* Copy the first part of user declarations.  */



# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "y.tab.h".  */
#ifndef YY_CLIGEN_PARSE_Y_TAB_H_INCLUDED
# define YY_CLIGEN_PARSE_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int cligen_parsedebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    MY_EOF = 258,
    V_RANGE = 259,
    V_LENGTH = 260,
    V_CHOICE = 261,
    V_KEYWORD = 262,
    V_REGEXP = 263,
    V_FRACTION_DIGITS = 264,
    V_SHOW = 265,
    V_TREENAME = 266,
    DOUBLEPARENT = 267,
    DQ = 268,
    DQP = 269,
    PDQ = 270,
    NAME = 271,
    NUMBER = 272,
    DECIMAL = 273,
    CHAR = 274
  };
#endif
/* Tokens.  */
#define MY_EOF 258
#define V_RANGE 259
#define V_LENGTH 260
#define V_CHOICE 261
#define V_KEYWORD 262
#define V_REGEXP 263
#define V_FRACTION_DIGITS 264
#define V_SHOW 265
#define V_TREENAME 266
#define DOUBLEPARENT 267
#define DQ 268
#define DQP 269
#define PDQ 270
#define NAME 271
#define NUMBER 272
#define DECIMAL 273
#define CHAR 274

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{


  int intval;
  char *string;


};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE cligen_parselval;

int cligen_parseparse (void *_ya);

#endif /* !YY_CLIGEN_PARSE_Y_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */


/* Here starts user C-code */

/* typecast macro */
#define _YA ((struct cligen_parse_yacc_arg *)_ya)

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


/* 
   cligen_parseerror
   also called from yacc generated code *
*/
void cligen_parseerror(void *_ya, char *s) 
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

/* 
 * create_cv
 * Create a CLIgen variable (cv) and store it in the current variable object 
 * Note that only one such cv can be stored.
 */
static cg_var *
create_cv(struct cligen_parse_yacc_arg *ya, char *type, char *str)
{
    cg_var             *cv = NULL;

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

/*
 */
static int
cgy_flag(struct cligen_parse_yacc_arg *ya, char *var)
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

/*! Set a new treename. This is fact registers the previous tree and creates a new 
 * Note that one could have used an assignment: treename = <name>; for this but
 * I decided to create special syntax for this so that assignments can use any
 * variable names.
 */
static int
cgy_treename(struct cligen_parse_yacc_arg *ya, char *name)
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

/*
 * Only string type supported for now
 */
static int
cgy_assignment(struct cligen_parse_yacc_arg *ya, char *var, char *val)
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

int
cgy_callback(struct cligen_parse_yacc_arg *ya, char *cb_str)
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

/* 
 * cgy_callback_arg
 * Create a CLIgen variable (cv) and store it in the YA struct.
 * Note that only one such cv can be stored.
 */
static int
cgy_callback_arg(struct cligen_parse_yacc_arg *ya, char *type, char *arg)
{
    int                 retval = -1;
    struct cg_callback *cc;
    struct cg_callback *cclast;

    cclast = NULL;
    for (cc=ya->ya_callbacks; cc; cc=cc->cc_next)
	cclast = cc;
    if (cclast)
	if ((cclast->cc_arg = create_cv(ya, type, arg)) == NULL)
	    goto done;
    retval = 0;
  done:
    return retval;
}

static int
expand_arg(struct cligen_parse_yacc_arg *ya, char *type, char *arg)
{
    int                 retval = -1;
    cg_var             *cgv;

    if ((cgv = create_cv(ya, type, arg)) == NULL)
	goto done;
    ya->ya_var->co_expand_fn_arg = cgv;
    retval = 0;
  done:
    return retval;
}

static int
cgy_list_push(cg_obj *co, struct cgy_list **cl0)
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

/* Delet whole list */
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

/* 
 * Create new tmp variable cligen object 
 * It must be filled in by later functions.
 * The reason is, the variable must be completely parsed by successive syntax
 * (eg <type:> stuff) and we cant insert it into the object tree until that is done.
 * And completed by the '_post' function
 * Returns: tmp variable object
 */
static cg_obj *
cgy_var_pre(struct cligen_parse_yacc_arg *ya)
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

/* 
 * Complete variable cligen object after parsing is complete and insert it into
 * object hierarchies. That is, insert a variable in each hieracrhy.
 * Returns:
 *       new object or sister that is placed in the hierarchy.
 * Arguments:
 *       cv     - tmp variable object
 */
static int
cgy_var_post(struct cligen_parse_yacc_arg *ya)
{
    struct cgy_list *cl; 
    cg_obj *cv1; /* variable copy object */
    cg_obj *cop; /* parent */
    cg_obj *co;  /* new obj/sister */
    cg_obj *cv = ya->ya_var;

#if 0
    if (cv->co_vtype == CGV_ERR) /* unassigned */
	cv->co_vtype = cv_str2type(cv->co_command);
#endif
    if (debug)
	fprintf(stderr, "%s: cmd:%s vtype:%d\n", __FUNCTION__, 
		cv->co_command,
		cv->co_vtype );
    if (cv->co_vtype == CGV_ERR){
	cligen_parseerror1(ya, "Wrong or unassigned variable type"); 	
	return -1;
    }
#if 0 /* XXX dont really know what i am doing but variables dont behave nice in choice */
    if (ya->ya_opt){     /* get cop from stack */
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
	cop = cl->cl_obj;
	if (cl->cl_next){
	    if (co_copy(cv, cop, &cv1) < 0)
		return -1;
	}
	else
	    cv1 = cv; /* Dont copy if last in list */
	co_up_set(cv1, cop);
	if ((co = co_insert(&cop->co_pt, cv1)) == NULL) /* co_new may be deleted */
	    return -1;
	cl->cl_obj = co;
    }
    return 0;
}

/*
 * Create a new command object. Actually, create a new for every tree in the list
 * and replace the old with the new object.
 * Returns:
 *  -1 on error 0 on OK
 * Arguments:
 *   cmd: the command string
 */
static int
cgy_cmd(struct cligen_parse_yacc_arg *ya, char *cmd)
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

/* 
   cgy_reference
   Create a REFERENCE node that references another tree.
   This is evaluated in runtime by pt_expand().
   See also db2tree() in clicon/apps/cli_main.c on how to create such a tree
   And pt_expand_1()/pt_callback_reference() how it is expanded
   And
 */
static int
cgy_reference(struct cligen_parse_yacc_arg *ya, char *name)
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
	if (co_insert(&cop->co_pt, cot) == NULL)  /* co_new may be deleted */
	    return -1;
	/* Replace parent in cgy_list: not allowed after ref?
	   but only way to add callbacks to it.
	*/
	cl->cl_obj = cot;
   }
 
    return 0;
}


/* assume comment is malloced and not freed by parser */
static int
cgy_comment(struct cligen_parse_yacc_arg *ya, char *comment)
{
    struct cgy_list *cl; 
    cg_obj *co; 

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

static char *
cgy_choice_merge(struct cligen_parse_yacc_arg *ya, char *str, char *app)
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

/*
 * Post-processing of commands, eg at ';':
 *  a cmd;<--
 * But only if parsing succesful.
 * 1. Add callback and args to every list
 * 2. Add empty child unless already empty child
 */
int
cgy_terminal(struct cligen_parse_yacc_arg *ya)
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
	    if (co_callback_copy(ya->ya_callbacks, ccp, NULL) < 0)
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
	if (cc->cc_arg)	
	    cv_free(cc->cc_arg);
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

/*
 * Take the whole cgy_list and push it to the stack 
 */
static int
ctx_push(struct cligen_parse_yacc_arg *ya)
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

/*
 * ctx_peek_swap
 * Peek context from stack and replace the object list with it
 * Typically done in a choice, eg "(c1|c2)" at c2.
 * Dont pop context
  */
static int
ctx_peek_swap(struct cligen_parse_yacc_arg *ya)
{
    struct cgy_stack *cs; 
    struct cgy_list *cl; 
    cg_obj *co; 

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

/*
 * ctx_peek_swap2
 * Peek context from stack and replace the object list with it
 * Typically done in a choice, eg "(c1|c2)" at c2.
 * Dont pop context
  */
static int
ctx_peek_swap2(struct cligen_parse_yacc_arg *ya)
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

/*
 * ctx_pop_add
 * Pop context from stack and add it to object list
 * Typically done after an option, eg "cmd [opt]"
 * "cmd <push> [opt] <pop>". At pop, all objects saved at push
 * are added to the object list.
  */
static int
ctx_pop_add(struct cligen_parse_yacc_arg *ya)
{
    struct cgy_stack *cs; 
    struct cgy_list *cl; 
    cg_obj *co; 

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

/*
 * ctx_pop
 * Pop context from stack and discard it.
 * Typically done after a grouping, eg "cmd (opt1|opt2)"
 */
static int
ctx_pop(struct cligen_parse_yacc_arg *ya)
{
    struct cgy_stack *cs; 
    struct cgy_list *cl; 
    cg_obj *co; 

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

static int
cg_regexp(struct cligen_parse_yacc_arg *ya, char *rx)
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
 */
static int
cg_minmax(struct cligen_parse_yacc_arg *ya, 
	  char                         *minstr, 
	  char                         *maxstr,
	  cg_obj                       *yv,
	  enum cv_type                  cvtype)
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

/* <x:string length[min:max]> 
   Note that the co_range structure fields are re-used for string length restrictions.
   but the range type is uint64, not depending on cv type as int:s
 */
static int
cg_length(struct cligen_parse_yacc_arg *ya, char *minstr, char *maxstr)
{
    cg_obj *yv;

    if ((yv = ya->ya_var) == NULL){
	fprintf(stderr, "No var obj");
	return -1;
    }
    return cg_minmax(ya, minstr, maxstr, yv, CGV_UINT64);
}

static int
cg_range(struct cligen_parse_yacc_arg *ya, char *minstr, char *maxstr)
{
    cg_obj *yv;

    if ((yv = ya->ya_var) == NULL){
	fprintf(stderr, "No var obj");
	return -1;
    }
    return cg_minmax(ya, minstr, maxstr, yv, yv->co_vtype);
}


static int
cg_dec64_n(struct cligen_parse_yacc_arg *ya, char *fraction_digits)
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

int
cgy_init(struct cligen_parse_yacc_arg *ya, cg_obj *co_top)
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

int
cgy_exit(struct cligen_parse_yacc_arg *ya)
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




#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   166

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  35
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  34
/* YYNRULES -- Number of rules.  */
#define YYNRULES  78
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  149

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   274

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,    34,     2,     2,     2,     2,     2,     2,     2,
      25,    26,     2,     2,    21,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    33,    20,
      31,    24,    32,     2,    30,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    28,     2,    29,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    22,    27,    23,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   954,   954,   957,   960,   963,   964,   968,   969,   972,
     973,   973,   976,   977,   976,   982,   983,   985,   986,   987,
     991,   994,   997,   997,  1000,  1001,  1002,  1006,  1007,  1011,
    1012,  1019,  1020,  1023,  1025,  1025,  1029,  1029,  1030,  1030,
    1030,  1031,  1034,  1035,  1036,  1039,  1040,  1041,  1041,  1045,
    1048,  1052,  1052,  1056,  1056,  1062,  1063,  1066,  1067,  1070,
    1071,  1072,  1077,  1080,  1083,  1086,  1089,  1092,  1095,  1096,
    1100,  1103,  1104,  1105,  1106,  1107,  1108,  1111,  1119
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "MY_EOF", "V_RANGE", "V_LENGTH",
  "V_CHOICE", "V_KEYWORD", "V_REGEXP", "V_FRACTION_DIGITS", "V_SHOW",
  "V_TREENAME", "DOUBLEPARENT", "DQ", "DQP", "PDQ", "NAME", "NUMBER",
  "DECIMAL", "CHAR", "';'", "','", "'{'", "'}'", "'='", "'('", "')'",
  "'|'", "'['", "']'", "'@'", "'<'", "'>'", "':'", "' '", "$accept",
  "file", "lines", "line", "line1", "line2", "$@1", "$@2", "$@3",
  "options", "option", "assignment", "flag", "callback", "$@4", "arg",
  "decltop", "decllist", "$@5", "declcomp", "$@6", "$@7", "$@8", "decl",
  "cmd", "$@9", "variable", "$@10", "$@11", "keypairs", "numdec",
  "keypair", "choices", "charseq", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
      59,    44,   123,   125,    61,    40,    41,   124,    91,    93,
      64,    60,    62,    58,    32
};
# endif

#define YYPACT_NINF -40

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-40)))

#define YYTABLE_NINF -23

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     -40,     7,    -1,   -40,   -40,   -15,   -40,   -40,     1,   -40,
     -40,     6,    16,   -40,   -40,   -40,    18,    27,    59,    59,
     -40,    26,   -40,   -40,    38,    49,   -40,   -40,   -40,   -40,
     -40,    42,    58,    23,    39,    40,    61,    69,   102,    97,
     -40,   -40,   -40,   -40,   -40,    59,   -40,   -40,    62,    -7,
     -40,   -40,    79,   -40,   -40,   -40,    82,    49,   -40,   -12,
     -40,   -40,   -40,   -40,   -40,    98,    94,   -40,    -5,   -40,
     -40,   -40,   -40,    85,    88,   100,   101,   103,   105,   106,
     110,   108,   -40,    55,    12,   121,   -40,    94,   111,   123,
     104,   125,   130,   127,   129,    32,    94,   -40,   120,    44,
     122,   108,   -40,   -40,   -19,    63,   -40,   -40,   -40,   124,
     -40,    58,   -40,   -40,    75,   -40,   -40,   -40,   126,    46,
     -40,   111,   -40,   132,   107,    92,   128,    93,   -40,    95,
     131,   -40,   118,   133,   -40,   -40,   -40,   -40,   -40,   134,
     135,    96,   -40,   -40,   -40,   -40,   -40,   137,   -40
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       4,     0,     0,     1,     2,    45,    36,    38,     0,    47,
       3,     0,     0,    31,    32,    41,    42,     0,     0,     0,
      46,     0,     6,    45,     9,     0,    10,    34,     5,     7,
      33,     0,     0,     0,     0,    49,     0,     0,    21,     0,
      16,    19,    18,    17,     4,     0,    44,    78,     0,     0,
      37,    39,     0,    51,    48,    13,     0,     0,     8,     0,
      35,    43,    77,    20,    40,    50,     0,     4,     0,    23,
      15,    11,    53,     0,     0,     0,     0,     0,     0,     0,
       0,    52,    55,     0,     0,     0,    24,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    14,     0,     0,
       0,    54,    57,    58,     0,     0,    72,    71,    73,    68,
      69,     0,    67,    62,     0,    59,    56,    25,     0,     0,
      64,     0,    66,     0,     0,     0,     0,     0,    26,     0,
       0,    27,     0,     0,    75,    74,    76,    70,    60,     0,
       0,     0,    28,    63,    65,    61,    29,     0,    30
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -40,   -40,   -39,   -40,   -40,   114,   -40,   -40,   -40,   -40,
      99,   -22,   -40,   -40,   -40,   -40,   112,   -40,   -40,   -11,
     -40,   -40,   -40,   -40,   -40,   -40,   -40,   -40,   -40,    68,
      29,    70,   -40,   -32
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     1,     2,    10,    28,    29,    44,    37,    67,    39,
      40,    11,    42,    43,    56,    69,    12,    13,    45,    14,
      18,    19,    64,    15,    16,    21,    36,    66,    87,    81,
     104,    82,   109,    48
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      49,    30,     4,    41,     5,    59,    63,     3,    84,    17,
     120,    71,    62,     6,   121,     5,     7,    20,     8,     9,
      85,    86,    30,    30,     6,    98,    22,     7,    83,     8,
       9,    47,    23,    31,    60,    41,    24,    25,    26,    23,
      32,     6,    35,    27,     7,   114,     8,     9,     6,    50,
      27,     7,    99,     8,     9,    23,    46,   118,   115,   129,
     -12,    47,   130,    62,     6,    38,    27,     7,    51,     8,
       9,     5,   131,    52,    53,    23,    61,    47,    97,   125,
       6,    62,   127,     7,     6,     8,     9,     7,   126,     8,
       9,    55,   122,    54,    47,    65,   123,   141,    73,    74,
      75,    76,    77,    78,    79,   137,   139,    68,   140,   147,
      80,    62,    62,    88,    47,    62,    89,    24,    57,    26,
     106,   107,   108,   134,   135,   136,    17,   -22,   102,   103,
      33,    34,    72,    90,    91,    95,    92,   100,    93,    94,
     105,   110,    96,   111,   112,   113,   117,   143,   119,   133,
     132,   124,   128,    58,   138,   101,    70,   142,     0,     0,
     145,   146,   144,   148,     0,     0,   116
};

static const yytype_int16 yycheck[] =
{
      32,    12,     3,    25,    16,    44,    13,     0,    13,    24,
      29,    23,    19,    25,    33,    16,    28,    16,    30,    31,
      25,    26,    33,    34,    25,    13,    20,    28,    67,    30,
      31,    19,    16,    15,    45,    57,    20,    21,    22,    16,
      13,    25,    16,    27,    28,    13,    30,    31,    25,    26,
      27,    28,    84,    30,    31,    16,    14,    13,    26,    13,
      22,    19,    16,    19,    25,    16,    27,    28,    29,    30,
      31,    16,    26,    33,    34,    16,    14,    19,    23,   111,
      25,    19,   114,    28,    25,    30,    31,    28,    13,    30,
      31,    22,    29,    32,    19,    16,    33,   129,     4,     5,
       6,     7,     8,     9,    10,    13,    13,    25,    13,    13,
      16,    19,    19,    28,    19,    19,    28,    20,    21,    22,
      16,    17,    18,    16,    17,    18,    24,    25,    17,    18,
      18,    19,    34,    33,    33,    25,    33,    16,    33,    33,
      17,    16,    34,    13,    17,    16,    26,    29,    26,    17,
     121,    27,    26,    39,    26,    87,    57,    26,    -1,    -1,
      26,    26,    29,    26,    -1,    -1,    96
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    36,    37,     0,     3,    16,    25,    28,    30,    31,
      38,    46,    51,    52,    54,    58,    59,    24,    55,    56,
      16,    60,    20,    16,    20,    21,    22,    27,    39,    40,
      54,    15,    13,    51,    51,    16,    61,    42,    16,    44,
      45,    46,    47,    48,    41,    53,    14,    19,    68,    68,
      26,    29,    33,    34,    32,    22,    49,    21,    40,    37,
      54,    14,    19,    13,    57,    16,    62,    43,    25,    50,
      45,    23,    34,     4,     5,     6,     7,     8,     9,    10,
      16,    64,    66,    37,    13,    25,    26,    63,    28,    28,
      33,    33,    33,    33,    33,    25,    34,    23,    13,    68,
      16,    64,    17,    18,    65,    17,    16,    17,    18,    67,
      16,    13,    17,    16,    13,    26,    66,    26,    13,    26,
      29,    33,    29,    33,    27,    68,    13,    68,    26,    13,
      16,    26,    65,    17,    16,    17,    18,    13,    26,    13,
      13,    68,    26,    29,    29,    26,    26,    13,    26
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    35,    36,    37,    37,    38,    38,    39,    39,    40,
      41,    40,    42,    43,    40,    44,    44,    45,    45,    45,
      46,    47,    49,    48,    50,    50,    50,    50,    50,    50,
      50,    51,    51,    52,    53,    52,    55,    54,    56,    57,
      54,    54,    58,    58,    58,    59,    59,    60,    59,    61,
      61,    62,    61,    63,    61,    64,    64,    65,    65,    66,
      66,    66,    66,    66,    66,    66,    66,    66,    66,    66,
      66,    67,    67,    67,    67,    67,    67,    68,    68
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     2,     0,     2,     2,     1,     3,     1,
       0,     4,     0,     0,     6,     3,     1,     1,     1,     1,
       5,     1,     0,     3,     2,     4,     5,     5,     6,     7,
       8,     1,     1,     2,     0,     4,     0,     4,     0,     0,
       5,     1,     1,     4,     3,     1,     2,     0,     4,     1,
       3,     0,     4,     0,     6,     1,     3,     1,     1,     3,
       5,     6,     3,     6,     4,     6,     4,     3,     3,     3,
       5,     1,     1,     1,     3,     3,     3,     2,     1
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (_ya, YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, _ya); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, void *_ya)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  YYUSE (_ya);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, void *_ya)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep, _ya);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule, void *_ya)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              , _ya);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule, _ya); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, void *_ya)
{
  YYUSE (yyvaluep);
  YYUSE (_ya);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void *_ya)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);

        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex (_ya);
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

    {if(debug)printf("file->lines\n"); YYACCEPT;}

    break;

  case 3:

    {
                  if(debug)printf("lines->lines line\n");
                 }

    break;

  case 4:

    { if(debug)printf("lines->\n"); }

    break;

  case 5:

    { if (debug) printf("line->decltop line1\n"); }

    break;

  case 6:

    { if (debug) fprintf(stderr, "line->assignment ;\n"); }

    break;

  case 7:

    { if (debug) printf("line1->line2\n"); }

    break;

  case 8:

    { if (debug) printf("line1->',' options line2\n"); }

    break;

  case 9:

    { if (debug) printf("line2->';'\n"); if (cgy_terminal(_ya) < 0) YYERROR;if (ctx_peek_swap2(_ya) < 0) YYERROR; }

    break;

  case 10:

    {if (ctx_push(_ya) < 0) YYERROR; }

    break;

  case 11:

    { if (debug) printf("line2->'{' lines '}'\n");if (ctx_pop(_ya) < 0) YYERROR;if (ctx_peek_swap2(_ya) < 0) YYERROR; }

    break;

  case 12:

    { if (cgy_terminal(_ya) < 0) YYERROR; }

    break;

  case 13:

    { if (ctx_push(_ya) < 0) YYERROR; }

    break;

  case 14:

    { if (debug) printf("line2->';' '{' lines '}'\n");if (ctx_pop(_ya) < 0) YYERROR;if (ctx_peek_swap2(_ya) < 0) YYERROR; }

    break;

  case 15:

    {if (debug)printf("terminal->callback\n");}

    break;

  case 17:

    {if (debug)printf("option->callback\n");}

    break;

  case 18:

    {if (debug)printf("option->flag\n");}

    break;

  case 19:

    {if (debug)printf("option->assignment\n");}

    break;

  case 20:

    {cgy_assignment(_ya, (yyvsp[-4].string),(yyvsp[-1].string));free((yyvsp[-4].string)); free((yyvsp[-1].string));}

    break;

  case 21:

    {cgy_flag(_ya, (yyvsp[0].string));free((yyvsp[0].string));}

    break;

  case 22:

    {if (cgy_callback(_ya, (yyvsp[0].string)) < 0) YYERROR;}

    break;

  case 26:

    { if (cgy_callback_arg(_ya, NULL, (yyvsp[-2].string)) < 0) YYERROR;
                free((yyvsp[-2].string));
              }

    break;

  case 27:

    { free((yyvsp[-2].string)); }

    break;

  case 28:

    { if (cgy_callback_arg(_ya, (yyvsp[-3].string), (yyvsp[-1].string)) < 0) YYERROR;
		  free((yyvsp[-3].string));free((yyvsp[-1].string));
              }

    break;

  case 29:

    { free((yyvsp[-4].string)); }

    break;

  case 30:

    {   if (strcmp("string", (yyvsp[-5].string))) YYERROR; 
                if (cgy_callback_arg(_ya, (yyvsp[-5].string), (yyvsp[-2].string)) < 0) YYERROR;
                free((yyvsp[-5].string));free((yyvsp[-2].string));
              }

    break;

  case 31:

    {if (debug)fprintf(stderr, "decltop->decllist\n");}

    break;

  case 32:

    {if (debug)fprintf(stderr, "decltop->declcomp\n");}

    break;

  case 33:

    {if (debug)fprintf(stderr, "decllist->decltop declcomp\n");}

    break;

  case 34:

    { if (ctx_peek_swap(_ya) < 0) YYERROR;}

    break;

  case 35:

    {if (debug)fprintf(stderr, "decllist->decltop | declcomp\n");}

    break;

  case 36:

    { if (ctx_push(_ya) < 0) YYERROR; }

    break;

  case 37:

    { if (ctx_pop(_ya) < 0) YYERROR; if (debug)fprintf(stderr, "declcomp->(decltop)\n");}

    break;

  case 38:

    { if (ctx_push(_ya) < 0) YYERROR; }

    break;

  case 39:

    { if (ctx_pop_add(_ya) < 0) YYERROR; }

    break;

  case 40:

    {if (debug)fprintf(stderr, "declcomp->[decltop]\n");}

    break;

  case 41:

    {if (debug)fprintf(stderr, "declcomp->decl\n");}

    break;

  case 42:

    {if (debug)fprintf(stderr, "decl->cmd\n");}

    break;

  case 43:

    { if (debug)fprintf(stderr, "decl->cmd (\" comment \")\n");if (cgy_comment(_ya, (yyvsp[-1].string)) < 0) YYERROR; free((yyvsp[-1].string));}

    break;

  case 44:

    { if (debug)fprintf(stderr, "decl->cmd (\"\")\n");}

    break;

  case 45:

    { if (debug)fprintf(stderr, "cmd->NAME(%s)\n", (yyvsp[0].string));if (cgy_cmd(_ya, (yyvsp[0].string)) < 0) YYERROR; free((yyvsp[0].string)); }

    break;

  case 46:

    { if (debug)fprintf(stderr, "cmd->@NAME\n");if (cgy_reference(_ya, (yyvsp[0].string)) < 0) YYERROR; free((yyvsp[0].string)); }

    break;

  case 47:

    { if ((_YA->ya_var = cgy_var_pre(_YA)) == NULL) YYERROR; }

    break;

  case 48:

    { if (cgy_var_post(_ya) < 0) YYERROR; }

    break;

  case 49:

    { 
                _YA->ya_var->co_command = (yyvsp[0].string); 
		_YA->ya_var->co_vtype = cv_str2type((yyvsp[0].string)); }

    break;

  case 50:

    { 
		_YA->ya_var->co_command = (yyvsp[-2].string); 
		_YA->ya_var->co_vtype = cv_str2type((yyvsp[0].string)); free((yyvsp[0].string));
	       }

    break;

  case 51:

    { 
		_YA->ya_var->co_command = (yyvsp[-1].string); 
		_YA->ya_var->co_vtype = cv_str2type((yyvsp[-1].string)); }

    break;

  case 53:

    { 
		 _YA->ya_var->co_command = (yyvsp[-3].string); 
		 _YA->ya_var->co_vtype = cv_str2type((yyvsp[-1].string)); free((yyvsp[-1].string)); }

    break;

  case 57:

    { (yyval.string) = (yyvsp[0].string); }

    break;

  case 59:

    { _YA->ya_var->co_expand_fn_str = (yyvsp[-2].string); }

    break;

  case 60:

    {_YA->ya_var->co_expand_fn_str = (yyvsp[-4].string); }

    break;

  case 61:

    {
		_YA->ya_var->co_expand_fn_str = (yyvsp[-5].string); 
		expand_arg(_ya, "string", (yyvsp[-2].string));
		free((yyvsp[-2].string)); 
	      }

    break;

  case 62:

    { 
		 _YA->ya_var->co_show = (yyvsp[0].string); 
	      }

    break;

  case 63:

    { 
		if (cg_range(_ya, (yyvsp[-3].string), (yyvsp[-1].string)) < 0) YYERROR; free((yyvsp[-3].string)); free((yyvsp[-1].string)); 
	      }

    break;

  case 64:

    { 
		if (cg_range(_ya, NULL, (yyvsp[-1].string)) < 0) YYERROR; free((yyvsp[-1].string)); 
	      }

    break;

  case 65:

    { 
		if (cg_length(_ya, (yyvsp[-3].string), (yyvsp[-1].string)) < 0) YYERROR; free((yyvsp[-3].string)); free((yyvsp[-1].string)); 
	      }

    break;

  case 66:

    { 
		if (cg_length(_ya, NULL, (yyvsp[-1].string)) < 0) YYERROR; free((yyvsp[-1].string)); 
	      }

    break;

  case 67:

    { 
		if (cg_dec64_n(_ya, (yyvsp[0].string)) < 0) YYERROR; free((yyvsp[0].string)); 
	      }

    break;

  case 68:

    { _YA->ya_var->co_choice = (yyvsp[0].string); }

    break;

  case 69:

    { 
		_YA->ya_var->co_keyword = (yyvsp[0].string);  
		_YA->ya_var->co_vtype=CGV_STRING; 
	      }

    break;

  case 70:

    { cg_regexp(_ya, (yyvsp[-1].string)); }

    break;

  case 71:

    { (yyval.string) = (yyvsp[0].string);}

    break;

  case 72:

    { (yyval.string) = (yyvsp[0].string);}

    break;

  case 73:

    { (yyval.string) = (yyvsp[0].string);}

    break;

  case 74:

    { (yyval.string) = cgy_choice_merge(_ya, (yyvsp[-2].string), (yyvsp[0].string)); free((yyvsp[0].string));}

    break;

  case 75:

    { (yyval.string) = cgy_choice_merge(_ya, (yyvsp[-2].string), (yyvsp[0].string)); free((yyvsp[0].string));}

    break;

  case 76:

    { (yyval.string) = cgy_choice_merge(_ya, (yyvsp[-2].string), (yyvsp[0].string)); free((yyvsp[0].string));}

    break;

  case 77:

    {
		  int len = strlen((yyvsp[-1].string));
		  (yyval.string) = realloc((yyvsp[-1].string), len+strlen((yyvsp[0].string)) +1); 
		  sprintf((yyval.string)+len, "%s", (yyvsp[0].string)); 
		  free((yyvsp[0].string));
                 }

    break;

  case 78:

    {(yyval.string)=(yyvsp[0].string);}

    break;



      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (_ya, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (_ya, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, _ya);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp, _ya);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (_ya, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, _ya);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp, _ya);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}



