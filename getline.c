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

#include "cligen_config.h"

#include <stddef.h>
#include <stdio.h>
#include <netinet/in.h>

#include "cligen_buf.h"
#include "cligen_var.h"
#include "cligen_cvec.h"
#include "cligen_gen.h"
#include "cligen_io.h"
#include "cligen_handle.h"

#include       "getline.h"

static int      gl_tab();  /* forward reference needed for gl_tab_hook */

/******************** imported interface *********************************/

#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>

#include <stdlib.h>

/********************* exported interface ********************************/

char           *gl_getline();		/* read a line of input */
void            gl_setwidth();		/* specify width of screen */
int             gl_getwidth();          /* get width of screen */
void            gl_histadd();		/* adds entries to hist */
void            gl_histclear();		/* clears entries to hist */
void		gl_strwidth();		/* to bind gl_strlen */

int 		(*gl_in_hook)() = 0;
int 		(*gl_out_hook)() = 0;
int 		(*gl_tab_hook)() = gl_tab;
int 		(*gl_qmark_hook)() = 0;
int		(*gl_susp_hook)() = 0;
int		(*gl_interrupt_hook)() = 0;

/******************** internal interface *********************************/

/* XXX: begin global variables */
static int      gl_init_done = -1;	/* terminal mode flag  */
static int      gl_termw = 80;		/* actual terminal width */
static int      gl_scroll = 27;		/* width of EOL scrolling region */
static int      gl_width = 0;		/* net size available for input */
static int      gl_extent = 0;		/* how far to redraw, 0 means all */
static int      gl_overwrite = 0;	/* overwrite mode */
static int      gl_pos, gl_cnt = 0;     /* position and size of input */

static char     gl_intrc = 0;		/* keyboard SIGINT char (^C) */
static char     gl_quitc = 0;		/* keyboard SIGQUIT char (^]) */
static char     gl_suspc = 0;		/* keyboard SIGTSTP char (^Z) */
static char     gl_dsuspc = 0;		/* delayed SIGTSTP char */
static int      gl_search_mode = 0;	/* search mode flag */

static int exitchars[8] = {0,}; /* 8 different exit chars should be enough */
static int gl_iseof = 0;

    static int   fixup_gl_shift;	/* index of first on screen character */
    static int   fixup_off_right;	/* true if more text right of screen */
    static int   fixup_off_left;	/* true if more text left of screen */
    static char  fixup_last_prompt[80] = "";

#define HIST_SIZE 100
static int      hist_pos = 0, hist_last = 0;
static char    *hist_buf[HIST_SIZE];
static char    *hist_pre = 0;

static char  search_prompt[101];  /* prompt includes search string */
static char  search_string[100];
static int   search_pos = 0;      /* current location in search_string */
static int   search_forw_flg = 0; /* search direction flag */
static int   search_last = 0;	  /* last match found */

/* XXX: end global variables */

static void     gl_init(void);		/* prepare to edit a line */
static void     gl_cleanup(void);	/* to undo gl_init */
void            gl_char_init(void);	/* get ready for no echo input */
void            gl_char_cleanup(void);	/* undo gl_char_init */
static size_t 	(*gl_strlen)() = (size_t(*)())strlen; 
					/* returns printable prompt width */

static void     gl_addchar(cligen_handle h, int c);	/* install specified char */
static void     gl_del(cligen_handle h, int loc);	/* del, either left (-1) or cur (0) */
static void     gl_error(char *buf);	/* write error msg and die */
static void     gl_fixup(cligen_handle h, char*,int,int);/* fixup state variables and screen */
static int      gl_getc(cligen_handle h);	        /* read one char from terminal */
static void     gl_kill(cligen_handle h, int pos);	/* delete to EOL */
static void     gl_kill_begin(cligen_handle h, int pos);	/* delete to BEGIN of line */
static void     gl_kill_word(cligen_handle h, int pos);	/* delete word */
static void     gl_newline(cligen_handle);	/* handle \n or \r */
static int      gl_putc(int c);		/* write one char to terminal */
static int      gl_puts(char *buf);	/* write a line to terminal */

static void     gl_transpose(cligen_handle h);	/* transpose two chars */
static void     gl_yank(cligen_handle h);		/* yank killed text */
static void     gl_word(cligen_handle h, int dir);	/* move a word */

static void     hist_init(void);	/* initializes hist pointers */
static char    *hist_next(void);	/* return ptr to next item */
static char    *hist_prev(void);	/* return ptr to prev item */
static char    *hist_save(char *p);	/* makes copy of a string, without NL */

static void     search_addchar(cligen_handle h, int c);	/* increment search string */
static void     search_term(cligen_handle);	/* reset with current contents */
static void     search_back(cligen_handle h, int new);	/* look back for current string */
static void     search_forw(cligen_handle h, int new);	/* look forw for current string */

/************************ nonportable part *********************************/

#ifdef unix
#ifndef __unix__
#define __unix__
#endif /* not __unix__ */
#endif /* unix */

#ifdef _IBMR2
#ifndef __unix__
#define __unix__
#endif
#endif

#ifdef __GO32__
#include <pc.h>
#undef MSDOS
#undef __unix__
#endif

#ifdef MSDOS
#include <bios.h>
#endif

#ifdef __unix__
#include <unistd.h>

