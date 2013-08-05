/*
  CVS Version: $Id: cligen_handle.c,v 1.25 2013/06/16 12:01:44 olof Exp $ 

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
/*! \file */ 
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


#include "cligen_var.h"
#include "cligen_cvec.h"
#include "cligen_gen.h"
#include "cligen_handle.h"
#include "cligen_read.h"
#include "cligen_parse.h"
#include "getline.h"

#if 0
#define handle(h) (fprintf(stderr, "%s\n", __FUNCTION__),	\
                   assert(cligen_check(h)==0),		\
                   (struct cligen_handle *)(h))
#endif

/* With sanity check */
#define handle(h) (assert(cligen_check(h)==0),(struct cligen_handle *)(h))

//#define handle(h) ((struct cligen_handle *)(h))

/*
 * CLIgen handle code.
 * Should be moved into its own, but there are some quirks with mutual dependencies
 * with rest cligen_gen.h that I didnt have time to sort out.
 */
/*
 * Constants
 */
#define TERM_ROWS_DEFAULT 24

typedef struct pt_element  { /* Linked list of expansion callbacks */
    struct pt_element *pe_next;
    char              *pe_name;      /* malloced */
#ifdef CLIGEN_SUBMODE
    parse_tree        *pe_submode;   /* pointer into pe_parsetree */
#endif /* SUBMODE */
    parse_tree         pe_parsetree; /* should be free:d */
} pt_element;

#define CLIGEN_MAGIC 0x56ab55aa

/* CLIgen handle. Its members should be hidden and only the typedef visible */
struct cligen_handle{
    int         ch_magic;        /* magic */
    char        ch_exiting;      /* Set by callback to request exit of CLIgen */
    char        ch_comment;      /* comment sign - everything behind it is ignored */
    char       *ch_prompt;       /* current prompt used */
    pt_element *ch_tree;         /* Linked list of parsetrees */
    char       *ch_tree_active;  /* Name of active parse-tree, if null use 1st */
    cg_obj     *ch_co_match;     /* Matching object in latest evaluation */
    int         ch_completion;   /* completion mode */    
    char       *ch_nomatch;      /* Why did a string not match an evaluation? */
    int         ch_terminal_rows; /* Number of output terminal rows */
    int         ch_terminal_length; /* Length of terminal row */
    int         ch_tabmode;      /* short or long output mode on TAB */

    int         ch_lexicalorder; /* strcmp (0) or strverscmp (1) syntax order.
                                    Also, this is global for now */
    int         ch_ignorecase; /* dont care about aA (0), care about aA (1) 
				     does not work if lexicalorder is set.
				     Also this is global for now
				  */

    char       *ch_buf;          /* getline input buffer */
    char       *ch_killbuf;      /* getline killed text */

    void       *ch_userhandle;   /* Use this as app-specific callback handle */
    void       *ch_userdata;     /* application-specific data (any data) */
};



/*! 
 * \brief This is the first call the CLIgen API and returns a handle. 
 *
 */
cligen_handle 
cligen_init(void)
{
    struct cligen_handle *ch;
    cligen_handle h = NULL;

    if ((ch = malloc(sizeof(*ch))) == NULL){
	fprintf(stderr, "%s: malloc: %s\n", __FUNCTION__, strerror(errno));
	goto done;
    }
    memset(ch, 0, sizeof(*ch));
    ch->ch_magic = CLIGEN_MAGIC;
    h = (cligen_handle)ch;
    cligen_prompt_set(h, CLIGEN_PROMPT_DEFAULT);
    cligen_terminalrows_set(h, TERM_ROWS_DEFAULT);
    cliread_init(h);
    gl_buf_init(h);

  done:
    return h;
}

/*! 
 * \brief This is the last call to the CLIgen API
 */
int
cligen_exit(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);
    pt_element *pe;

    gl_histclear();
    gl_buf_cleanup(h);
    if (ch->ch_prompt)
	free(ch->ch_prompt);
    if (ch->ch_nomatch)
	free(ch->ch_nomatch);
    if (ch->ch_tree_active)
	free(ch->ch_tree_active);
    while ((pe = ch->ch_tree) != NULL){
	ch->ch_tree =  pe->pe_next;
	free(pe->pe_name);
	cligen_parsetree_free(pe->pe_parsetree, 1);
	free(pe);
    }
    free(ch);
    return 0;
}

/*
 * Check struct magic number for sanity checks
 * return 0 if OK, -1 if fail.
 */
int
cligen_check(cligen_handle h)
{
    /* Dont use handle macro to avoid recursion */
    struct cligen_handle *ch = (struct cligen_handle *)(h);

    return ch->ch_magic == CLIGEN_MAGIC ? 0 : -1;
}

