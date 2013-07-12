#ifndef GETLINE_H
#define GETLINE_H


/* unix systems can #define POSIX to use termios, otherwise 
 * the bsd or sysv interface will be used 
 */

#ifdef __STDC__ 

typedef size_t (*gl_strwidth_proc)(char *);

int            gl_eof(void);
void           gl_exitchar_add(char c);

void           gl_char_init(void);	
void           gl_char_cleanup(void);	
char           *gl_getline(void *h);		/* read a line of input */
void            gl_setwidth(int);		/* specify width of screen */
int             gl_getwidth(void);		/* get width of screen */
void            gl_histadd(char *);		/* adds entries to hist */
void            gl_histclear();			/* clears entries to hist */
void		gl_strwidth(gl_strwidth_proc);	/* to bind gl_strlen */
void		gl_redraw(cligen_handle h);	/* issue \n and redraw all */
int             gl_regfd(int, int (*)(int, void*), void *);
int             gl_unregfd(int);

int 	(*gl_in_hook)(void *, char *);
int 	(*gl_out_hook)(void*, char *);
int	(*gl_tab_hook)(void*, char *, int, int *);
int	(*gl_susp_hook)(void*, char *, int, int *);
int	(*gl_qmark_hook)(void*, char *, int);

#else	/* not __STDC__ */

void            gl_char_init();	
void            gl_char_cleanup();	
char           *gl_getline();	
void            gl_setwidth();
int             gl_getwidth();
void            gl_histadd();
void		gl_strwidth();
void		gl_redraw();
int             gl_regfd(int fd, int (*cb)(int, void*), void *arg);

extern int 	(*gl_in_hook)();
extern int 	(*gl_out_hook)();
extern int	(*gl_tab_hook)();
extern int	(*gl_qmark_hook)();

#endif /* __STDC__ */

#endif /* GETLINE_H */