#define POSIX
#ifdef POSIX		/* use POSIX interface */
#include <termios.h>
struct termios  new_termios, old_termios;
#else /* not POSIX */
#include <sys/ioctl.h>
#ifdef M_XENIX	/* does not really use bsd terminal interface */
#undef TIOCSETN
#endif /* M_XENIX */
#ifdef TIOCSETN		/* use BSD interface */
#include <sgtty.h>
struct sgttyb   new_tty, old_tty;
struct tchars   tch;
struct ltchars  ltch;
#else			/* use SYSV interface */
#include <termio.h>
struct termio   new_termio, old_termio;
#endif /* TIOCSETN */
#endif /* POSIX */
#endif	/* __unix__ */

#ifdef vms
#include <descrip.h>
#include <ttdef.h>
#include <iodef.h>
#include unixio
   
static int   setbuff[2];             /* buffer to set terminal attributes */
static short chan = -1;              /* channel to terminal */
struct dsc$descriptor_s descrip;     /* VMS descriptor */
#endif

void
gl_char_init(void)			/* turn off input echo */
{
#ifdef __unix__
#ifdef POSIX
    tcgetattr(0, &old_termios);
    gl_intrc = old_termios.c_cc[VINTR];
    gl_quitc = old_termios.c_cc[VQUIT];
#ifdef VSUSP
    gl_suspc = old_termios.c_cc[VSUSP];
#endif
#ifdef VDSUSP
    gl_dsuspc = old_termios.c_cc[VDSUSP];
#endif
    new_termios = old_termios;
    new_termios.c_iflag &= ~(BRKINT|ISTRIP|IXON|IXOFF);
    new_termios.c_iflag |= (IGNBRK|IGNPAR);
    new_termios.c_lflag &= ~(ICANON|ISIG|IEXTEN|ECHO);
    new_termios.c_cc[VMIN] = 1;
    new_termios.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &new_termios);
#else				/* not POSIX */
#ifdef TIOCSETN			/* BSD */
    ioctl(0, TIOCGETC, &tch);
    ioctl(0, TIOCGLTC, &ltch);
    gl_intrc = tch.t_intrc;
    gl_quitc = tch.t_quitc;
    gl_suspc = ltch.t_suspc;
    gl_dsuspc = ltch.t_dsuspc;
    ioctl(0, TIOCGETP, &old_tty);
    new_tty = old_tty;
    new_tty.sg_flags |= RAW;
    new_tty.sg_flags &= ~ECHO;
    ioctl(0, TIOCSETN, &new_tty);
#else				/* SYSV */
    ioctl(0, TCGETA, &old_termio);
    gl_intrc = old_termio.c_cc[VINTR];
    gl_quitc = old_termio.c_cc[VQUIT];
    new_termio = old_termio;
    new_termio.c_iflag &= ~(BRKINT|ISTRIP|IXON|IXOFF);
    new_termio.c_iflag |= (IGNBRK|IGNPAR);
    new_termio.c_lflag &= ~(ICANON|ISIG|ECHO);
    new_termio.c_cc[VMIN] = 1;
    new_termio.c_cc[VTIME] = 0;
    ioctl(0, TCSETA, &new_termio);
#endif /* TIOCSETN */
#endif /* POSIX */
#endif /* __unix__ */

#ifdef vms
    descrip.dsc$w_length  = strlen("tt:");
    descrip.dsc$b_dtype   = DSC$K_DTYPE_T;
    descrip.dsc$b_class   = DSC$K_CLASS_S;
    descrip.dsc$a_pointer = "tt:";
    (void)sys$assign(&descrip,&chan,0,0);
    (void)sys$qiow(0,chan,IO$_SENSEMODE,0,0,0,setbuff,8,0,0,0,0);
    setbuff[1] |= TT$M_NOECHO;
    (void)sys$qiow(0,chan,IO$_SETMODE,0,0,0,setbuff,8,0,0,0,0);
#endif /* vms */
}

void
gl_char_cleanup(void)		/* undo effects of gl_char_init */
{
#ifdef __unix__
#ifdef POSIX 
    tcsetattr(0, TCSANOW, &old_termios);
#else 			/* not POSIX */
#ifdef TIOCSETN		/* BSD */
    ioctl(0, TIOCSETN, &old_tty);
#else			/* SYSV */
    ioctl(0, TCSETA, &old_termio);
#endif /* TIOCSETN */
#endif /* POSIX */
#endif /* __unix__ */

#ifdef vms
    setbuff[1] &= ~TT$M_NOECHO;
    (void)sys$qiow(0,chan,IO$_SETMODE,0,0,0,setbuff,8,0,0,0,0);
    sys$dassgn(chan);
    chan = -1;
#endif 
}

#if MSDOS || __EMX__ || __GO32__
int pc_keymap(c)
int c;
{
    switch (c) {
    case 72: c = 16;   /* up -> ^P */
        break;
    case 80: c = 14;   /* down -> ^N */
        break;
    case 75: c = 2;    /* left -> ^B */
        break;
    case 77: c = 6;    /* right -> ^F */
        break;
    default: c = 0;    /* make it garbage */
    }
    return c;
}
#endif /* MSDOS || __EMX__ || __GO32__ */


#if CLIGEN_REGFD
struct regfd {
    int fd;
    int (*cb)(int, void*);
    void *arg;
};
static int nextfds = 0;
static struct regfd *extfds = NULL;

