/*
  CLI generator input/output support functions.

  Copyright (C) 2001-2014 Olof Hagsand

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

#include "cligen_config.h"


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <assert.h>
#include <string.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>

#include "cligen_buf.h"
#include "cligen_var.h"
#include "cligen_cvec.h"
#include "cligen_gen.h"
#include "cligen_handle.h"
#include "cligen_read.h"
#include "cligen_print.h"
#include "cligen_io.h"
#include "getline.h"

/*
 * Local variables
 */
static int d_lines=0; /* XXX: global */

int
cli_output_reset(void)
{
    d_lines = 0;
    return 0;
}

/*! CLIgen output function. All printf-style output should be made via this function.
 * 
 * It deals with formatting, page breaks, etc. 
 * NOTE: Many style of outputs cannot be handled this way. Spawning more/less process
 * is more general.
 */
int
cligen_output(FILE *f, char *template, ... )
{
    int     retval = -1;
    va_list args;
    char   *buf = NULL;
    char   *start;
    char   *end;
    char   *bufend;
    char    c;
    int     term_rows;
    int     len;

    term_rows = cligen_terminalrows(NULL);
    /* form a string in buf from all args */

    va_start(args, template);
    len = vsnprintf(NULL, 0, template, args);
    len++;
    va_end(args);
    if ((buf = malloc(len)) == NULL){
	fprintf(stderr, "%s: malloc: %s\n", __FUNCTION__, strerror(errno));
	goto done;
    }
    va_start(args, template);
    vsnprintf(buf, len, template, args);
    va_end(args);

    /* if writing to stdout, format output
     */
    if ((term_rows) && (f == stdout)){
	start = end = buf;
	bufend = buf + strlen(buf);
	while (end < bufend){
	    end = strstr(start, "\n");
	    if (end) /* got a NL */{
		if (d_lines >= 0)
		    d_lines++;
		*end = '\0';
		if (d_lines > -1)
		    fprintf(f, "%s\n", start);
	      
		if (end < bufend)
		    start = end+1;
	      
		if (d_lines >= (term_rows -1)){		    
		    gl_char_init();

		    fprintf(f, "--More--");
		    c = fgetc(stdin);
		    if (c == '\n')
			d_lines--;
		    else if (c == ' ')
			    d_lines = 0;
		    else if (c == 'q' || c == 3) /* ^c */
			d_lines = -1;
		    else if (c == '?')
			fprintf(f, "Press CR for one more line, SPACE for next page, q to quit\n");
		    else 
			d_lines = 0;  
		    fprintf(f, "        ");
		    gl_char_cleanup();
		}
	    } /* NL */
	    else{
		/* do only print if we have data */
		if (d_lines >=0 && *start != '\0')
		    fprintf(f, "%s", start);
		end = start + strlen(start);
		start = end;
	    }
	}
    }
    else{
	fprintf(f, "%s", buf);
    }  
    fflush(f);
    retval = 0;
 done:
    if (buf)
	free(buf);
    return retval;
}

#ifdef notyet
/*
 * Yes/No question. Returns 1 for yes and 0 for no.
 */
int
cli_yesno (const char *fmt, ...)
{
    va_list	  ap;
    char	  buf[1024];

    va_start (ap, fmt);
    vsnprintf (buf, sizeof (buf)-1, fmt, ap);
    va_end (ap);
    printf("%s [yes/no]: ", buf);
    if (cli_getln (buf, sizeof (buf)) == 0)
	if (strlen (buf) && !strncasecmp (buf, "yes", strlen (buf)))
	    return 1;
    return 0;
}
#endif

#if CLIGEN_REGFD
/* The following three callback functions are just wrappers in order to
   not expose getline to external interface */
int 
cligen_regfd(int fd, int (*cb)(int, void*), void *arg)
{
    return gl_regfd(fd, cb, arg);
}

int 
cligen_unregfd(int fd)
{
    return gl_unregfd(fd);
}
#endif /* CLIGEN_REGFD */

void 
cligen_redraw(cligen_handle h)
{
    gl_redraw(h);
}

/*! Register a suspend (^Z) function hook 
 */
int
cligen_susp_hook(cligen_handle h, cligen_susp_cb_t *fn)
{
    gl_susp_hook = fn; /* XXX global */
    return 0;
}

/*! Register extra exit characters (in addition to ctrl-c)
 */
void 
cligen_exitchar_add(cligen_handle h, char c)
{
    gl_exitchar_add(c); /* XXX global */
}


/*! Print top-level help (all commands) of a parse-tree
 */
int
cligen_help(FILE *f, parse_tree pt)
{
    cg_obj *co;
    char    cmd[COLUMN_WIDTH+1];
    int     i;
    cbuf   *cb;

    for (i=0; i<pt.pt_len; i++){
	co = pt.pt_vec[i];
	if (co->co_command != NULL){
	    switch(co->co_type){
	    case CO_VARIABLE:
		if ((cb = cbuf_new()) == NULL){
		    fprintf(stderr, "cbuf_new: %s\n", strerror(errno));
		    return -1;
		}
		cov2cbuf(cb, co, 1);
		snprintf(cmd, COLUMN_WIDTH, "%s", cbuf_get(cb));
		cbuf_free(cb);
		break;
	    case CO_COMMAND:
		strncpy (cmd, co->co_command, COLUMN_WIDTH);
		break;
	    default:
		break;
	    }
	    cligen_output(f, "  %*s %s\n", 
		       -COLUMN_WIDTH, 
		       cmd,
		       co->co_help ? co->co_help : "");
	}
    }
    return 0;
}
