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
#include <stdlib.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>

#include "cligen_buf.h"
#include "cligen_cv.h"
#include "cligen_cvec.h"
#include "cligen_parsetree.h"
#include "cligen_pt_head.h"
#include "cligen_callback.h"
#include "cligen_object.h"
#include "cligen_io.h"
#include "cligen_handle.h"
#include "cligen_history_internal.h"

#include "cligen_getline.h" /* exported interface */

/********************* exported variables ********************************/

int (*gl_in_hook)() = NULL;
int (*gl_out_hook)() = NULL;
int (*gl_tab_hook)() = NULL;
int (*gl_qmark_hook)() = NULL;
int (*gl_susp_hook)() = NULL;
int (*gl_interrupt_hook)() = NULL;

/******************** internal interface *********************************/

/* begin forward declared internal functions */
static void     gl_init1(void);		/* prepare to edit a line */
static void     gl_cleanup(void);	/* to undo gl_init1 */
void            gl_char_init(void);	/* get ready for no echo input */
void            gl_char_cleanup(void);	/* undo gl_char_init */
static size_t 	(*gl_strlen)() = (size_t(*)())strlen; 
					/* returns printable prompt width */

static int      gl_addchar(cligen_handle h, int c);	/* install specified char */
static void     gl_del(cligen_handle h, int loc);	/* del, either left (-1) or cur (0) */
static inline void gl_fixup(cligen_handle h, char*,int,int);/* fixup state variables and screen */
static int      gl_getc(cligen_handle h);	        /* read one char from terminal */
static void     gl_kill(cligen_handle h, int pos);	/* delete to EOL */
static void     gl_kill_begin(cligen_handle h, int pos);	/* delete to BEGIN of line */
static int      gl_kill_word(cligen_handle h, int pos);	/* delete word */
static void     gl_newline(cligen_handle);	/* handle \n or \r */
static int      gl_puts(char *buf);	/* write a line to terminal */

static void     gl_transpose(cligen_handle h);	/* transpose two chars */
static int      gl_yank(cligen_handle h);		/* yank killed text */
static void     gl_word(cligen_handle h, int dir);	/* move a word */

static void     search_addchar(cligen_handle h, int c);	/* increment search string */
static void     search_term(cligen_handle);	/* reset with current contents */
static void     search_back(cligen_handle h, int new);	/* look back for current string */
static void     search_forw(cligen_handle h, int new);	/* look forw for current string */
/* end forward declared internal functions */

/* begin global variables */
static int      gl_init_done = -1;	/* terminal mode flag  */
static int      gl_termw = 80;		/* actual terminal width */
static int      gl_utf8 = 0;		/* UTF-8 experimental mode */
static int      gl_scrolling_mode = 1;	/* Scrolling on / off */
static int      gl_scrollw = 27;	/* width of EOL scrolling region */
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

#define SEARCH_LEN 100
static char  search_prompt[SEARCH_LEN+2];  /* prompt includes search string */
static char  search_string[SEARCH_LEN];
static int   search_pos = 0;      /* current location in search_string */
static int   search_forw_flg = 0; /* search direction flag */
static int   search_last = 0;	  /* last match found */

/* end global variables */


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

#if defined(__unix__) || defined(__APPLE__)
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
    tcsetattr(0, TCSADRAIN, &new_termios);
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
    tcsetattr(0, TCSADRAIN, &old_termios);
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
int pc_keymap(int c)
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
gl_regfd(int fd, 
	 cligen_fd_cb_t *cb, 
	 void *arg)
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
    fd_set fdset = {0,};

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

/*! Initiate an exit, not actually made, but gl_exiting() will return 1.
 * @param[in]  h     CLIgen handle
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

/*! Get a character without echoing it to screen 
 * @param[in]  h     CLIgen handle
 */