/* XXX: If arg is malloced, the treatment of arg creates leaks */
int
gl_regfd(int fd, cligen_fd_cb_t *cb, void *arg)
{
    int i;
    struct regfd *tmp;

    for (i = 0; i < nextfds; i++) {
	if (extfds[i].fd == fd) { /* Already registered. Update arg and cb */
	    extfds[i].cb = cb;
	    extfds[i].arg = arg;
	    return 0;
	}
    }

    if ((tmp = realloc(extfds, (nextfds+1) * sizeof(*extfds))) == NULL)
	return -1;
    tmp[nextfds].fd = fd;
    tmp[nextfds].cb = cb;
    tmp[nextfds].arg = arg;
    extfds = tmp;
    nextfds++;

    return 0;
}

int
gl_unregfd(int fd)
{
    int i;

    for (i = 0; i < nextfds; i++) {
	if (extfds[i].fd == fd) {
	    if (i+1 < nextfds)
		memcpy(&extfds[i], &extfds[i+1], nextfds-i);
	    extfds = realloc(extfds, (nextfds-1) * sizeof(*extfds));
	    nextfds--;
	    return 0;
	}
    }

    return -1;
}

int
gl_select()
{
    int i;
    fd_set fdset;

    while (1){
	FD_ZERO(&fdset);
	FD_SET(0, &fdset);
	for(i = 0; i < nextfds; i++)
	    FD_SET(extfds[i].fd, &fdset);
	if (select(FD_SETSIZE, &fdset, NULL, NULL, NULL) < 0)
	    return -1;
	for(i = 0; i < nextfds; i++)
	    if (FD_ISSET(extfds[i].fd, &fdset))
		if (extfds[i].cb(extfds[i].fd, extfds[i].arg) < 0)
		    return -1;
	if (FD_ISSET(0, &fdset))
	    break;
    }
    return 0;
}
#endif


int
gl_eof()
{
    return gl_iseof;
}

void
gl_exitchar_add(char c)
{
    int i;

    for (i=0;sizeof(exitchars);i++)
	if (!exitchars[i]){
	    exitchars[i] = c;
	    break;
	}
}

/* check if c is an exit char */
static int
gl_exitchar(char c)
{
    int i;

    for (i=0;sizeof(exitchars);i++){
	if (!exitchars[i])
	    break;
	if (exitchars[i] == c)
	    return 1;
    }
    return 0; /* ^C */
}

/*
 * Initiate an exit, not actually made, but gl_exiting() will return 1.
 */
static char *
gl_exit(cligen_handle h)
{
    char *gl_buf = cligen_buf(h);

    gl_iseof++;
    gl_buf[0] = 0;
    gl_cleanup();
    gl_putc('\n');
    return gl_buf;
}

static int
gl_getc(cligen_handle h)
/* get a character without echoing it to screen */
{
    int             c;
#ifdef __unix__
    char            ch;
#endif

#if CLIGEN_REGFD 
    gl_select(); /* block until something arrives on stdin */
#endif
#ifdef __unix__
    while ((c = read(0, &ch, 1)) == -1) {
	if (errno == EINTR){
	    if (gl_interrupt_hook(h) <0)
		return -1;
	    continue;
	}
	break;
    }
    if (c == 0){
	gl_iseof++;
	cligen_buf(h)[0] = 0; /* clean exit from gl? */
	gl_cleanup();
	gl_putc('\n');
	return -1;
    }
    c = (ch <= 0)? -1 : ch;
#endif	/* __unix__ */
#ifdef MSDOS
    c = _bios_keybrd(_NKEYBRD_READ);
#endif  /* MSDOS */
#ifdef __GO32__
    c = getkey () ;
    if (c > 255) c = pc_keymap(c & 0377);
#endif /* __GO32__ */
#ifdef __TURBOC__
    while(!bioskey(1))
	;
    c = bioskey(0);
#endif
#if MSDOS || __TURBOC__
    if ((c & 0377) == 224) {
	c = pc_keymap((c >> 8) & 0377);
    } else {
	c &= 0377;
    }
#endif /* MSDOS || __TURBOC__ */
#ifdef __EMX__
    c = _read_kbd(0, 1, 0);
    if (c == 224 || c == 0) {
        c = pc_keymap(_read_kbd(0, 1, 0));
    } else {
        c &= 0377;
    }
#endif
#ifdef vms
    if(chan < 0) {
       c='\0';
    }
    (void)sys$qiow(0,chan,IO$_TTYREADALL,0,0,0,&c,1,0,0,0,0);
    c &= 0177;                        /* get a char */
#endif
    return c;
}

static int
gl_putc(c)
int     c;
{
    char   ch = c;

    if (write(1, &ch, 1) < 0)
      return -1;
    if (ch == '\n') {
	ch = '\r'; /* RAW mode needs '\r', does not hurt */
        if (write(1, &ch, 1) < 0)
	  return -1;
    }
    return 0;
}

/******************** fairly portable part *********************************/

static int
gl_puts(buf)
char *buf;
{
    int len; 
    
    if (buf) {
        len = strlen(buf);
        if (write(1, buf, len) < 0)
	  return -1;
    }
    return 0;
}

static void
gl_error(buf)
char *buf;
{
    int len = strlen(buf);

    gl_cleanup();
    if (write(2, buf, len) < 0)
	exit(1);
    exit(1);
}

static void
gl_init()
/* set up variables and terminal */
{
    gl_iseof = 0;
    if (gl_init_done < 0) {		/* -1 only on startup */
        hist_init();
    }
#if 0
    if (isatty(0) == 0 || isatty(1) == 0)
	gl_error("\n*** Error: gl_getline(): not interactive, use stdio.\n");
#endif
    gl_char_init();
    gl_init_done = 1;
}