int 
cligen_exiting(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);

    return ch->ch_exiting;
}

int 
cligen_exiting_set(cligen_handle h, int status)
{
    struct cligen_handle *ch = handle(h);

    ch->ch_exiting = status;
    return 0;
}

char
cligen_comment(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);

    return ch->ch_comment;
}

/*! 
 * \brief Set comment character.
 */
int
cligen_comment_set(cligen_handle h, char c)
{
    struct cligen_handle *ch = handle(h);

    ch->ch_comment = c;
    return 0;
}

/*
 * get and set prompt
 */
char*
cligen_prompt(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);

    return ch->ch_prompt;
}

/*! 
 * \brief Set CLIgen prompt string.
 */
int
cligen_prompt_set(cligen_handle h, char *prompt)
{
    struct cligen_handle *ch = handle(h);

    if (ch->ch_prompt){
	if (strcmp(prompt, ch->ch_prompt) == 0)
	    return 0;
	free(ch->ch_prompt);
	ch->ch_prompt = NULL;
    }
    if (prompt)
	ch->ch_prompt = strdup(prompt);
    return 0;
}

/*! 
 * \brief Get a parsetree, if name==NULL, return first parse-tree
 */
parse_tree *
cligen_tree(cligen_handle h, char *name)
{
    pt_element *pe;
    struct cligen_handle *ch = handle(h);

    for (pe = ch->ch_tree; pe; pe = pe->pe_next)
	if (name==NULL || strcmp(pe->pe_name, name) == 0){
#ifdef CLIGEN_SUBMODE
	    if (pe->pe_submode)
		return pe->pe_submode;
#endif /* SUBMODE */
	    return &pe->pe_parsetree;
	}
    return NULL;
}

/*
 * Get a parsetree (not sub.mode)
 * if name==NULL, return 1st
 */
parse_tree *
cligen_treetop(cligen_handle h, char *name)
{
    pt_element *pe;
    struct cligen_handle *ch = handle(h);

    for (pe = ch->ch_tree; pe; pe = pe->pe_next)
	if (name==NULL || strcmp(pe->pe_name, name) == 0)
	    return &pe->pe_parsetree;
    return NULL;
}



/*! 
 * \brief Add a new parsetree
 */
int 
cligen_tree_add(cligen_handle h, char *name, parse_tree pt)
{
    pt_element *pe;
    struct cligen_handle *ch = handle(h);

    if ((pe = (pt_element *)malloc(sizeof(*pe))) == NULL){
	fprintf(stderr, "%s malloc: %s\n", __FUNCTION__, strerror(errno));
	return -1;
    }
    memset(pe, 0, sizeof(*pe));
    pe->pe_name = strdup(name);
    pe->pe_parsetree = pt;
    /* Here we could add pt_up_set() to all children? */
#if 0
    {
	int i;
	cg_obj *co;

	for (i=0; i<pt.pt_len; i++)
	    if ((co = pt.pt_vec[i]) != NULL)
		pt_up_set((parse_tree*)co, &pe->pe_parsetree);
    }
#endif
    pe->pe_next = ch->ch_tree;
    ch->ch_tree = pe;
    return 0;
}

/*
 * cligen_tree_del
 * Delete a parsetree
 */
int 
cligen_tree_del(cligen_handle h, char *name)
{
    pt_element *pe;
    pt_element **pe_prev;
    struct cligen_handle *ch = handle(h);

    for (pe_prev = &ch->ch_tree, pe = *pe_prev; 
	 pe; 
	 pe_prev = &pe->pe_next, pe = pe->pe_next)
	if (strcmp(pe->pe_name, name) == 0){
	    *pe_prev = pe->pe_next;
	    free(pe->pe_name);
	    free(pe);
	    break;
	}
    return 0;
}

/*! 
 * \brief Get name of currently active parsetree.
 */
char*
cligen_tree_active(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);

    return ch->ch_tree_active;
}

/*! 
 * \brief Set currently active parsetree by name.
 */
int
cligen_tree_active_set(cligen_handle h, char *treename)
{
    struct cligen_handle *ch = handle(h);

    if (ch->ch_tree_active){
	if (strcmp(treename, ch->ch_tree_active) == 0)
	    return 0;
	free(ch->ch_tree_active);
	ch->ch_tree_active = NULL;
    }
    ch->ch_tree_active = treename?strdup(treename):NULL;
    return 0;
}

/* 
 * After an evaluation when calling a callback, a node has been matched in the
 * current parse-tree. This matching node is returned (and set) here.
 */
cg_obj *
cligen_co_match(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);

    return ch->ch_co_match;
}

int
cligen_co_match_set(cligen_handle h, cg_obj *co)
{
    struct cligen_handle *ch = handle(h);

    ch->ch_co_match = co;
    return 0;
}



