/*
  CVS Version: $Id: cligen_gen.h,v 1.52 2013/06/16 12:00:55 olof Exp $ 

  Copyright (C) 2001-2013 Olof Hagsand

  This file is part of CLIgen.

  CLIgen is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  CLIgen is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with CLIgen; see the file COPYING.
*/

#ifndef _CLIGEN_GEN_H_
#define _CLIGEN_GEN_H_

#define CLIGEN_DELIMITERS " \t"
#define CLIGEN_QUOTES     "\""

/*
 * Parse tree nodes. The different types are associated to
 * different fields in the parse tree node object, each interpreted/parsed
 * differently.
 */
enum cg_objtype{
  CO_COMMAND,   /* Simple string parsing */
  CO_VARIABLE,  /* parse as type eg int */
  CO_REFERENCE  /* Symbolic reference to other tree */
};

/*
 * Callback function type. Is called after a specific syntax node has been identified.,
 *   arg is an optionalargument
 *   argc is number of variables (1...)
 *   argv[] is a vector of variables. The first is always the whole syntax string as entered.
 */
typedef int (cg_fnstype_t)(cligen_handle h, cvec *vars, cg_var *arg);

/* Expand callback function (should be in cligen_expand.h) 
   Returns 0 if handled expand, that is, it returned commands for 'name'
           1 if did not handle expand 
          -1 on error.
*/
typedef int (expand_cb)(cligen_handle h,      /* handler: cligen or userhandle */
			char *name,           /* name of this function (in text) */
			cvec *cvec,           /* vars vector of values in command */
			cg_var *arg,          /* argument given to callback */
			int *len,             /* len of return commands & helptxt */
			char ***commands,     /* vector of function strings */
			char ***helptexts);   /* vector of help-texts */

/* expand_cb2 is an update of expand_cb where entries are added using
 * cvec_add rather than by realloc(). Just a better interface.
 * should be merged into new applications.
 */
#ifdef notyet
typedef int (expand_cb2)(void *h,              /* handler: cligen or userhandle */
			 char *name,           /* name of this function (in text) */
			 cvec *cvec,           /* vars vector of values in command */
			 cg_var *arg,          /* argument given to callback */
			 struct cvec *commands,/* vector of commands */
			 struct cvec *helptexts); /* vector of help-texts */
#endif

typedef int (cligen_susp_cb_t)(void *h, char *, int, int *);

/*! A parse tree is a top object containing a vector of parse-tree nodes 
 *
 * @code
 *      o
 *      ^
 *      |
 *      up
 *   [0 1..n] <--- parse-tree
 *    | |  |
 *    v v  v
 *    o o  o
 * @endcode
 */
struct parse_tree{
    struct cg_obj     **pt_vec;    /**< vector of pointers to parse-tree nodes */
    int                 pt_len;    /**< length of vector */
    struct parse_tree  *pt_up;     /**< parent cligen object, if any */
    int                 pt_obj;    /**< if this parse-tree is a part of a cg_obj (can be 
				      typecast to (cg_obj*). See co_up() below */
};
typedef struct parse_tree parse_tree;

/*! A CLIgen object may have one or several callbacks. This type defines one callback
 */
struct cg_callback  { /* Linked list of command callbacks */
    struct  cg_callback *cc_next;    /**< Next callback in list.  */
    cg_fnstype_t        *cc_fn;      /**< callback/function pointer.  */
    char                *cc_fn_str;  /**< callback/function name. malloced */
    cg_var              *cc_arg;     /**< callback/function argument */
};

/*
 * If cligen object is a variable, this is its variable spec.
 * But it is not complete, it is a part of a cg_obj.
 * A cg_var is the value of such a varspec.
 */
struct cg_varspec{
    enum cv_type    cgs_vtype;         /* its type */
    char           *cgs_expand_fn_str; /* expand callback string */
    expand_cb      *cgs_expand_fn;     /* expand callback */
    cg_var         *cgs_expand_fn_arg; /* expand callback arg */
    char           *cgs_choice;        /* list of choices */
    int             cgs_range;         /* range interval valid */
    int64_t         cgs_range_low;     /* range interval lower limit*/
    int64_t         cgs_range_high;    /* range interval upper limit*/
    char           *cgs_regex;         /* regular expression */
    uint8_t         cgs_dec64_n;       /* negative decimal exponential 1..18 */
};
typedef struct cg_varspec cg_varspec;

/* Default number of fraction digits if type is DEC64 */
#define CGV_DEC64_N_DEFAULT 2

/*! cligen object is a parse-tree node. A cg_obj is either a command or a variable
 * A cg_obj 
 * @code
 *      o <--- cg_obj
 *      ^
 *      |
 *      up
 *   [0 1..n]
 *    | |  |
 *    v v  v
 *    o o  o   <--- cg_obj
 * @endcode
 */
struct cg_obj{
    parse_tree        co_pt;        /* Child parse-tree (see co_next macro below) */
    struct cg_obj    *co_prev;
    enum cg_objtype   co_type;      /* Type of object */
    char              co_delimiter; /* Delimiter (after this obj) */
    char             *co_command;   /* malloc:ed matching string / name or type */
    struct cg_callback *co_callbacks;/* linked list of callbacks and arguments */
    cvec             *co_cvec;       /* List of cligen variables (XXX: not visible to callbacks) */
    int               co_mark;      /* Only used internally (for recursion avoidance) */
    char	     *co_help;	    /* Brief helptext */
#ifdef notused
    int	              co_top;	    /* Top of syntax tree: use as virtual top */
#endif
    int	              co_hide;      /* Don't show in help/completion */
    char             *co_mode;      /* Name of other syntax mode */