static void
gl_cleanup()
/* undo effects of gl_init, as necessary */
{
    if (gl_init_done > 0)
        gl_char_cleanup();
    gl_init_done = 0;
}

int
gl_getwidth(void)
{
    return gl_termw;
}

void
gl_setwidth(w)
int  w;
{
    if (w > TERM_MIN_SCREEN_WIDTH-1) {
	gl_termw = w;
	gl_scroll = w / 3;
    } else {
	gl_error("\n*** Error: minimum screen width is 21\n");
    }
}

char *
gl_getline(cligen_handle h)
{
    int             c, loc, tmp;
    char           *gl_buf;
    char           *gl_prompt;
    int             escape = 0;
#ifdef __unix__
    int	            sig;
#endif

    gl_buf = cligen_buf(h);
    gl_init();	
    gl_prompt = (cligen_prompt(h))? cligen_prompt(h) : "";
    gl_buf[0] = 0;
    if (gl_in_hook)
	gl_in_hook(h, gl_buf);
    gl_fixup(h, gl_prompt, -2, gl_bufsize(h));
    while ((c = gl_getc(h)) >= 0) {
	gl_extent = 0;  	/* reset to full extent */
	if (isprint(c)) {
	    if (escape == 0 && c == '\\')
               escape++;
            else{
	    if (escape ==0 && c == '?' && gl_qmark_hook) {
		escape = 0;
		if (gl_qmark_hook(h, gl_buf, gl_pos))
		    gl_fixup(h, gl_prompt, -2, gl_pos);
	    }
	    else{ 
		escape = 0;
		if (gl_search_mode)
		search_addchar(h, c);
	    else
		gl_addchar(h, c);
	    }
	    }
	} else {
	    escape = 0;
	    if (gl_search_mode) { /* after ^S or ^R */
	        if (c == '\033' || c == '\016' || c == '\020') { /* ESC, ^N, ^P */
	            search_term(h);
	            c = 0;     		/* ignore the character */
		} else if (c == '\010' || c == '\177') { /* del */
		    search_addchar(h, -1); /* unwind search string */
		    c = 0;
		} else if (c != '\022' && c != '\023') { /* ^R ^S */
		    search_term(h);	/* terminate and handle char */
		}
	    }
	    /* special exit characters */
	    if (gl_exitchar(c))
		return gl_exit(h);
	    switch (c) {
	    case '\n': case '\r': 			/* newline */
		gl_newline(h);
		gl_cleanup();
		return gl_buf;
		/*NOTREACHED*/
		break; 
	    case '\001': gl_fixup(h, gl_prompt, -1, 0);		/* ^A */
		break;
	    case '\002': gl_fixup(h, gl_prompt, -1, gl_pos-1);	/* ^B */
		break;
	    case '\004':					/* ^D */
		if (gl_cnt == 0) {
		    return gl_exit(h);
		} else {
		    gl_del(h, 0);
		}
		break;
	    case '\005': gl_fixup(h, gl_prompt, -1, gl_cnt);	/* ^E */
		break;
	    case '\006': gl_fixup(h, gl_prompt, -1, gl_pos+1);	/* ^F */
		break;
	    case '\010': case '\177': gl_del(h, -1);	/* ^H and DEL */
		break;
	    case '\t':        				/* TAB */
                if (gl_tab_hook) {
		    tmp = gl_pos;
	            loc = gl_tab_hook(h, gl_buf, gl_strlen(gl_prompt), &tmp);
	            if (loc != -1 || tmp != gl_pos)
	                gl_fixup(h, gl_prompt, loc, tmp);
                }
		break;
	    case '\013': gl_kill(h, gl_pos);			/* ^K */
		break;
	    case '\014': gl_redraw(h);				/* ^L */
		break;
	    case '\016': 					/* ^N */
		strncpy(gl_buf, hist_next(), gl_bufsize(h));
                if (gl_in_hook)
	            gl_in_hook(h, gl_buf);
		gl_fixup(h, gl_prompt, 0, gl_bufsize(h));
		break;
	    case '\017': gl_overwrite = !gl_overwrite;       	/* ^O */
		break;
	    case '\020': 					/* ^P */
		strncpy(gl_buf, hist_prev(), gl_bufsize(h));
                if (gl_in_hook)
	            gl_in_hook(h, gl_buf);
		gl_fixup(h, gl_prompt, 0, gl_bufsize(h));
		break;
	    case '\022': search_back(h, 1);			/* ^R */
		break;
	    case '\023': search_forw(h, 1);			/* ^S */
		break;
	    case '\024': gl_transpose(h);			/* ^T */
		break;
	    case '\025': gl_kill_begin(h, gl_pos);		/* ^U */
		break;
	    case '\027': gl_kill_word(h, gl_pos);		/* ^W */
		break;
	    case '\031': gl_yank(h);				/* ^Y */
		break;
	    case '\032':                                      /* ^Z */
		if(gl_susp_hook) {
		    tmp = gl_pos;
	            loc = gl_susp_hook(cligen_userhandle(h)?cligen_userhandle(h):h,
				       gl_buf, gl_strlen(gl_prompt), &tmp);
	            if (loc != -1 || tmp != gl_pos)
	                gl_fixup(h, gl_prompt, loc, tmp);
		    if (strchr (gl_buf, '\n')) {
			gl_cleanup();
			return gl_buf;
		    }
		}
		break;
	    case '\033':	/* ansi arrow keys (ESC) */
		c = gl_getc(h);
		/* ESC-[ is normal and ESC-O is application cursor keys */
		if (c == '[' || c == 'O') { 
		    switch(c = gl_getc(h)) {
		    case 'A':             			/* up */
			strncpy(gl_buf, hist_prev(), gl_bufsize(h));
                        if (gl_in_hook)
	                    gl_in_hook(h, gl_buf);
		        gl_fixup(h, gl_prompt, 0, gl_bufsize(h));
		        break;
		    case 'B':                         	/* down */
		        strncpy(gl_buf, hist_next(), gl_bufsize(h));
                        if (gl_in_hook)
	                    gl_in_hook(h, gl_buf);
		        gl_fixup(h, gl_prompt, 0, gl_bufsize(h));
		        break;
		    case 'C': gl_fixup(h, gl_prompt, -1, gl_pos+1); /* right */
		        break;
		    case 'D': gl_fixup(h, gl_prompt, -1, gl_pos-1); /* left */
		        break;
		    default: gl_putc('\007');         /* who knows */
		        break;
		    }
		} else if (c == 'f' || c == 'F') {
		    gl_word(h, 1);
		} else if (c == 'b' || c == 'B') {
		    gl_word(h, -1);
		} else
		    gl_putc('\007');
		break;
	    default:		/* check for a terminal signal */
#ifdef __unix__
	        if (c > 0) {	/* ignore 0 (reset above) */
	            sig = 0;
#ifdef SIGINT
	            if (c == gl_intrc)
	                sig = SIGINT;
#endif
#ifdef SIGQUIT
	            if (c == gl_quitc)
	                sig = SIGQUIT;
#endif
#ifdef SIGTSTP
	            if (c == gl_suspc || c == gl_dsuspc)
	                sig = SIGTSTP;
#endif
                    if (sig != 0) {
	                gl_cleanup();
	                kill(0, sig);
	                gl_init();
	                gl_redraw(h);
			gl_kill(h, 0);
			c = 0;
		    } 
		}
#endif /* __unix__ */
                if (c > 0)
		    gl_putc('\007');
		break;
	    }
	}
    }
    gl_cleanup();
    gl_buf[0] = 0;
    return gl_buf;
}