#ifdef CLIGEN_SUBMODE

#if 0
int
goto(cligen_handle h, cvec *vars, cg_var *arg)
{
    cg_obj     *co;
    char       *treename;
    parse_tree *pt;

    if ((treename = cv_string_get(arg)) == NULL)
        return 0;
    if ((pt = cligen_tree(h, treename)) == NULL)
        return 0;
    if ((co = cligen_co_match(h)) == NULL)
	return 0;
    /* Eeh, we have an object co in the 'full syntax tree'. We need to find the
       corresponding node in syntax-tree 's' */
    return cligen_submode_set(h, cligen_tree_active(h), &co->co_pt);
}
/*
  Same as above, dont do the manipulation on active tree, do it on s.
  XXX: Must have a back-pointer from pt!!
*/
int
up(cligen_handle h, cvec *vars, cg_var *arg)
{
    cg_obj     *co, *cop, *copp;
    parse_tree *pt, *ptp;
    char       *treename;

    treename = cligen_tree_active(h);
    if ((pt = cligen_submode(h, treename)) == NULL)
	return 0;
    if (pt->pt_len == 0)
	return 0;
    if ((co = pt->pt_vec[0]) == NULL)
	return 0;
    if ((cop = co_up(co)) == NULL)
	return 0;
    if ((copp = co_prec(cop)) == NULL)
	return 0;
    ptp = copp->co_pt;
    return cligen_submode_set(h, treename, ptp);
}
#endif

parse_tree *
cligen_submode(cligen_handle h, char *name)
{
    struct cligen_handle *ch = handle(h);
    pt_element *pe;

    for (pe = ch->ch_tree; pe; pe = pe->pe_next)
	if (name==NULL || strcmp(pe->pe_name, name) == 0)
	    break;
    if (pe == NULL)
	return NULL;
    return pe->pe_submode;
}

int 
cligen_submode_set(cligen_handle h, char *name, parse_tree *pt)
{
    struct cligen_handle *ch = handle(h);
    pt_element *pe;

    for (pe = ch->ch_tree; pe; pe = pe->pe_next)
	if (name==NULL || strcmp(pe->pe_name, name) == 0)
	    break;
    if (pe == NULL){
	fprintf(stderr, "No such parse-tree\n");
	return -1;
    }
    /* XXX verify that pt is present in pe->pe_parsetree */
    pe->pe_submode = pt;
    return 0;
}
#endif /* CLIGEN_SUBMODE */

/*! 
 * \brief Get completion mode
 *
 *  0   for each <TAB> complete one level. (default)
 *  1   complete all unique levels at once
 * Example: syntax is 'a b;'. mode = 0 gives completion to 'a ' on first TAB and to 'a b '
 * on second. mode = 1 gives completion to 'a b ' on first TAB.
 */
int
cligen_completion(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);

    return ch->ch_completion;
}

/*! 
 * \brief Set completion mode
 */
int
cligen_completion_set(cligen_handle h, int mode)
{
    struct cligen_handle *ch = handle(h);

    ch->ch_completion = mode;
    return 0;
}

/*
 * Fill error string buffer
 * Why is there no match of an input string in the parse-tree?
 * The call to cliread_parse/getline/eval returns CG_NOMATCH and this is where
 * the reason is stored.
 */
char *
cligen_nomatch(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);

    return ch->ch_nomatch;
}

int
cligen_nomatch_set(cligen_handle h, const char *fmt, ...)
{
    struct cligen_handle *ch = handle(h);
    int res;
    int len;
    va_list ap;
	
    if (fmt == NULL){
	if (ch->ch_nomatch){
	    free(ch->ch_nomatch);
	    ch->ch_nomatch = NULL;
	}
	return 0;
    }
    assert(ch->ch_nomatch == NULL);
    va_start (ap, fmt);
    len = vsnprintf(NULL, 0, fmt, ap);
    len++;
    va_end (ap);
    if ((ch->ch_nomatch = malloc(len)) == NULL){
	fprintf(stderr, "%s: malloc: %s\n", __FUNCTION__, strerror(errno));
	return -1;
    }
    va_start (ap, fmt);
    res = vsnprintf(ch->ch_nomatch, len, fmt, ap);
    va_end (ap);
    
    return res;	
}

static int _terminalrows; /* XXX: global since cli_output dont take handle */

/*! 
 * \brief Get number of displayed terminal rows.
 */
int 
cligen_terminalrows(cligen_handle h)
{
//    struct cligen_handle *ch = handle(h);

    return _terminalrows; /* ch->ch_terminalrows; */
}

/*! 
 * \brief Set number of displayed terminal rows.
 */