    /* Expand data: expand, choice temporarily replaces the original parse-tree
       with one where expand and choice is replaced by string constants. */
    struct parse_tree co_pt_exp;   /* Expanded parse-trees (why > 1?) */
    struct cg_obj    *co_ref;       /* Ref to original (if this is expanded) */
    char             *co_value;     /* Expanded value can be a string with a 
				       constant. Store the constant in the original
				       variable. */
    void             *co_userdata;  /* User-specific data, malloced and defined by
				       the user. Will be freed by cligen on exit */
    size_t            co_userlen;   /* Length of the userdata (need copying) */

#ifdef notyet
    struct cg_var    *co_cv;        /* Store value directly at matching */
#endif
    int               co_treeref;   /* This node is top of expanded sub-tree */
    int               co_refdone;   /* This reference has already been expanded */
    /* Experiment to make the junos edit style work. When we graft the tree,
       these places should insert extra NULL child to be able to edit.
     */
    int               co_nonterminal; /* Set if we should be able to <CR> here */

    /* Ugly application code */
    struct parse_tree co_pt_push;   /* Saved orig of parse-tree (ugly user code) */
    int               co_pushed;    /* if pushed contains data */


    union {
	struct {        } cou_cmd;
	struct cg_varspec cou_var;
    } u;
};
typedef struct cg_obj cg_obj; /* in cli_var.h */

typedef cg_obj** pt_vec;  /* vector of (pointers to) parse-tree nodes */

/* Callback for pt_apply() */
typedef int (cg_applyfn_t)(cg_obj *co, void *arg);

/* Access macro to cligen object variable specification */
#define co2varspec(co)  &(co)->u.cou_var

/* Access fields for code traversing parse tree */
#define co_next          co_pt.pt_vec
#define co_max           co_pt.pt_len
#define co_vtype         u.cou_var.cgs_vtype
#define co_expand_fn  	 u.cou_var.cgs_expand_fn
#define co_expand_fn_str u.cou_var.cgs_expand_fn_str
#define co_expand_fn_arg u.cou_var.cgs_expand_fn_arg
#define co_choice	 u.cou_var.cgs_choice
#define co_keyword	 u.cou_var.cgs_choice
#define co_range	 u.cou_var.cgs_range
#define co_range_low	 u.cou_var.cgs_range_low
#define co_range_high	 u.cou_var.cgs_range_high
#define co_regex         u.cou_var.cgs_regex
#define co_dec64_n       u.cou_var.cgs_dec64_n

#define iskeyword(CV) ((CV)->co_choice!=NULL && strchr((CV)->co_choice, '|')==NULL)

/* tmp Access macro */

#if 1

static inline cg_obj* 
co_up(cg_obj *co) 
{
    return co->co_prev;
}

static inline int
co_up_set(cg_obj *co, cg_obj *cop) 
{
    co->co_prev = cop;
    return 0;
}
#else
static inline parse_tree* 
pt_up(parse_tree *pt)
{
    return pt->pt_up;
}

static inline int
pt_up_set(parse_tree *pt, parse_tree *ptp) 
{
    pt->pt_up = ptp;
    pt->pt_obj = 0;
    return 0;
}

static inline cg_obj* 
co_up(cg_obj *co) 
{
    parse_tree *pt = (parse_tree*)&co->co_pt;

    if (pt->pt_obj)
	return (cg_obj*)pt_up(pt);
    else
	return NULL;
}

static inline int
co_up_set(cg_obj *co, cg_obj *cop) 
{
    int retval;
    parse_tree *pt = (parse_tree*)&co->co_pt;

    retval = pt_up_set(pt, (parse_tree*)cop);
    pt->pt_obj = (cop != NULL);
    return retval;
}
#endif

/*
 * A function that maps from string to functions. Used when parsing a file that needs
 * to map function names (string) to actual function pointers.
 * (We may be stretching the power of C here,...)
 */
typedef cg_fnstype_t *(cg_str2fn_t)(char *str, void *arg, char **err);

/*
 * Prototypes
 */
void cligen_parsetree_sort(parse_tree pt, int recursive);
cg_obj *co_new(char *cmd, cg_obj *prev);
cg_obj *cov_new(enum cv_type cvtype, cg_obj *prev);
int co_pref(cg_obj *co, int exact);
int pt_realloc(parse_tree *);

int co_callback_copy(struct cg_callback *cc0, struct cg_callback **ccn, cg_var *arg);
int co_copy(cg_obj *co, cg_obj *parent, cg_obj **conp);
int pt_copy(parse_tree pt, cg_obj *parent, parse_tree *ptn);
int cligen_parsetree_merge(parse_tree *pt0, cg_obj *parent0, parse_tree pt1);
int cligen_parsetree_free(parse_tree pt, int recurse);

int co_free(cg_obj *co, int recursive);

cg_obj *co_insert(parse_tree *pt, cg_obj *co);
cg_obj *co_find_one(parse_tree pt, char *name);
int co_value_set(cg_obj *co, char *str);
char *cligen_reason(const char *fmt, ...);
int pt_apply(parse_tree pt, cg_applyfn_t fn, void *arg);

#endif /* _CLIGEN_GEN_H_ */