static void
gl_addchar(cligen_handle h, int c)
/* adds the character c to the input buffer at current location */
{
    int  i;

    if (gl_cnt >= gl_bufsize(h) - 1){
	gl_buf_increase(h);
    }
    if (gl_overwrite == 0 || gl_pos == gl_cnt) {
        for (i=gl_cnt; i >= gl_pos; i--)
            cligen_buf(h)[i+1] = cligen_buf(h)[i];
        cligen_buf(h)[gl_pos] = c;
        gl_fixup(h, cligen_prompt(h), gl_pos, gl_pos+1);
    } else {
	cligen_buf(h)[gl_pos] = c;
	gl_extent = 1;
        gl_fixup(h, cligen_prompt(h), gl_pos, gl_pos+1);
    }
}

static void
gl_yank(cligen_handle h)
/* adds the kill buffer to the input buffer at current location */
{
    int  i, len;

    len = strlen(cligen_killbuf(h));
    if (len > 0) {
	if (gl_overwrite == 0) {
            if (gl_cnt + len >= gl_bufsize(h) - 1) {
		gl_buf_increase(h);
	    }
            for (i=gl_cnt; i >= gl_pos; i--)
                cligen_buf(h)[i+len] = cligen_buf(h)[i];
	    for (i=0; i < len; i++)
                cligen_buf(h)[gl_pos+i] = cligen_killbuf(h)[i];
            gl_fixup(h, cligen_prompt(h), gl_pos, gl_pos+len);
	} else {
	    if (gl_pos + len > gl_cnt) {
                if (gl_pos + len >= gl_bufsize(h) - 1) {
		    gl_buf_increase(h);
		}
		cligen_buf(h)[gl_pos + len] = 0;
            }
	    for (i=0; i < len; i++)
                cligen_buf(h)[gl_pos+i] = cligen_killbuf(h)[i];
	    gl_extent = len;
            gl_fixup(h, cligen_prompt(h), gl_pos, gl_pos+len);
	}
    } else
	gl_putc('\007');
}

static void
gl_transpose(cligen_handle h)
/* switch character under cursor and to left of cursor */
{
    int    c;

    if (gl_pos > 0 && gl_cnt > gl_pos) {
	c = cligen_buf(h)[gl_pos-1];
	cligen_buf(h)[gl_pos-1] = cligen_buf(h)[gl_pos];
	cligen_buf(h)[gl_pos] = c;
	gl_extent = 2;
	gl_fixup(h, cligen_prompt(h), gl_pos-1, gl_pos);
    } else
	gl_putc('\007');
}

static void
gl_newline(cligen_handle h)
/*
 * Cleans up entire line before returning to caller. A \n is appended.
 * If line longer than screen, we redraw starting at beginning
 */
{
    int change = gl_cnt;
    int len = gl_cnt;
    int loc = gl_width - 5;	/* shifts line back to start position */

    if (gl_cnt >= gl_bufsize(h) - 1) 
        gl_error("\n*** Error: gl_getline(): input buffer overflow\n");
    if (gl_out_hook) {
	change = gl_out_hook(h, cligen_buf(h));
        len = strlen(cligen_buf(h));
    } 
    if (loc > len)
	loc = len;
    gl_fixup(h, cligen_prompt(h), change, loc);	/* must do this before appending \n */
    cligen_buf(h)[len] = '\n';
    cligen_buf(h)[len+1] = '\0';
    gl_putc('\n');
}