static int
gl_getc(cligen_handle h)
{
    int             c;
#ifdef __unix__
    unsigned char  ch;
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
    c = getkey() ;
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

int
gl_putc(int c)
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
gl_puts(char *buf)
{
    int len; 
    
    if (buf) {
        len = strlen(buf);
        if (write(1, buf, len) < 0)
	  return -1;
    }
    return 0;
}

/*! set up variables and terminal 
 * @see gl_init  cal this once first
 */
static void
gl_init1()
{
    gl_iseof = 0;
    gl_char_init();
    gl_init_done = 1;
}

/*! undo effects of gl_init1, as necessary */
static void
gl_cleanup()
{
    if (gl_init_done > 0)
        gl_char_cleanup();
    gl_init_done = 0;
}

int
gl_getscrolling(void)
{
    return gl_scrolling_mode;
}

void
gl_setscrolling(int  mode)
{
    gl_scrolling_mode = mode;
}

int
gl_getwidth(void)
{
    return gl_termw;
}

/*! Set UTF-8 experimental mode 
 * @param[in] enabled   Set to 1 to enable UTF-8 experimental mode
 */
int
gl_utf8_set(int mode)
{
    gl_utf8 = mode;
    return 0;
}

/*! get UTF-8 experimental mode 
 * @retval 0 UTF-8 is disabled
 * @retval 1 UTF-8 is enabled
 */
int
gl_utf8_get(void)
{
    return gl_utf8;
}

int
gl_setwidth(int  w)
{
    if (w < TERM_MIN_SCREEN_WIDTH)
	return -1;
    gl_termw = w;
    gl_scrollw = w / 3;
    return 0;
}

/*! Main getline function handling a command line
 *
 * @param[in]  h     CLIgen handle
 * @param[out] buf   Pointer to char* buffer containing CLIgen command
 * @retval     0     OK: string or EOF
 * @retval    -1     Error
 * Typically called by cliread.
 */
int
gl_getline(cligen_handle h,
	   char        **buf)
{
    int             c, loc, tmp;
    char           *gl_prompt;
    int             escape = 0;
#ifdef __unix__
    int	            sig;
#endif

    gl_init1();	
    gl_prompt = (cligen_prompt(h))? cligen_prompt(h) : "";
    cligen_buf(h)[0] = 0;
    if (gl_in_hook)
	gl_in_hook(h, cligen_buf(h));
    gl_fixup(h, gl_prompt, -2, cligen_buf_size(h));
    while ((c = gl_getc(h)) >= 0) {
	gl_extent = 0;  	/* reset to full extent */
	if (isprint(c) || (escape && c=='\n')) {
	    if (escape == 0 && c == '\\')
               escape++;
            else{
		if (escape ==0 && c == '?' && gl_qmark_hook) {
		    escape = 0;
		    if ((loc = gl_qmark_hook(h, cligen_buf(h))) < 0)
			goto err;
		    gl_fixup(h, gl_prompt, -2, gl_pos);
		}
		else{ 
		    escape = 0;
		    if (gl_search_mode)
			search_addchar(h, c);
		    else
			if (gl_addchar(h, c) < 0)
			    goto err;
		}
	    }
	} else {
	    escape = 0;
	    if (gl_search_mode) { /* after ^S or ^R */
	        if (c == '\033') { /* ESC */
	            search_term(h);
		}
		else  if (c == '\016' || c == '\020') { /* ^N, ^P */
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
		goto exit;
	    switch (c) {
	    case '\n': case '\r': 			/* newline */
		gl_newline(h);
		goto done;
		/*NOTREACHED*/
		break; 
	    case '\001': gl_fixup(h, gl_prompt, -1, 0);		/* ^A */
		break;
	    case '\002': gl_fixup(h, gl_prompt, -1, gl_pos-1);	/* ^B */
		break;
	    case '\004':					/* ^D */
		if (gl_cnt == 0) 
		    goto exit;
		else 
		    gl_del(h, 0);
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
	            if ((loc = gl_tab_hook(h, &tmp)) < 0)
			goto err;
		    gl_fixup(h, gl_prompt, -2, tmp);
#if 0
                 if (loc != -1 || tmp != gl_pos)
                       gl_fixup(h, gl_prompt, loc, tmp);
#endif
                }
		break;
	    case '\013': gl_kill(h, gl_pos);			/* ^K */
		break;
	    case '\014':
		    gl_clear_screen(h);				/* ^L */
		break;
	    case '\016': 					/* ^N */
		hist_copy_next(h);
                if (gl_in_hook)
	            gl_in_hook(h, cligen_buf(h));
		gl_fixup(h, gl_prompt, 0, cligen_buf_size(h));
		break;
	    case '\017': gl_overwrite = !gl_overwrite;       	/* ^O */
		break;
	    case '\020': 					/* ^P */
		hist_copy_prev(h);
                if (gl_in_hook)
	            gl_in_hook(h, cligen_buf(h));
		gl_fixup(h, gl_prompt, 0, cligen_buf_size(h));
		break;
	    case '\022': search_back(h, 1);			/* ^R */
		break;
	    case '\023': search_forw(h, 1);			/* ^S */
		break;
	    case '\024': gl_transpose(h);			/* ^T */
		break;
	    case '\025': gl_kill_begin(h, gl_pos);		/* ^U */
		break;
	    case '\027': if (gl_kill_word(h, gl_pos) < 0) goto err;/* ^W */
		break;
	    case '\031': if (gl_yank(h) < 0) goto err;		/* ^Y */
		break;
	    case '\032':                                      /* ^Z */
		if(gl_susp_hook) {
		    tmp = gl_pos;
	            loc = gl_susp_hook(cligen_userhandle(h)?cligen_userhandle(h):h,
				       cligen_buf(h), gl_strlen(gl_prompt), &tmp);
	            if (loc != -1 || tmp != gl_pos)
	                gl_fixup(h, gl_prompt, loc, tmp);
		    if (strchr (cligen_buf(h), '\n')) 
			goto done;
		}
		break;
	    case '\033':	/* ansi arrow keys (ESC) */
		c = gl_getc(h);
		/* ESC-[ is normal and ESC-O is application cursor keys */
		if (c == '[' || c == 'O') { 
		    switch(c = gl_getc(h)) {
		    case 'A':             			/* up */
			hist_copy_prev(h);
                        if (gl_in_hook)
	                    gl_in_hook(h, cligen_buf(h));
		        gl_fixup(h, gl_prompt, 0, cligen_buf_size(h));
		        break;
		    case 'B':                         	/* down */
			hist_copy_next(h);
                        if (gl_in_hook)
	                    gl_in_hook(h, cligen_buf(h));
		        gl_fixup(h, gl_prompt, 0, cligen_buf_size(h));
		        break;
		    case 'C': gl_fixup(h, gl_prompt, -1, gl_pos+1); /* right */
		        break;
		    case 'D': gl_fixup(h, gl_prompt, -1, gl_pos-1); /* left */
		        break;
		    case 'H': gl_fixup(h, gl_prompt, -1, 0); /* home */
		        break;
		    case 'F': gl_fixup(h, gl_prompt, -1, cligen_buf_size(h)); /* end */
		        break;
		    case '3': /* del */
			if (gl_getc(h) != '~')
			    break;
			gl_del(h, 0);
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
		if ((c & 0xe0) == 0xc0){ /* UTF-2 */
		    int c2;
		    c2 = gl_getc(h);
		    if (gl_utf8){
			if (gl_addchar(h, c) < 0)
			    goto err;
			if (gl_addchar(h, c2) < 0)
			    goto err;
		    }
		}
		else if ((c & 0xf0) == 0xe0){ /* UTF-2 */
		    int c2, c3;
		    c2 = gl_getc(h);
		    c3 = gl_getc(h);
		    if (gl_utf8){
			if (gl_addchar(h, c) < 0)
			    goto err;
			if (gl_addchar(h, c2) < 0)
			    goto err;
			if (gl_addchar(h, c3) < 0)
			    goto err;
		    }
		}
		else if ((c & 0xf8) == 0xf0){ /* UTF-3 */
		    int c2, c3, c4;
		    c2 = gl_getc(h);
		    c3 = gl_getc(h);
		    c4 = gl_getc(h);
		    if (gl_utf8){
			if (gl_addchar(h, c) < 0)
			    goto err;
			if (gl_addchar(h, c2) < 0)
			    goto err;
			if (gl_addchar(h, c3) < 0)
			    goto err;
			if (gl_addchar(h, c4) < 0)
			    goto err;
		    }
		}
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
	                gl_init1();
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
    } /* while */
    cligen_buf(h)[0] = 0;
 done:
    gl_cleanup();
    *buf = cligen_buf(h);
    return 0;
 exit: /* ie exit from cli, not necessarily error */
    gl_exit(h);
    *buf = cligen_buf(h);
    return 0;
 err: /* fatal error */
    gl_cleanup();
    return -1;
}

/*! Add the character c to the input buffer at current location 
 * @param[in]  h     CLIgen handle
 * @param[in]  c     Character
 */
static int
gl_addchar(cligen_handle h, 
	   int           c)
{
    int  i;

    if (cligen_buf_increase(h, gl_cnt+1) < 0) /* assume increase enough for gl_pos-gl_cnt */
	return -1;
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
    return 0;
}

/*! Add the kill buffer to the input buffer at current location 
 * @param[in]  h     CLIgen handle
 */
static int
gl_yank(cligen_handle h)
{
    int  i, len;

    len = strlen(cligen_killbuf(h));
    if (len > 0) {
	if (gl_overwrite == 0) {
	    if (cligen_buf_increase(h, gl_cnt + len + 1) < 0)
		return -1;
            for (i=gl_cnt; i >= gl_pos; i--)
                cligen_buf(h)[i+len] = cligen_buf(h)[i];
	    for (i=0; i < len; i++)
                cligen_buf(h)[gl_pos+i] = cligen_killbuf(h)[i];
            gl_fixup(h, cligen_prompt(h), gl_pos, gl_pos+len);
	} else {
	    if (gl_pos + len > gl_cnt) {
	        if (cligen_buf_increase(h, gl_pos + len + 1) < 0)
		    return -1;
		cligen_buf(h)[gl_pos + len] = 0;
            }
	    for (i=0; i < len; i++)
                cligen_buf(h)[gl_pos+i] = cligen_killbuf(h)[i];
	    gl_extent = len;
            gl_fixup(h, cligen_prompt(h), gl_pos, gl_pos+len);
	}
    } else
	gl_putc('\007');
    return 0;
}

/*! Switch character under cursor and to left of cursor 
 * @param[in]  h     CLIgen handle
 */
static void
gl_transpose(cligen_handle h)
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

/*! Cleans up entire line before returning to caller. 
 *
 * @param[in]  h     CLIgen handle
 * A \n is appended. If line longer than screen, redraw starting at beginning
 */
static void
gl_newline(cligen_handle h)
{
    int len = gl_cnt;
    int loc;

    if (gl_scrolling_mode)
	loc = gl_width - 5;	/* shifts line back to start position */
    else
	loc = gl_cnt;
    
    cligen_buf_increase(h, gl_cnt+1); /* \n\0 added */
    if (gl_out_hook) {
        len = strlen(cligen_buf(h));
    } 
    if (loc > len)
	loc = len;
    gl_fixup(h, cligen_prompt(h), -1, loc);	/* must do this before appending \n */
    cligen_buf(h)[len] = '\n';
    cligen_buf(h)[len+1] = '\0';
    gl_putc('\n');
}

/*! Delete a character.  
 * @param[in] h    CLIgen handle
 * @param[in] loc  -1 : delete character to left of cursor
 *                 0 : delete character under cursor
 */
static void
gl_del(cligen_handle h, 
       int           loc)
{
    int i;

    if ((loc == -1 && gl_pos > 0) || (loc == 0 && gl_pos < gl_cnt)) {
        for (i=gl_pos+loc; i < gl_cnt; i++)
	    cligen_buf(h)[i] = cligen_buf(h)[i+1];
	gl_fixup(h, cligen_prompt(h), gl_pos+loc, gl_pos+loc);
    } else
	gl_putc('\007');
}

/*! Delete position to end of line 
 * @param[in]  h     CLIgen handle
 * @param[in]  pos   Delete from pos to the end of line 
 */
static void
gl_kill(cligen_handle h, 
	int           pos)
{
    if (pos < gl_cnt) {
        cligen_killbuf_increase(h, cligen_buf_size(h));
	strncpy(cligen_killbuf(h), cligen_buf(h) + pos, cligen_buf_size(h));
	cligen_buf(h)[pos] = '\0';
	gl_fixup(h, cligen_prompt(h), pos, pos);
    } else
	gl_putc('\007');
}

/* Delete from pos to start of line 
 * @param[in]  h     CLIgen handle
 * @param[in]  pos   Delete from pos to start of line 
 */
static void
gl_kill_begin(cligen_handle h, 
	      int           pos)
{
    int i;
    int len;

    if (pos != 0) {
	len = strlen(cligen_buf(h));
	cligen_killbuf_increase(h, pos);
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

/*! Delete one previous word from pos 
 * @param[in]  h     CLIgen handle
 * @param[in]  pos   Delete one previous word from pos 
 */
static int
gl_kill_word(cligen_handle h, 
	     int           pos)
{
    int i, wpos;

    if (pos == 0) 
	gl_putc('\007');
    else {
	wpos = pos;
	if (pos > 0)
	    pos--;
	while (isspace((int)cligen_buf(h)[pos]) && pos > 0)
	    pos--;
        while (!isspace((int)cligen_buf(h)[pos]) && pos > 0) 
	    pos--;
	if (pos < gl_cnt && isspace((int)cligen_buf(h)[pos]))   /* move onto word */
	    pos++;
	if (cligen_killbuf_increase(h, wpos-pos) < 0)
	    return -1;
	strncpy(cligen_killbuf(h), cligen_buf(h)+pos, wpos-pos);
	cligen_killbuf(h)[wpos-pos] = '\0';
	memmove(cligen_buf(h)+pos, cligen_buf(h) + wpos, gl_cnt-wpos+1);
	gl_fixup(h, cligen_prompt(h), wpos, pos);
	for (i=gl_pos; i < gl_cnt; i++)
            gl_putc(cligen_buf(h)[i]);
	gl_fixup(h, cligen_prompt(h), -2, pos);
    }
    return 0;
}

/*! Move forward or backword one word 
 * @param[in]  h          CLIgen handle
 * @param[in]  direction  >0 forward; else backward
 */
static void
gl_word(cligen_handle h, 
	int           direction)

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

static int
move_cursor_up(int nr)
{
    gl_putc(033);
    gl_putc('[');
    gl_putc('1');
    gl_putc('A');
    return 0;
}

static int
move_cursor_right(int nr)
{
    char   str[16];
    int    i;
    size_t len;
    
    gl_putc(033);
    gl_putc('[');
    snprintf(str, 15, "%d", nr);
    len = strlen(str);
    for (i=0; i<len; i++)
	gl_putc(str[i]);
    gl_putc('C');
    return 0;
}

static int
wrap_line()
{
    gl_putc('\n'); /* wrap line */
    return 0;
}

static int
unwrap_line()
{
    move_cursor_up(1);
    move_cursor_right(gl_termw-1);
    return 0;
}

int
wrap(int p, 
     int plen)
{
    return (p+plen+1)%gl_termw==0;
}

void gl_clear_screen(cligen_handle h)
{
    if (gl_init_done <= 0) {
	return;
    }

    gl_putc('\033');	/* clear */
    gl_putc('[');
    gl_putc('2');
    gl_putc('J');

    gl_putc('\033');	/* home */
    gl_putc('[');
    gl_putc('H');

    gl_fixup(h, cligen_prompt(h), -2, gl_pos);
}

/*! Emit a newline, reset and redraw prompt and current input line 
 * @param[in]  h     CLIgen handle
*/
void
gl_redraw(cligen_handle h)
{
    if (gl_init_done > 0) {
        gl_putc('\n');
        gl_fixup(h, cligen_prompt(h), -2, gl_pos);
    }
}

/*! Redrawing or moving within line
 *
 * This function is used both for redrawing when input changes or for
 * moving within the input line.  The parameters are:
 *
 * @param[in] h       CLIgen handle
 * @param[in] prompt  Compared to last_prompt[] for changes;
 * @param[in] change  Index of the start of changes in the input buffer,
 *                    with -1 indicating no changes, -2 indicating we're on
 *                    a new line, redraw everything.
 * @param[in] cursor  The desired location of the cursor after the call.
 *                    A value of cligen_buf_size(h) can be used  to indicate 
 *                    the cursor should move just past the end of the input line.
 */
static void
gl_fixup_noscroll(cligen_handle h, 
		  char         *prompt, 
		  int           change, 
		  int           cursor)
{
    int          left = 0, right = -1;		/* bounds for redraw */
    int          pad;		/* how much to erase at end of line */
    int          backup;        /* how far to backup before fixing */
    int          i;
    int          p; /* pos */
    int          new_right = -1; /* alternate right bound, using gl_extent */
    int          l1, l2;
    int          plen=strlen(prompt);

    if (change == -2) {   /* reset */
	gl_pos = gl_cnt = fixup_gl_shift = fixup_off_right = fixup_off_left = 0;
	gl_putc('\r');
	gl_puts(prompt);
	strncpy(fixup_last_prompt, prompt, sizeof(fixup_last_prompt)-1);
	change = 0;
        gl_width = gl_termw - gl_strlen(prompt);
    } else if (strcmp(prompt, fixup_last_prompt) != 0) {
	l1 = gl_strlen(fixup_last_prompt);
	l2 = gl_strlen(prompt);
	gl_cnt = gl_cnt + l1 - l2;
	strncpy(fixup_last_prompt, prompt, sizeof(fixup_last_prompt)-1);
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
	if (cursor != cligen_buf_size(h))		/* cligen_buf_size(h) means end of line */
	    gl_putc('\007');
	cursor = gl_cnt;
    }
    if (cursor < 0) {
	gl_putc('\007');
	cursor = 0;
    }
    if (change >= 0) {		/* text changed */
	if (change < fixup_gl_shift + fixup_off_left) {
	    left = fixup_gl_shift;
	} else {
	    left = change;
	    backup = gl_pos - change;
	}
	right = gl_cnt;
	new_right = (gl_extent && (right > left + gl_extent))? 
	    left + gl_extent : right;
    }
    pad -= gl_cnt - fixup_gl_shift;
    pad = (pad < 0)? 0 : pad;
    if (left <= right) {		/* clean up screen */
	for (p=left+backup-1; p >= left; p--){
	    if (wrap(p, plen))
		unwrap_line();
	    else
		gl_putc('\b');
	}
	if (left == fixup_gl_shift && fixup_off_left) {
	    gl_putc('$');
	    left++;
        }
	for (p=left; p < new_right; p++){
	    gl_putc(cligen_buf(h)[p]);
	    if (wrap(p, plen))
		wrap_line();
	}
	gl_pos = new_right;
	for (p=new_right; p < new_right+pad; p++){ /* erase remains of prev line */
	    gl_putc(' ');
	    if (wrap(p, plen))
		wrap_line();
	}
	gl_pos += pad;
    }
    /* move to final cursor location */
    if (gl_pos - cursor > 0) {
	for (p=gl_pos; p > cursor; p--){
	    if (wrap(p-1, plen))
		unwrap_line();
	    else
		gl_putc('\b');
	} 
    }
    else {
	for (i=gl_pos; i < cursor; i++)
	    gl_putc(cligen_buf(h)[i]);
    }
    gl_pos = cursor;
}


/*! Redrawing or moving within line
 *
 * This function is used both for redrawing when input changes or for
 * moving within the input line.  The parameters are:
 *   prompt:  compared to last_prompt[] for changes;
 *   change : the index of the start of changes in the input buffer,
 *            with -1 indicating no changes, -2 indicating we're on
 *            a new line, redraw everything.
 *   cursor : the desired location of the cursor after the call.
 *            A value of cligen_buf_size(h) can be used  to indicate the cursor should
 *            move just past the end of the input line.
 */
static void
gl_fixup_scroll(cligen_handle h, 
		char         *prompt, 
		int           change, 
		int           cursor)
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
	strncpy(fixup_last_prompt, prompt, sizeof(fixup_last_prompt)-1);
	change = 0;
        gl_width = gl_termw - gl_strlen(prompt);
    } else if (strcmp(prompt, fixup_last_prompt) != 0) {
	l1 = gl_strlen(fixup_last_prompt);
	l2 = gl_strlen(prompt);
	gl_cnt = gl_cnt + l1 - l2;
	strncpy(fixup_last_prompt, prompt, sizeof(fixup_last_prompt)-1);
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
	if (cursor != cligen_buf_size(h))		/* cligen_buf_size(h) means end of line */
	    gl_putc('\007');
	cursor = gl_cnt;
    }
    if (cursor < 0) {
	gl_putc('\007');
	cursor = 0;
    }
    if (fixup_off_right || (fixup_off_left && cursor < fixup_gl_shift + gl_width - gl_scrollw / 2)){
	extra = 2;			/* shift the scrolling boundary */
    }
    else 
	extra = 0;
    
    new_shift = cursor + extra + gl_scrollw - gl_width;
    if (new_shift > 0) {
	new_shift /= gl_scrollw;
	new_shift *= gl_scrollw;
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

static inline void
gl_fixup(cligen_handle h, 
	 char         *prompt, 
	 int           change, 
	 int           cursor)
{
    if (gl_scrolling_mode)
	return gl_fixup_scroll(h, prompt, change, cursor);
    else
	return gl_fixup_noscroll(h, prompt, change, cursor);
}

/******************* strlen stuff **************************************/

void 
gl_strwidth(size_t (*func)())
{
    if (func != 0) {
	gl_strlen = func;
    }
}


/******************* Search stuff **************************************/


static void  
search_update(cligen_handle h,
	      int           c)
{
    if (c == 0) {
	search_pos = 0;
        search_string[0] = 0;
        search_prompt[0] = '?';
        search_prompt[1] = ' ';
        search_prompt[2] = 0;
    } else if (c > 0){
	if (search_pos+1 < SEARCH_LEN) {
	    search_string[search_pos] = c;
	    search_string[search_pos+1] = 0;
	    search_prompt[search_pos] = c;
	    search_prompt[search_pos+1] = '?';
	    search_prompt[search_pos+2] = ' ';
	    search_prompt[search_pos+3] = 0;
	    search_pos++;
	}
    } else {
	if (search_pos > 0) {
	    search_pos--;
            search_string[search_pos] = 0;
            search_prompt[search_pos] = '?';
            search_prompt[search_pos+1] = ' ';
            search_prompt[search_pos+2] = 0;
	} else {
	    gl_putc('\007');
	    hist_pos_set(h, hist_last_get(h));
	}
    }
}

/*! Search addchar
 * @param[in]  h     CLIgen handle
 */
static void 
search_addchar(cligen_handle h, 
	       int           c)
{
    char *loc;

    search_update(h, c);
    if (c < 0) {
	if (search_pos > 0) {
	    hist_pos_set(h, search_last);
	} else {
	    cligen_buf(h)[0] = 0;
	    hist_pos_set(h, hist_last_get(h));
	}
	hist_copy_pos(h);
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

/*! Search terminate
 * @param[in]  h     CLIgen handle
 */
static void     
search_term(cligen_handle h)
{
    gl_search_mode = 0;
    if (cligen_buf(h)[0] == 0)		/* not found, reset hist list */
        hist_pos_set(h, hist_last_get(h));
    if (gl_in_hook)
	gl_in_hook(h, cligen_buf(h));
    gl_fixup(h, cligen_prompt(h), 0, gl_pos);
}

/*! Search backwards
 * @param[in]  h     CLIgen handle
 */
static void     
search_back(cligen_handle h, 
	    int           new_search)
{
    int    found = 0;
    char  *p, *loc;
    int    last;

    search_forw_flg = 0;
    if (gl_search_mode == 0) {
	last = hist_last_get(h);
	hist_pos_set(h, last);
	search_last = last;	
	search_update(h, 0);
	gl_search_mode = 1;
        cligen_buf(h)[0] = 0;
	gl_fixup(h, search_prompt, 0, 0);
    } else if (search_pos > 0) {
	while (!found) {
	    p = hist_prev(h);
	    if (*p == 0) {		/* not found, done looking */
	       cligen_buf(h)[0] = 0;
	       gl_fixup(h, search_prompt, 0, 0);
	       found = 1;
	    } else if ((loc = strstr(p, search_string)) != 0) {
		strncpy(cligen_buf(h), p, cligen_buf_size(h));
		gl_fixup(h, search_prompt, 0, loc - p);
	       if (new_search)
		   search_last = hist_pos(h);
	       found = 1;
	    } 
	}
    } else {
        gl_putc('\007');
    }
}

/*! Search forward
 * @param[in]  h     CLIgen handle
 */
static void     
search_forw(cligen_handle h, 
	    int           new_search)
{
    int    found = 0;
    char  *p, *loc;
    int    last;

    search_forw_flg = 1;
    if (gl_search_mode == 0) {
	last = hist_last_get(h);
	hist_pos_set(h, last);
	search_last = last;

	search_update(h, 0);	
	gl_search_mode = 1;
        cligen_buf(h)[0] = 0;
	gl_fixup(h, search_prompt, 0, 0);
    } else if (search_pos > 0) {
	while (!found) {
	    p = hist_next(h);
	    if (*p == 0) {		/* not found, done looking */
	       cligen_buf(h)[0] = 0;
	       gl_fixup(h, search_prompt, 0, 0);
	       found = 1;
	    } else if ((loc = strstr(p, search_string)) != 0) {
		strncpy(cligen_buf(h), p, cligen_buf_size(h));
		gl_fixup(h, search_prompt, 0, loc - p);
	       if (new_search)
		   search_last = hist_pos(h);
	       found = 1;
	    } 
	}
    } else {
        gl_putc('\007');
    }
}