int 
cligen_terminalrows_set(cligen_handle h, int rows)
{
    struct cligen_handle *ch = handle(h);

    ch->ch_terminal_rows = rows;
    _terminalrows = rows;
    return 0;
}

/*! 
 * \brief Get length of lines (number of 'columns' in a line)
 */
int 
cligen_terminal_length(cligen_handle h)
{
//    struct cligen_handle *ch = handle(h);

    return gl_getwidth();
}

/*! 
 * \brief Set length of lines (number of 'columns' in a line)
 */
int 
cligen_terminal_length_set(cligen_handle h, int length)
{
    struct cligen_handle *ch = handle(h);

    ch->ch_terminal_length = length;
    gl_setwidth(length);
    return 0;
}


/*! 
 * \brief Get tab-mode, 
 *
 * 0 means short mode (eg like ios), 1 means long mode (eg like junos)
 */
int 
cligen_tabmode(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);

    return ch->ch_tabmode;
}

/*! 
 * \brief Set tab-mode, 
 */
int 
cligen_tabmode_set(cligen_handle h, int mode)
{
    struct cligen_handle *ch = handle(h);

    ch->ch_tabmode = mode;
    return 0;
}


static int _lexicalorder = 0; /* XXX shouldnt be global */

/*! 
 * \brief Get lexical matching order: strcmp (0) or strverscmp (1).
 */
int
cligen_lexicalorder(cligen_handle h)
{
//    struct cligen_handle *ch = handle(h);

//    return ch->ch_lexicalorder;
    return _lexicalorder;
}

/*! 
 * \brief Set lexical matching order: strcmp (0) or strverscmp (1).
 */
int
cligen_lexicalorder_set(cligen_handle h, int n)
{
//    struct cligen_handle *ch = handle(h);

//    ch->ch_lexicalorder = n;
    _lexicalorder = n;
    return 0;
}

static int _ignorecase = 0; /* XXX shouldnt be global */


/*! 
 * \brief Ignore uppercase/lowercase or not
 */
int
cligen_ignorecase(cligen_handle h)
{
//    struct cligen_handle *ch = handle(h);

//    return ch->ch_ignorecase;
    return _ignorecase;
}

/*! 
 * \brief Ignore uppercase/lowercase or not
 */
int
cligen_ignorecase_set(cligen_handle h, int n)
{
//    struct cligen_handle *ch = handle(h);

//    ch->ch_ignorecase = n;
    _ignorecase = n;
    return 0;
}


/*
 * cligen_userdata
 * get user data 
 */
void*
cligen_userhandle(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);

    return ch->ch_userhandle;
}

/*
 * cligen_userdata_set
 * set user data as a part of the cligen_handle
 */
int
cligen_userhandle_set(cligen_handle h, void *userhandle)
{
    struct cligen_handle *ch = handle(h);

    ch->ch_userhandle = userhandle;
    return 0;
}

/*
 * cligen_userdata
 * get user data 
 */
void*
cligen_userdata(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);

    return ch->ch_userdata;
}

/*
 * cligen_userdata_set
 * set user data as a part of the cligen_handle
 */
int
cligen_userdata_set(cligen_handle h, void *user)
{
    struct cligen_handle *ch = handle(h);

    ch->ch_userdata = user;
    return 0;
}

static int _getline_bufsize = 128;

char*
cligen_buf(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);

    return ch->ch_buf;
}

char*
cligen_killbuf(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);

    return ch->ch_killbuf;
}

int 
gl_bufsize(cligen_handle h)
{
    return _getline_bufsize;
}

int
gl_buf_init(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);

    if ((ch->ch_buf = malloc(_getline_bufsize)) == NULL)
	return -1;
    memset(ch->ch_buf, 0, _getline_bufsize);
    if ((ch->ch_killbuf = malloc(_getline_bufsize)) == NULL)
	return -1;
    memset(ch->ch_killbuf, 0, _getline_bufsize);
    return 0;
}

int       
gl_buf_increase(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);
    int len0 = _getline_bufsize;

    _getline_bufsize *= 2;
    if ((ch->ch_buf = realloc(ch->ch_buf, _getline_bufsize)) == NULL)
	return -1;
    memset(ch->ch_buf+len0, 0, _getline_bufsize-len0);
    if ((ch->ch_killbuf = realloc(ch->ch_killbuf, _getline_bufsize)) == NULL)
	return -1;
    memset(ch->ch_killbuf+len0, 0, _getline_bufsize-len0);
    return 0;
}

int
gl_buf_cleanup(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);
    if (ch->ch_buf){
	free(ch->ch_buf);
	ch->ch_buf = NULL;
    }
    if (ch->ch_killbuf){
	free(ch->ch_killbuf);
	ch->ch_killbuf = NULL;
    }
    return 0;
}