static void
gl_del(cligen_handle h, int loc)
/*
 * Delete a character.  The loc variable can be:
 *    -1 : delete character to left of cursor
 *     0 : delete character under cursor
 */
{
    int i;

    if ((loc == -1 && gl_pos > 0) || (loc == 0 && gl_pos < gl_cnt)) {
        for (i=gl_pos+loc; i < gl_cnt; i++)
	    cligen_buf(h)[i] = cligen_buf(h)[i+1];
	gl_fixup(h, cligen_prompt(h), gl_pos+loc, gl_pos+loc);
    } else
	gl_putc('\007');
}

static void
gl_kill(cligen_handle h, int pos)
/* delete from pos to the end of line */
{
    if (pos < gl_cnt) {
	strncpy(cligen_killbuf(h), cligen_buf(h) + pos, gl_bufsize(h));
	cligen_buf(h)[pos] = '\0';
	gl_fixup(h, cligen_prompt(h), pos, pos);
    } else
	gl_putc('\007');
}

static void
gl_kill_begin(cligen_handle h, int pos)
/* delete from pos to start of line */
{
    int i;
    int len;

    if (pos != 0) {
	len = strlen(cligen_buf(h));
	strncpy(cligen_killbuf(h), cligen_buf(h), pos);
	cligen_killbuf(h)[pos] = '\0';
	memmove(cligen_buf(h), cligen_buf(h) + pos, len-pos+1); /* memmove may overlap */
	gl_fixup(h, cligen_prompt(h), 0, 0);
	for (i=gl_pos; i < gl_cnt; i++)
            gl_putc(cligen_buf(h)[i]);
	gl_fixup(h, cligen_prompt(h), -2, 0);
    } else
	gl_putc('\007');
}

static void
gl_kill_word(cligen_handle h, int pos)
/* delete one previous word from pos */
{
    int i, wpos;
    if (pos != 0) {
	wpos = pos;
	if (pos > 0)
	    pos--;
	while (isspace((int)cligen_buf(h)[pos]) && pos > 0)
	    pos--;
        while (!isspace((int)cligen_buf(h)[pos]) && pos > 0) 
	    pos--;
	if (pos < gl_cnt && isspace((int)cligen_buf(h)[pos]))   /* move onto word */
	    pos++;
	strncpy(cligen_killbuf(h), cligen_buf(h)+pos, wpos-pos);
	cligen_killbuf(h)[wpos-pos] = '\0';
	memmove(cligen_buf(h)+pos, cligen_buf(h) + wpos, wpos-pos+1);
	gl_fixup(h, cligen_prompt(h), wpos, pos);
	for (i=gl_pos; i < gl_cnt; i++)
            gl_putc(cligen_buf(h)[i]);
	gl_fixup(h, cligen_prompt(h), -2, pos);

    } else
	gl_putc('\007');
}


static void
gl_word(cligen_handle h, int direction)
/* move forward or backword one word */
{
    int pos = gl_pos;

    if (direction > 0) {		/* forward */
        while (!isspace((int)cligen_buf(h)[pos]) && (pos < gl_cnt)) 
	    pos++;
	while (isspace((int)cligen_buf(h)[pos]) && pos < gl_cnt)
	    pos++;
    } else {				/* backword */
	if (pos > 0)
	    pos--;
	while (isspace((int)cligen_buf(h)[pos]) && pos > 0)
	    pos--;
        while (!isspace((int)cligen_buf(h)[pos]) && pos > 0) 
	    pos--;
	if (pos < gl_cnt && isspace((int)cligen_buf(h)[pos]))   /* move onto word */
	    pos++;
    }
    gl_fixup(h, cligen_prompt(h), -1, pos);
}

void
gl_redraw(cligen_handle h)
/* emit a newline, reset and redraw prompt and current input line */
{
    if (gl_init_done > 0) {
        gl_putc('\n');
        gl_fixup(h, cligen_prompt(h), -2, gl_pos);
    }
}

