/*
 * Copyright (C) 1991, 1992, 1993 by Chris Thewalt (thewalt@ce.berkeley.edu)
 *
 * Permission to use, copy, modify, and distribute this software 
 * for any purpose and without fee is hereby granted, provided
 * that the above copyright notices appear in all copies and that both the
 * copyright notice and this permission notice appear in supporting
 * documentation.  This software is provided "as is" without express or
 * implied warranty.
 *
 * Thanks to the following people who have provided enhancements and fixes:
 *   Ron Ueberschaer, Christoph Keller, Scott Schwartz, Steven List,
 *   DaviD W. Sanderson, Goran Bostrom, Michael Gleason, Glenn Kasten,
 *   Edin Hodzic, Eric J Bivona, Kai Uwe Rommel, Danny Quah, Ulrich Betzler
 */

#ifndef CLIGEN_GETLINE_H
#define CLIGEN_GETLINE_H

/* These declarations are internal to cligen, not part of cligen.h API
 */

/*
 * Types
 */
typedef size_t (*gl_strwidth_proc)(char *);

/*
 * Prototypes
 */
int     gl_eof(void);
void    gl_exitchar_add(char c);
void    gl_char_init(void);	
void    gl_char_cleanup(void);	

int     gl_getline(cligen_handle h, char **buf); /* read a line of input */
int     gl_init(void);
int     gl_putc(int c);		/* write one char to terminal */
int     gl_getscrolling(void);
void    gl_setscrolling(int);
int     gl_setwidth(int);		/* specify width of screen */
int     gl_getwidth(void);		/* get width of screen */
int     gl_utf8_set(int mode);          /* set UTF-8 experimental mode */
int     gl_utf8_get(void);              /* get UTF-8 mode */
void	gl_strwidth(gl_strwidth_proc);	/* to bind gl_strlen */
void	gl_clear_screen(cligen_handle h); /* clear sceen and redraw */
void	gl_redraw(cligen_handle h);	/* issue \n and redraw all */
int     gl_regfd(int, cligen_fd_cb_t *, void *);
int     gl_unregfd(int);

extern int 	(*gl_in_hook)(void *, char *);
extern int 	(*gl_out_hook)(void*, char *);
extern int	(*gl_tab_hook)(cligen_handle, int *);
extern cligen_susp_cb_t *gl_susp_hook;
extern cligen_interrupt_cb_t *gl_interrupt_hook;
extern int	(*gl_qmark_hook)(cligen_handle, char *);

#endif /* CLIGEN_GETLINE_H */
