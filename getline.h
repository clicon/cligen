#ifndef GETLINE_H
#define GETLINE_H

typedef size_t (*gl_strwidth_proc)(char *);

int     gl_eof(void);
void    gl_exitchar_add(char c);
void    gl_char_init(void);	
void    gl_char_cleanup(void);	

int     gl_getline(cligen_handle h, char **buf); /* read a line of input */
int     gl_getscrolling(void);
void    gl_setscrolling(int);
void    gl_setwidth(int);		/* specify width of screen */
int     gl_getwidth(void);		/* get width of screen */
void    gl_histadd(char *);		/* adds entries to hist */
void    gl_histclear();			/* clears entries to hist */
void	gl_strwidth(gl_strwidth_proc);	/* to bind gl_strlen */

void	gl_clear_screen(cligen_handle h); /* clear sceen and redraw */
void	gl_redraw(cligen_handle h);	/* issue \n and redraw all */
int     gl_regfd(int, cligen_fd_cb_t *, void *);
int     gl_unregfd(int);

int 	(*gl_in_hook)(void *, char *);
int 	(*gl_out_hook)(void*, char *);
int	(*gl_tab_hook)(void*, char *, int, int *);
cligen_susp_cb_t *gl_susp_hook;
cligen_interrupt_cb_t *gl_interrupt_hook;
int	(*gl_qmark_hook)(void*, char *, int);

#endif /* GETLINE_H */