static void
gl_fixup(cligen_handle h, char  *prompt, int change, int cursor)
/*
 * This function is used both for redrawing when input changes or for
 * moving within the input line.  The parameters are:
 *   prompt:  compared to last_prompt[] for changes;
 *   change : the index of the start of changes in the input buffer,
 *            with -1 indicating no changes, -2 indicating we're on
 *            a new line, redraw everything.
 *   cursor : the desired location of the cursor after the call.
 *            A value of gl_bufsize(h) can be used  to indicate the cursor should
 *            move just past the end of the input line.
 */
{
    int          left = 0, right = -1;		/* bounds for redraw */
    int          pad;		/* how much to erase at end of line */
    int          backup;        /* how far to backup before fixing */
    int          new_shift;     /* value of shift based on cursor */
    int          extra;         /* adjusts when shift (scroll) happens */
    int          i;
    int          new_right = -1; /* alternate right bound, using gl_extent */
    int          l1, l2;

    if (change == -2) {   /* reset */
	gl_pos = gl_cnt = fixup_gl_shift = fixup_off_right = fixup_off_left = 0;
	gl_putc('\r');
	gl_puts(prompt);
	strncpy(fixup_last_prompt, prompt, sizeof(fixup_last_prompt));
	change = 0;
        gl_width = gl_termw - gl_strlen(prompt);
    } else if (strcmp(prompt, fixup_last_prompt) != 0) {
	l1 = gl_strlen(fixup_last_prompt);
	l2 = gl_strlen(prompt);
	gl_cnt = gl_cnt + l1 - l2;
	strncpy(fixup_last_prompt, prompt, sizeof(fixup_last_prompt));
	gl_putc('\r');
	gl_puts(prompt);
	gl_pos = fixup_gl_shift;
        gl_width = gl_termw - l2;
	change = 0;
    }
    pad = (fixup_off_right)? gl_width - 1 : gl_cnt - fixup_gl_shift;   /* old length */
    backup = gl_pos - fixup_gl_shift;
    if (change >= 0) {
        gl_cnt = strlen(cligen_buf(h));
        if (change > gl_cnt)
	    change = gl_cnt;
    }
    if (cursor > gl_cnt) {
	if (cursor != gl_bufsize(h))		/* gl_bufsize(h) means end of line */
	    gl_putc('\007');
	cursor = gl_cnt;
    }
    if (cursor < 0) {
	gl_putc('\007');
	cursor = 0;
    }
    if (fixup_off_right || (fixup_off_left && cursor < fixup_gl_shift + gl_width - gl_scroll / 2))
	extra = 2;			/* shift the scrolling boundary */
    else 
	extra = 0;
    new_shift = cursor + extra + gl_scroll - gl_width;
    if (new_shift > 0) {
	new_shift /= gl_scroll;
	new_shift *= gl_scroll;
    } else
	new_shift = 0;
    if (new_shift != fixup_gl_shift) {	/* scroll occurs */
	fixup_gl_shift = new_shift;
	fixup_off_left = (fixup_gl_shift)? 1 : 0;
	fixup_off_right = (gl_cnt > fixup_gl_shift + gl_width - 1)? 1 : 0;
        left = fixup_gl_shift;
	new_right = right = (fixup_off_right)? fixup_gl_shift + gl_width - 2 : gl_cnt;
    } else if (change >= 0) {		/* no scroll, but text changed */
	if (change < fixup_gl_shift + fixup_off_left) {
	    left = fixup_gl_shift;
	} else {
	    left = change;
	    backup = gl_pos - change;
	}
	fixup_off_right = (gl_cnt > fixup_gl_shift + gl_width - 1)? 1 : 0;
	right = (fixup_off_right)? fixup_gl_shift + gl_width - 2 : gl_cnt;
	new_right = (gl_extent && (right > left + gl_extent))? 
	             left + gl_extent : right;
    }
    pad -= (fixup_off_right)? gl_width - 1 : gl_cnt - fixup_gl_shift;
    pad = (pad < 0)? 0 : pad;
    if (left <= right) {		/* clean up screen */
	for (i=0; i < backup; i++)
	    gl_putc('\b');
	if (left == fixup_gl_shift && fixup_off_left) {
	    gl_putc('$');
	    left++;
        }
	for (i=left; i < new_right; i++)
	    gl_putc(cligen_buf(h)[i]);
	gl_pos = new_right;
	if (fixup_off_right && new_right == right) {
	    gl_putc('$');
	    gl_pos++;
	} else { 
	    for (i=0; i < pad; i++)	/* erase remains of prev line */
		gl_putc(' ');
	    gl_pos += pad;
	}
    }
    i = gl_pos - cursor;		/* move to final cursor location */
    if (i > 0) {
	while (i--)
	   gl_putc('\b');
    } else {
	for (i=gl_pos; i < cursor; i++)
	    gl_putc(cligen_buf(h)[i]);
    }
    gl_pos = cursor;
}

static int
gl_tab(cligen_handle h, char  *buf, int    offset, int   *loc)
/* default tab handler, acts like tabstops every 8 cols */
{
    int i, count, len;

    len = strlen(buf);
    count = 8 - (offset + *loc) % 8;
    for (i=len; i >= *loc; i--)
        buf[i+count] = buf[i];
    for (i=0; i < count; i++)
        buf[*loc+i] = ' ';
    i = *loc;
    *loc = i + count;
    return i;
}

/******************* strlen stuff **************************************/

void gl_strwidth(func)
size_t (*func)();
{
    if (func != 0) {
	gl_strlen = func;
    }
}

/******************* History stuff **************************************/


static void
hist_init()
{
    int i;

    hist_pos = 0;
    hist_last = 0;
    hist_pre = 0;
    hist_buf[0] = "";
    for (i=1; i < HIST_SIZE; i++)
	hist_buf[i] = (char *)0;
}

void
gl_histadd(buf)
char *buf;
{
    char *p = buf;
    int len;

    /* in case we call gl_histadd() before we call gl_getline() */
    if (gl_init_done < 0) {		/* -1 only on startup */
        hist_init();
        gl_init_done = 0;
    }
    while (*p == ' ' || *p == '\t' || *p == '\n') 
	p++;
    if (*p) {
	len = strlen(buf);
	if (strchr(p, '\n')) 	/* previously line already has NL stripped */
	    len--;
	if (hist_pre == 0 || strlen(hist_pre) != len || 
			    strncmp(hist_pre, buf, len) != 0) {
            hist_buf[hist_last] = hist_save(buf);
	    hist_pre = hist_buf[hist_last];
            hist_last = (hist_last + 1) % HIST_SIZE;
            if (hist_buf[hist_last] && *hist_buf[hist_last]) {
	        free(hist_buf[hist_last]);
            }
	    hist_buf[hist_last] = "";
	}
    }
    hist_pos = hist_last;
}

void
gl_histclear()
{
    int i;

    for (i=0; i < HIST_SIZE; i++)
	if (hist_buf[i] && strlen (hist_buf[i])){
	    free(hist_buf[i]);
	    hist_buf[i] = NULL;
	}
    hist_init();
}

static char *
hist_prev()
/* loads previous hist entry into input buffer, sticks on first */
{
    char *p = 0;
    int   next = (hist_pos - 1 + HIST_SIZE) % HIST_SIZE;

    if (hist_buf[hist_pos] != 0 && next != hist_last) {
        hist_pos = next;
        p = hist_buf[hist_pos];
    } 
    if (p == 0) {
	p = "";
	gl_putc('\007');
    }
    return p;
}

static char *
hist_next()
/* loads next hist entry into input buffer, clears on last */
{
    char *p = 0;

    if (hist_pos != hist_last) {
        hist_pos = (hist_pos+1) % HIST_SIZE;
	p = hist_buf[hist_pos];
    } 
    if (p == 0) {
	p = "";
	gl_putc('\007');
    }
    return p;
}

static char *
hist_save(p)
char *p;
/* makes a copy of the string */
{
    char *s = 0;
    int   len = strlen(p);
    char *nl = strchr(p, '\n');

    if (nl) {
        if ((s = malloc (len)) != 0) {
            strncpy(s, p, len-1);
	    s[len-1] = 0;
	}
    } else {
        if ((s = malloc(len+1)) != 0) {
            strncpy(s, p, len+1);
        }
    }
    if (s == 0) 
	gl_error("\n*** Error: hist_save() failed on malloc\n");
    return s;
}

/******************* Search stuff **************************************/



static void  
search_update(int c)
{
    if (c == 0) {
	search_pos = 0;
        search_string[0] = 0;
        search_prompt[0] = '?';
        search_prompt[1] = ' ';
        search_prompt[2] = 0;
    } else if (c > 0) {
        search_string[search_pos] = c;
        search_string[search_pos+1] = 0;
        search_prompt[search_pos] = c;
        search_prompt[search_pos+1] = '?';
        search_prompt[search_pos+2] = ' ';
        search_prompt[search_pos+3] = 0;
	search_pos++;
    } else {
	if (search_pos > 0) {
	    search_pos--;
            search_string[search_pos] = 0;
            search_prompt[search_pos] = '?';
            search_prompt[search_pos+1] = ' ';
            search_prompt[search_pos+2] = 0;
	} else {
	    gl_putc('\007');
	    hist_pos = hist_last;
	}
    }
}

static void 
search_addchar(cligen_handle h, int c)
{
    char *loc;

    search_update(c);
    if (c < 0) {
	if (search_pos > 0) {
	    hist_pos = search_last;
	} else {
	    cligen_buf(h)[0] = 0;
	    hist_pos = hist_last;
	}
	strncpy(cligen_buf(h), hist_buf[hist_pos], gl_bufsize(h));
    }
    if ((loc = strstr(cligen_buf(h), search_string)) != 0) {
	gl_fixup(h, search_prompt, 0, loc - cligen_buf(h));
    } else if (search_pos > 0) {
        if (search_forw_flg) {
	    search_forw(h, 0);
        } else {
	    search_back(h, 0);
        }
    } else {
	gl_fixup(h, search_prompt, 0, 0);
    }
}

static void     
search_term(cligen_handle h)
{
    gl_search_mode = 0;
    if (cligen_buf(h)[0] == 0)		/* not found, reset hist list */
        hist_pos = hist_last;
    if (gl_in_hook)
	gl_in_hook(h, cligen_buf(h));
    gl_fixup(h, cligen_prompt(h), 0, gl_pos);
}

static void     
search_back(cligen_handle h, int new_search)
{
    int    found = 0;
    char  *p, *loc;

    search_forw_flg = 0;
    if (gl_search_mode == 0) {
	search_last = hist_pos = hist_last;	
	search_update(0);	
	gl_search_mode = 1;
        cligen_buf(h)[0] = 0;
	gl_fixup(h, search_prompt, 0, 0);
    } else if (search_pos > 0) {
	while (!found) {
	    p = hist_prev();
	    if (*p == 0) {		/* not found, done looking */
	       cligen_buf(h)[0] = 0;
	       gl_fixup(h, search_prompt, 0, 0);
	       found = 1;
	    } else if ((loc = strstr(p, search_string)) != 0) {
		strncpy(cligen_buf(h), p, gl_bufsize(h));
		gl_fixup(h, search_prompt, 0, loc - p);
	       if (new_search)
		   search_last = hist_pos;
	       found = 1;
	    } 
	}
    } else {
        gl_putc('\007');
    }
}

static void     
search_forw(cligen_handle h, int new_search)
{
    int    found = 0;
    char  *p, *loc;

    search_forw_flg = 1;
    if (gl_search_mode == 0) {
	search_last = hist_pos = hist_last;	
	search_update(0);	
	gl_search_mode = 1;
        cligen_buf(h)[0] = 0;
	gl_fixup(h, search_prompt, 0, 0);
    } else if (search_pos > 0) {
	while (!found) {
	    p = hist_next();
	    if (*p == 0) {		/* not found, done looking */
	       cligen_buf(h)[0] = 0;
	       gl_fixup(h, search_prompt, 0, 0);
	       found = 1;
	    } else if ((loc = strstr(p, search_string)) != 0) {
		strncpy(cligen_buf(h), p, gl_bufsize(h));
		gl_fixup(h, search_prompt, 0, loc - p);
	       if (new_search)
		   search_last = hist_pos;
	       found = 1;
	    } 
	}
    } else {
        gl_putc('\007');
    }
}

