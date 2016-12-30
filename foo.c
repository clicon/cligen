# 1 "cligen_var.c"
# 1 "/home/olof/src/cligen//"
# 1 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 1 "<command-line>" 2
# 1 "cligen_var.c"
# 24 "cligen_var.c"
# 1 "cligen_config.h" 1
# 87 "cligen_config.h"
# 1 "./cligen_custom.h" 1
# 22 "./cligen_custom.h"
typedef struct {int a;} *cligen_handle;
# 87 "cligen_config.h" 2
# 25 "cligen_var.c" 2

# 1 "/usr/include/stdio.h" 1 3 4
# 27 "/usr/include/stdio.h" 3 4
# 1 "/usr/include/features.h" 1 3 4
# 374 "/usr/include/features.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/sys/cdefs.h" 1 3 4
# 385 "/usr/include/x86_64-linux-gnu/sys/cdefs.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/wordsize.h" 1 3 4
# 386 "/usr/include/x86_64-linux-gnu/sys/cdefs.h" 2 3 4
# 375 "/usr/include/features.h" 2 3 4
# 398 "/usr/include/features.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/gnu/stubs.h" 1 3 4
# 10 "/usr/include/x86_64-linux-gnu/gnu/stubs.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/gnu/stubs-64.h" 1 3 4
# 11 "/usr/include/x86_64-linux-gnu/gnu/stubs.h" 2 3 4
# 399 "/usr/include/features.h" 2 3 4
# 28 "/usr/include/stdio.h" 2 3 4





# 1 "/usr/lib/gcc/x86_64-linux-gnu/4.8/include/stddef.h" 1 3 4
# 212 "/usr/lib/gcc/x86_64-linux-gnu/4.8/include/stddef.h" 3 4
typedef long unsigned int size_t;
# 34 "/usr/include/stdio.h" 2 3 4

# 1 "/usr/include/x86_64-linux-gnu/bits/types.h" 1 3 4
# 27 "/usr/include/x86_64-linux-gnu/bits/types.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/wordsize.h" 1 3 4
# 28 "/usr/include/x86_64-linux-gnu/bits/types.h" 2 3 4


typedef unsigned char __u_char;
typedef unsigned short int __u_short;
typedef unsigned int __u_int;
typedef unsigned long int __u_long;


typedef signed char __int8_t;
typedef unsigned char __uint8_t;
typedef signed short int __int16_t;
typedef unsigned short int __uint16_t;
typedef signed int __int32_t;
typedef unsigned int __uint32_t;

typedef signed long int __int64_t;
typedef unsigned long int __uint64_t;







typedef long int __quad_t;
typedef unsigned long int __u_quad_t;
# 121 "/usr/include/x86_64-linux-gnu/bits/types.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/typesizes.h" 1 3 4
# 122 "/usr/include/x86_64-linux-gnu/bits/types.h" 2 3 4


typedef unsigned long int __dev_t;
typedef unsigned int __uid_t;
typedef unsigned int __gid_t;
typedef unsigned long int __ino_t;
typedef unsigned long int __ino64_t;
typedef unsigned int __mode_t;
typedef unsigned long int __nlink_t;
typedef long int __off_t;
typedef long int __off64_t;
typedef int __pid_t;
typedef struct { int __val[2]; } __fsid_t;
typedef long int __clock_t;
typedef unsigned long int __rlim_t;
typedef unsigned long int __rlim64_t;
typedef unsigned int __id_t;
typedef long int __time_t;
typedef unsigned int __useconds_t;
typedef long int __suseconds_t;

typedef int __daddr_t;
typedef int __key_t;


typedef int __clockid_t;


typedef void * __timer_t;


typedef long int __blksize_t;




typedef long int __blkcnt_t;
typedef long int __blkcnt64_t;


typedef unsigned long int __fsblkcnt_t;
typedef unsigned long int __fsblkcnt64_t;


typedef unsigned long int __fsfilcnt_t;
typedef unsigned long int __fsfilcnt64_t;


typedef long int __fsword_t;

typedef long int __ssize_t;


typedef long int __syscall_slong_t;

typedef unsigned long int __syscall_ulong_t;



typedef __off64_t __loff_t;
typedef __quad_t *__qaddr_t;
typedef char *__caddr_t;


typedef long int __intptr_t;


typedef unsigned int __socklen_t;
# 36 "/usr/include/stdio.h" 2 3 4
# 44 "/usr/include/stdio.h" 3 4
struct _IO_FILE;



typedef struct _IO_FILE FILE;





# 64 "/usr/include/stdio.h" 3 4
typedef struct _IO_FILE __FILE;
# 74 "/usr/include/stdio.h" 3 4
# 1 "/usr/include/libio.h" 1 3 4
# 31 "/usr/include/libio.h" 3 4
# 1 "/usr/include/_G_config.h" 1 3 4
# 15 "/usr/include/_G_config.h" 3 4
# 1 "/usr/lib/gcc/x86_64-linux-gnu/4.8/include/stddef.h" 1 3 4
# 16 "/usr/include/_G_config.h" 2 3 4




# 1 "/usr/include/wchar.h" 1 3 4
# 82 "/usr/include/wchar.h" 3 4
typedef struct
{
  int __count;
  union
  {

    unsigned int __wch;



    char __wchb[4];
  } __value;
} __mbstate_t;
# 21 "/usr/include/_G_config.h" 2 3 4
typedef struct
{
  __off_t __pos;
  __mbstate_t __state;
} _G_fpos_t;
typedef struct
{
  __off64_t __pos;
  __mbstate_t __state;
} _G_fpos64_t;
# 32 "/usr/include/libio.h" 2 3 4
# 49 "/usr/include/libio.h" 3 4
# 1 "/usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdarg.h" 1 3 4
# 40 "/usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdarg.h" 3 4
typedef __builtin_va_list __gnuc_va_list;
# 50 "/usr/include/libio.h" 2 3 4
# 144 "/usr/include/libio.h" 3 4
struct _IO_jump_t; struct _IO_FILE;
# 154 "/usr/include/libio.h" 3 4
typedef void _IO_lock_t;





struct _IO_marker {
  struct _IO_marker *_next;
  struct _IO_FILE *_sbuf;



  int _pos;
# 177 "/usr/include/libio.h" 3 4
};


enum __codecvt_result
{
  __codecvt_ok,
  __codecvt_partial,
  __codecvt_error,
  __codecvt_noconv
};
# 245 "/usr/include/libio.h" 3 4
struct _IO_FILE {
  int _flags;




  char* _IO_read_ptr;
  char* _IO_read_end;
  char* _IO_read_base;
  char* _IO_write_base;
  char* _IO_write_ptr;
  char* _IO_write_end;
  char* _IO_buf_base;
  char* _IO_buf_end;

  char *_IO_save_base;
  char *_IO_backup_base;
  char *_IO_save_end;

  struct _IO_marker *_markers;

  struct _IO_FILE *_chain;

  int _fileno;



  int _flags2;

  __off_t _old_offset;



  unsigned short _cur_column;
  signed char _vtable_offset;
  char _shortbuf[1];



  _IO_lock_t *_lock;
# 293 "/usr/include/libio.h" 3 4
  __off64_t _offset;
# 302 "/usr/include/libio.h" 3 4
  void *__pad1;
  void *__pad2;
  void *__pad3;
  void *__pad4;
  size_t __pad5;

  int _mode;

  char _unused2[15 * sizeof (int) - 4 * sizeof (void *) - sizeof (size_t)];

};


typedef struct _IO_FILE _IO_FILE;


struct _IO_FILE_plus;

extern struct _IO_FILE_plus _IO_2_1_stdin_;
extern struct _IO_FILE_plus _IO_2_1_stdout_;
extern struct _IO_FILE_plus _IO_2_1_stderr_;
# 338 "/usr/include/libio.h" 3 4
typedef __ssize_t __io_read_fn (void *__cookie, char *__buf, size_t __nbytes);







typedef __ssize_t __io_write_fn (void *__cookie, const char *__buf,
     size_t __n);







typedef int __io_seek_fn (void *__cookie, __off64_t *__pos, int __w);


typedef int __io_close_fn (void *__cookie);
# 390 "/usr/include/libio.h" 3 4
extern int __underflow (_IO_FILE *);
extern int __uflow (_IO_FILE *);
extern int __overflow (_IO_FILE *, int);
# 434 "/usr/include/libio.h" 3 4
extern int _IO_getc (_IO_FILE *__fp);
extern int _IO_putc (int __c, _IO_FILE *__fp);
extern int _IO_feof (_IO_FILE *__fp) __attribute__ ((__nothrow__ , __leaf__));
extern int _IO_ferror (_IO_FILE *__fp) __attribute__ ((__nothrow__ , __leaf__));

extern int _IO_peekc_locked (_IO_FILE *__fp);





extern void _IO_flockfile (_IO_FILE *) __attribute__ ((__nothrow__ , __leaf__));
extern void _IO_funlockfile (_IO_FILE *) __attribute__ ((__nothrow__ , __leaf__));
extern int _IO_ftrylockfile (_IO_FILE *) __attribute__ ((__nothrow__ , __leaf__));
# 464 "/usr/include/libio.h" 3 4
extern int _IO_vfscanf (_IO_FILE * __restrict, const char * __restrict,
   __gnuc_va_list, int *__restrict);
extern int _IO_vfprintf (_IO_FILE *__restrict, const char *__restrict,
    __gnuc_va_list);
extern __ssize_t _IO_padn (_IO_FILE *, int, __ssize_t);
extern size_t _IO_sgetn (_IO_FILE *, void *, size_t);

extern __off64_t _IO_seekoff (_IO_FILE *, __off64_t, int, int);
extern __off64_t _IO_seekpos (_IO_FILE *, __off64_t, int);

extern void _IO_free_backup_area (_IO_FILE *) __attribute__ ((__nothrow__ , __leaf__));
# 75 "/usr/include/stdio.h" 2 3 4




typedef __gnuc_va_list va_list;
# 90 "/usr/include/stdio.h" 3 4
typedef __off_t off_t;
# 102 "/usr/include/stdio.h" 3 4
typedef __ssize_t ssize_t;







typedef _G_fpos_t fpos_t;




# 164 "/usr/include/stdio.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/stdio_lim.h" 1 3 4
# 165 "/usr/include/stdio.h" 2 3 4



extern struct _IO_FILE *stdin;
extern struct _IO_FILE *stdout;
extern struct _IO_FILE *stderr;







extern int remove (const char *__filename) __attribute__ ((__nothrow__ , __leaf__));

extern int rename (const char *__old, const char *__new) __attribute__ ((__nothrow__ , __leaf__));




extern int renameat (int __oldfd, const char *__old, int __newfd,
       const char *__new) __attribute__ ((__nothrow__ , __leaf__));








extern FILE *tmpfile (void) ;
# 209 "/usr/include/stdio.h" 3 4
extern char *tmpnam (char *__s) __attribute__ ((__nothrow__ , __leaf__)) ;





extern char *tmpnam_r (char *__s) __attribute__ ((__nothrow__ , __leaf__)) ;
# 227 "/usr/include/stdio.h" 3 4
extern char *tempnam (const char *__dir, const char *__pfx)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__malloc__)) ;








extern int fclose (FILE *__stream);




extern int fflush (FILE *__stream);

# 252 "/usr/include/stdio.h" 3 4
extern int fflush_unlocked (FILE *__stream);
# 266 "/usr/include/stdio.h" 3 4






extern FILE *fopen (const char *__restrict __filename,
      const char *__restrict __modes) ;




extern FILE *freopen (const char *__restrict __filename,
        const char *__restrict __modes,
        FILE *__restrict __stream) ;
# 295 "/usr/include/stdio.h" 3 4

# 306 "/usr/include/stdio.h" 3 4
extern FILE *fdopen (int __fd, const char *__modes) __attribute__ ((__nothrow__ , __leaf__)) ;
# 319 "/usr/include/stdio.h" 3 4
extern FILE *fmemopen (void *__s, size_t __len, const char *__modes)
  __attribute__ ((__nothrow__ , __leaf__)) ;




extern FILE *open_memstream (char **__bufloc, size_t *__sizeloc) __attribute__ ((__nothrow__ , __leaf__)) ;






extern void setbuf (FILE *__restrict __stream, char *__restrict __buf) __attribute__ ((__nothrow__ , __leaf__));



extern int setvbuf (FILE *__restrict __stream, char *__restrict __buf,
      int __modes, size_t __n) __attribute__ ((__nothrow__ , __leaf__));





extern void setbuffer (FILE *__restrict __stream, char *__restrict __buf,
         size_t __size) __attribute__ ((__nothrow__ , __leaf__));


extern void setlinebuf (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__));








extern int fprintf (FILE *__restrict __stream,
      const char *__restrict __format, ...);




extern int printf (const char *__restrict __format, ...);

extern int sprintf (char *__restrict __s,
      const char *__restrict __format, ...) __attribute__ ((__nothrow__));





extern int vfprintf (FILE *__restrict __s, const char *__restrict __format,
       __gnuc_va_list __arg);




extern int vprintf (const char *__restrict __format, __gnuc_va_list __arg);

extern int vsprintf (char *__restrict __s, const char *__restrict __format,
       __gnuc_va_list __arg) __attribute__ ((__nothrow__));





extern int snprintf (char *__restrict __s, size_t __maxlen,
       const char *__restrict __format, ...)
     __attribute__ ((__nothrow__)) __attribute__ ((__format__ (__printf__, 3, 4)));

extern int vsnprintf (char *__restrict __s, size_t __maxlen,
        const char *__restrict __format, __gnuc_va_list __arg)
     __attribute__ ((__nothrow__)) __attribute__ ((__format__ (__printf__, 3, 0)));

# 412 "/usr/include/stdio.h" 3 4
extern int vdprintf (int __fd, const char *__restrict __fmt,
       __gnuc_va_list __arg)
     __attribute__ ((__format__ (__printf__, 2, 0)));
extern int dprintf (int __fd, const char *__restrict __fmt, ...)
     __attribute__ ((__format__ (__printf__, 2, 3)));








extern int fscanf (FILE *__restrict __stream,
     const char *__restrict __format, ...) ;




extern int scanf (const char *__restrict __format, ...) ;

extern int sscanf (const char *__restrict __s,
     const char *__restrict __format, ...) __attribute__ ((__nothrow__ , __leaf__));
# 443 "/usr/include/stdio.h" 3 4
extern int fscanf (FILE *__restrict __stream, const char *__restrict __format, ...) __asm__ ("" "__isoc99_fscanf")

                               ;
extern int scanf (const char *__restrict __format, ...) __asm__ ("" "__isoc99_scanf")
                              ;
extern int sscanf (const char *__restrict __s, const char *__restrict __format, ...) __asm__ ("" "__isoc99_sscanf") __attribute__ ((__nothrow__ , __leaf__))

                      ;
# 463 "/usr/include/stdio.h" 3 4








extern int vfscanf (FILE *__restrict __s, const char *__restrict __format,
      __gnuc_va_list __arg)
     __attribute__ ((__format__ (__scanf__, 2, 0))) ;





extern int vscanf (const char *__restrict __format, __gnuc_va_list __arg)
     __attribute__ ((__format__ (__scanf__, 1, 0))) ;


extern int vsscanf (const char *__restrict __s,
      const char *__restrict __format, __gnuc_va_list __arg)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__format__ (__scanf__, 2, 0)));
# 494 "/usr/include/stdio.h" 3 4
extern int vfscanf (FILE *__restrict __s, const char *__restrict __format, __gnuc_va_list __arg) __asm__ ("" "__isoc99_vfscanf")



     __attribute__ ((__format__ (__scanf__, 2, 0))) ;
extern int vscanf (const char *__restrict __format, __gnuc_va_list __arg) __asm__ ("" "__isoc99_vscanf")

     __attribute__ ((__format__ (__scanf__, 1, 0))) ;
extern int vsscanf (const char *__restrict __s, const char *__restrict __format, __gnuc_va_list __arg) __asm__ ("" "__isoc99_vsscanf") __attribute__ ((__nothrow__ , __leaf__))



     __attribute__ ((__format__ (__scanf__, 2, 0)));
# 522 "/usr/include/stdio.h" 3 4









extern int fgetc (FILE *__stream);
extern int getc (FILE *__stream);





extern int getchar (void);

# 550 "/usr/include/stdio.h" 3 4
extern int getc_unlocked (FILE *__stream);
extern int getchar_unlocked (void);
# 561 "/usr/include/stdio.h" 3 4
extern int fgetc_unlocked (FILE *__stream);











extern int fputc (int __c, FILE *__stream);
extern int putc (int __c, FILE *__stream);





extern int putchar (int __c);

# 594 "/usr/include/stdio.h" 3 4
extern int fputc_unlocked (int __c, FILE *__stream);







extern int putc_unlocked (int __c, FILE *__stream);
extern int putchar_unlocked (int __c);






extern int getw (FILE *__stream);


extern int putw (int __w, FILE *__stream);








extern char *fgets (char *__restrict __s, int __n, FILE *__restrict __stream)
     ;
# 638 "/usr/include/stdio.h" 3 4
extern char *gets (char *__s) __attribute__ ((__deprecated__));


# 665 "/usr/include/stdio.h" 3 4
extern __ssize_t __getdelim (char **__restrict __lineptr,
          size_t *__restrict __n, int __delimiter,
          FILE *__restrict __stream) ;
extern __ssize_t getdelim (char **__restrict __lineptr,
        size_t *__restrict __n, int __delimiter,
        FILE *__restrict __stream) ;







extern __ssize_t getline (char **__restrict __lineptr,
       size_t *__restrict __n,
       FILE *__restrict __stream) ;








extern int fputs (const char *__restrict __s, FILE *__restrict __stream);





extern int puts (const char *__s);






extern int ungetc (int __c, FILE *__stream);






extern size_t fread (void *__restrict __ptr, size_t __size,
       size_t __n, FILE *__restrict __stream) ;




extern size_t fwrite (const void *__restrict __ptr, size_t __size,
        size_t __n, FILE *__restrict __s);

# 737 "/usr/include/stdio.h" 3 4
extern size_t fread_unlocked (void *__restrict __ptr, size_t __size,
         size_t __n, FILE *__restrict __stream) ;
extern size_t fwrite_unlocked (const void *__restrict __ptr, size_t __size,
          size_t __n, FILE *__restrict __stream);








extern int fseek (FILE *__stream, long int __off, int __whence);




extern long int ftell (FILE *__stream) ;




extern void rewind (FILE *__stream);

# 773 "/usr/include/stdio.h" 3 4
extern int fseeko (FILE *__stream, __off_t __off, int __whence);




extern __off_t ftello (FILE *__stream) ;
# 792 "/usr/include/stdio.h" 3 4






extern int fgetpos (FILE *__restrict __stream, fpos_t *__restrict __pos);




extern int fsetpos (FILE *__stream, const fpos_t *__pos);
# 815 "/usr/include/stdio.h" 3 4

# 824 "/usr/include/stdio.h" 3 4


extern void clearerr (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__));

extern int feof (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__)) ;

extern int ferror (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__)) ;




extern void clearerr_unlocked (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__));
extern int feof_unlocked (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__)) ;
extern int ferror_unlocked (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__)) ;








extern void perror (const char *__s);






# 1 "/usr/include/x86_64-linux-gnu/bits/sys_errlist.h" 1 3 4
# 26 "/usr/include/x86_64-linux-gnu/bits/sys_errlist.h" 3 4
extern int sys_nerr;
extern const char *const sys_errlist[];
# 854 "/usr/include/stdio.h" 2 3 4




extern int fileno (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__)) ;




extern int fileno_unlocked (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__)) ;
# 873 "/usr/include/stdio.h" 3 4
extern FILE *popen (const char *__command, const char *__modes) ;





extern int pclose (FILE *__stream);





extern char *ctermid (char *__s) __attribute__ ((__nothrow__ , __leaf__));
# 913 "/usr/include/stdio.h" 3 4
extern void flockfile (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__));



extern int ftrylockfile (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__)) ;


extern void funlockfile (FILE *__stream) __attribute__ ((__nothrow__ , __leaf__));
# 943 "/usr/include/stdio.h" 3 4

# 27 "cligen_var.c" 2
# 1 "/usr/include/stdlib.h" 1 3 4
# 32 "/usr/include/stdlib.h" 3 4
# 1 "/usr/lib/gcc/x86_64-linux-gnu/4.8/include/stddef.h" 1 3 4
# 324 "/usr/lib/gcc/x86_64-linux-gnu/4.8/include/stddef.h" 3 4
typedef int wchar_t;
# 33 "/usr/include/stdlib.h" 2 3 4








# 1 "/usr/include/x86_64-linux-gnu/bits/waitflags.h" 1 3 4
# 50 "/usr/include/x86_64-linux-gnu/bits/waitflags.h" 3 4
typedef enum
{
  P_ALL,
  P_PID,
  P_PGID
} idtype_t;
# 42 "/usr/include/stdlib.h" 2 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/waitstatus.h" 1 3 4
# 64 "/usr/include/x86_64-linux-gnu/bits/waitstatus.h" 3 4
# 1 "/usr/include/endian.h" 1 3 4
# 36 "/usr/include/endian.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/endian.h" 1 3 4
# 37 "/usr/include/endian.h" 2 3 4
# 60 "/usr/include/endian.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/byteswap.h" 1 3 4
# 28 "/usr/include/x86_64-linux-gnu/bits/byteswap.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/wordsize.h" 1 3 4
# 29 "/usr/include/x86_64-linux-gnu/bits/byteswap.h" 2 3 4






# 1 "/usr/include/x86_64-linux-gnu/bits/byteswap-16.h" 1 3 4
# 36 "/usr/include/x86_64-linux-gnu/bits/byteswap.h" 2 3 4
# 44 "/usr/include/x86_64-linux-gnu/bits/byteswap.h" 3 4
static __inline unsigned int
__bswap_32 (unsigned int __bsx)
{
  return __builtin_bswap32 (__bsx);
}
# 108 "/usr/include/x86_64-linux-gnu/bits/byteswap.h" 3 4
static __inline __uint64_t
__bswap_64 (__uint64_t __bsx)
{
  return __builtin_bswap64 (__bsx);
}
# 61 "/usr/include/endian.h" 2 3 4
# 65 "/usr/include/x86_64-linux-gnu/bits/waitstatus.h" 2 3 4

union wait
  {
    int w_status;
    struct
      {

 unsigned int __w_termsig:7;
 unsigned int __w_coredump:1;
 unsigned int __w_retcode:8;
 unsigned int:16;







      } __wait_terminated;
    struct
      {

 unsigned int __w_stopval:8;
 unsigned int __w_stopsig:8;
 unsigned int:16;






      } __wait_stopped;
  };
# 43 "/usr/include/stdlib.h" 2 3 4
# 67 "/usr/include/stdlib.h" 3 4
typedef union
  {
    union wait *__uptr;
    int *__iptr;
  } __WAIT_STATUS __attribute__ ((__transparent_union__));
# 95 "/usr/include/stdlib.h" 3 4


typedef struct
  {
    int quot;
    int rem;
  } div_t;



typedef struct
  {
    long int quot;
    long int rem;
  } ldiv_t;







__extension__ typedef struct
  {
    long long int quot;
    long long int rem;
  } lldiv_t;


# 139 "/usr/include/stdlib.h" 3 4
extern size_t __ctype_get_mb_cur_max (void) __attribute__ ((__nothrow__ , __leaf__)) ;




extern double atof (const char *__nptr)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;

extern int atoi (const char *__nptr)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;

extern long int atol (const char *__nptr)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;





__extension__ extern long long int atoll (const char *__nptr)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;





extern double strtod (const char *__restrict __nptr,
        char **__restrict __endptr)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));





extern float strtof (const char *__restrict __nptr,
       char **__restrict __endptr) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));

extern long double strtold (const char *__restrict __nptr,
       char **__restrict __endptr)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));





extern long int strtol (const char *__restrict __nptr,
   char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));

extern unsigned long int strtoul (const char *__restrict __nptr,
      char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));




__extension__
extern long long int strtoq (const char *__restrict __nptr,
        char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));

__extension__
extern unsigned long long int strtouq (const char *__restrict __nptr,
           char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));





__extension__
extern long long int strtoll (const char *__restrict __nptr,
         char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));

__extension__
extern unsigned long long int strtoull (const char *__restrict __nptr,
     char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));

# 305 "/usr/include/stdlib.h" 3 4
extern char *l64a (long int __n) __attribute__ ((__nothrow__ , __leaf__)) ;


extern long int a64l (const char *__s)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;




# 1 "/usr/include/x86_64-linux-gnu/sys/types.h" 1 3 4
# 27 "/usr/include/x86_64-linux-gnu/sys/types.h" 3 4






typedef __u_char u_char;
typedef __u_short u_short;
typedef __u_int u_int;
typedef __u_long u_long;
typedef __quad_t quad_t;
typedef __u_quad_t u_quad_t;
typedef __fsid_t fsid_t;




typedef __loff_t loff_t;



typedef __ino_t ino_t;
# 60 "/usr/include/x86_64-linux-gnu/sys/types.h" 3 4
typedef __dev_t dev_t;




typedef __gid_t gid_t;




typedef __mode_t mode_t;




typedef __nlink_t nlink_t;




typedef __uid_t uid_t;
# 98 "/usr/include/x86_64-linux-gnu/sys/types.h" 3 4
typedef __pid_t pid_t;





typedef __id_t id_t;
# 115 "/usr/include/x86_64-linux-gnu/sys/types.h" 3 4
typedef __daddr_t daddr_t;
typedef __caddr_t caddr_t;





typedef __key_t key_t;
# 132 "/usr/include/x86_64-linux-gnu/sys/types.h" 3 4
# 1 "/usr/include/time.h" 1 3 4
# 57 "/usr/include/time.h" 3 4


typedef __clock_t clock_t;



# 73 "/usr/include/time.h" 3 4


typedef __time_t time_t;



# 91 "/usr/include/time.h" 3 4
typedef __clockid_t clockid_t;
# 103 "/usr/include/time.h" 3 4
typedef __timer_t timer_t;
# 133 "/usr/include/x86_64-linux-gnu/sys/types.h" 2 3 4
# 146 "/usr/include/x86_64-linux-gnu/sys/types.h" 3 4
# 1 "/usr/lib/gcc/x86_64-linux-gnu/4.8/include/stddef.h" 1 3 4
# 147 "/usr/include/x86_64-linux-gnu/sys/types.h" 2 3 4



typedef unsigned long int ulong;
typedef unsigned short int ushort;
typedef unsigned int uint;
# 194 "/usr/include/x86_64-linux-gnu/sys/types.h" 3 4
typedef int int8_t __attribute__ ((__mode__ (__QI__)));
typedef int int16_t __attribute__ ((__mode__ (__HI__)));
typedef int int32_t __attribute__ ((__mode__ (__SI__)));
typedef int int64_t __attribute__ ((__mode__ (__DI__)));


typedef unsigned int u_int8_t __attribute__ ((__mode__ (__QI__)));
typedef unsigned int u_int16_t __attribute__ ((__mode__ (__HI__)));
typedef unsigned int u_int32_t __attribute__ ((__mode__ (__SI__)));
typedef unsigned int u_int64_t __attribute__ ((__mode__ (__DI__)));

typedef int register_t __attribute__ ((__mode__ (__word__)));
# 219 "/usr/include/x86_64-linux-gnu/sys/types.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/sys/select.h" 1 3 4
# 30 "/usr/include/x86_64-linux-gnu/sys/select.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/select.h" 1 3 4
# 22 "/usr/include/x86_64-linux-gnu/bits/select.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/wordsize.h" 1 3 4
# 23 "/usr/include/x86_64-linux-gnu/bits/select.h" 2 3 4
# 31 "/usr/include/x86_64-linux-gnu/sys/select.h" 2 3 4


# 1 "/usr/include/x86_64-linux-gnu/bits/sigset.h" 1 3 4
# 22 "/usr/include/x86_64-linux-gnu/bits/sigset.h" 3 4
typedef int __sig_atomic_t;




typedef struct
  {
    unsigned long int __val[(1024 / (8 * sizeof (unsigned long int)))];
  } __sigset_t;
# 34 "/usr/include/x86_64-linux-gnu/sys/select.h" 2 3 4



typedef __sigset_t sigset_t;





# 1 "/usr/include/time.h" 1 3 4
# 120 "/usr/include/time.h" 3 4
struct timespec
  {
    __time_t tv_sec;
    __syscall_slong_t tv_nsec;
  };
# 44 "/usr/include/x86_64-linux-gnu/sys/select.h" 2 3 4

# 1 "/usr/include/x86_64-linux-gnu/bits/time.h" 1 3 4
# 30 "/usr/include/x86_64-linux-gnu/bits/time.h" 3 4
struct timeval
  {
    __time_t tv_sec;
    __suseconds_t tv_usec;
  };
# 46 "/usr/include/x86_64-linux-gnu/sys/select.h" 2 3 4


typedef __suseconds_t suseconds_t;





typedef long int __fd_mask;
# 64 "/usr/include/x86_64-linux-gnu/sys/select.h" 3 4
typedef struct
  {






    __fd_mask __fds_bits[1024 / (8 * (int) sizeof (__fd_mask))];


  } fd_set;






typedef __fd_mask fd_mask;
# 96 "/usr/include/x86_64-linux-gnu/sys/select.h" 3 4

# 106 "/usr/include/x86_64-linux-gnu/sys/select.h" 3 4
extern int select (int __nfds, fd_set *__restrict __readfds,
     fd_set *__restrict __writefds,
     fd_set *__restrict __exceptfds,
     struct timeval *__restrict __timeout);
# 118 "/usr/include/x86_64-linux-gnu/sys/select.h" 3 4
extern int pselect (int __nfds, fd_set *__restrict __readfds,
      fd_set *__restrict __writefds,
      fd_set *__restrict __exceptfds,
      const struct timespec *__restrict __timeout,
      const __sigset_t *__restrict __sigmask);
# 131 "/usr/include/x86_64-linux-gnu/sys/select.h" 3 4

# 220 "/usr/include/x86_64-linux-gnu/sys/types.h" 2 3 4


# 1 "/usr/include/x86_64-linux-gnu/sys/sysmacros.h" 1 3 4
# 24 "/usr/include/x86_64-linux-gnu/sys/sysmacros.h" 3 4


__extension__
extern unsigned int gnu_dev_major (unsigned long long int __dev)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
__extension__
extern unsigned int gnu_dev_minor (unsigned long long int __dev)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
__extension__
extern unsigned long long int gnu_dev_makedev (unsigned int __major,
            unsigned int __minor)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
# 58 "/usr/include/x86_64-linux-gnu/sys/sysmacros.h" 3 4

# 223 "/usr/include/x86_64-linux-gnu/sys/types.h" 2 3 4





typedef __blksize_t blksize_t;






typedef __blkcnt_t blkcnt_t;



typedef __fsblkcnt_t fsblkcnt_t;



typedef __fsfilcnt_t fsfilcnt_t;
# 270 "/usr/include/x86_64-linux-gnu/sys/types.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/pthreadtypes.h" 1 3 4
# 21 "/usr/include/x86_64-linux-gnu/bits/pthreadtypes.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/wordsize.h" 1 3 4
# 22 "/usr/include/x86_64-linux-gnu/bits/pthreadtypes.h" 2 3 4
# 60 "/usr/include/x86_64-linux-gnu/bits/pthreadtypes.h" 3 4
typedef unsigned long int pthread_t;


union pthread_attr_t
{
  char __size[56];
  long int __align;
};

typedef union pthread_attr_t pthread_attr_t;





typedef struct __pthread_internal_list
{
  struct __pthread_internal_list *__prev;
  struct __pthread_internal_list *__next;
} __pthread_list_t;
# 90 "/usr/include/x86_64-linux-gnu/bits/pthreadtypes.h" 3 4
typedef union
{
  struct __pthread_mutex_s
  {
    int __lock;
    unsigned int __count;
    int __owner;

    unsigned int __nusers;



    int __kind;

    short __spins;
    short __elision;
    __pthread_list_t __list;
# 124 "/usr/include/x86_64-linux-gnu/bits/pthreadtypes.h" 3 4
  } __data;
  char __size[40];
  long int __align;
} pthread_mutex_t;

typedef union
{
  char __size[4];
  int __align;
} pthread_mutexattr_t;




typedef union
{
  struct
  {
    int __lock;
    unsigned int __futex;
    __extension__ unsigned long long int __total_seq;
    __extension__ unsigned long long int __wakeup_seq;
    __extension__ unsigned long long int __woken_seq;
    void *__mutex;
    unsigned int __nwaiters;
    unsigned int __broadcast_seq;
  } __data;
  char __size[48];
  __extension__ long long int __align;
} pthread_cond_t;

typedef union
{
  char __size[4];
  int __align;
} pthread_condattr_t;



typedef unsigned int pthread_key_t;



typedef int pthread_once_t;





typedef union
{

  struct
  {
    int __lock;
    unsigned int __nr_readers;
    unsigned int __readers_wakeup;
    unsigned int __writer_wakeup;
    unsigned int __nr_readers_queued;
    unsigned int __nr_writers_queued;
    int __writer;
    int __shared;
    unsigned long int __pad1;
    unsigned long int __pad2;


    unsigned int __flags;

  } __data;
# 211 "/usr/include/x86_64-linux-gnu/bits/pthreadtypes.h" 3 4
  char __size[56];
  long int __align;
} pthread_rwlock_t;

typedef union
{
  char __size[8];
  long int __align;
} pthread_rwlockattr_t;





typedef volatile int pthread_spinlock_t;




typedef union
{
  char __size[32];
  long int __align;
} pthread_barrier_t;

typedef union
{
  char __size[4];
  int __align;
} pthread_barrierattr_t;
# 271 "/usr/include/x86_64-linux-gnu/sys/types.h" 2 3 4



# 315 "/usr/include/stdlib.h" 2 3 4






extern long int random (void) __attribute__ ((__nothrow__ , __leaf__));


extern void srandom (unsigned int __seed) __attribute__ ((__nothrow__ , __leaf__));





extern char *initstate (unsigned int __seed, char *__statebuf,
   size_t __statelen) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));



extern char *setstate (char *__statebuf) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));







struct random_data
  {
    int32_t *fptr;
    int32_t *rptr;
    int32_t *state;
    int rand_type;
    int rand_deg;
    int rand_sep;
    int32_t *end_ptr;
  };

extern int random_r (struct random_data *__restrict __buf,
       int32_t *__restrict __result) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));

extern int srandom_r (unsigned int __seed, struct random_data *__buf)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));

extern int initstate_r (unsigned int __seed, char *__restrict __statebuf,
   size_t __statelen,
   struct random_data *__restrict __buf)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2, 4)));

extern int setstate_r (char *__restrict __statebuf,
         struct random_data *__restrict __buf)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));






extern int rand (void) __attribute__ ((__nothrow__ , __leaf__));

extern void srand (unsigned int __seed) __attribute__ ((__nothrow__ , __leaf__));




extern int rand_r (unsigned int *__seed) __attribute__ ((__nothrow__ , __leaf__));







extern double drand48 (void) __attribute__ ((__nothrow__ , __leaf__));
extern double erand48 (unsigned short int __xsubi[3]) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));


extern long int lrand48 (void) __attribute__ ((__nothrow__ , __leaf__));
extern long int nrand48 (unsigned short int __xsubi[3])
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));


extern long int mrand48 (void) __attribute__ ((__nothrow__ , __leaf__));
extern long int jrand48 (unsigned short int __xsubi[3])
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));


extern void srand48 (long int __seedval) __attribute__ ((__nothrow__ , __leaf__));
extern unsigned short int *seed48 (unsigned short int __seed16v[3])
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));
extern void lcong48 (unsigned short int __param[7]) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));





struct drand48_data
  {
    unsigned short int __x[3];
    unsigned short int __old_x[3];
    unsigned short int __c;
    unsigned short int __init;
    __extension__ unsigned long long int __a;

  };


extern int drand48_r (struct drand48_data *__restrict __buffer,
        double *__restrict __result) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));
extern int erand48_r (unsigned short int __xsubi[3],
        struct drand48_data *__restrict __buffer,
        double *__restrict __result) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));


extern int lrand48_r (struct drand48_data *__restrict __buffer,
        long int *__restrict __result)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));
extern int nrand48_r (unsigned short int __xsubi[3],
        struct drand48_data *__restrict __buffer,
        long int *__restrict __result)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));


extern int mrand48_r (struct drand48_data *__restrict __buffer,
        long int *__restrict __result)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));
extern int jrand48_r (unsigned short int __xsubi[3],
        struct drand48_data *__restrict __buffer,
        long int *__restrict __result)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));


extern int srand48_r (long int __seedval, struct drand48_data *__buffer)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));

extern int seed48_r (unsigned short int __seed16v[3],
       struct drand48_data *__buffer) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));

extern int lcong48_r (unsigned short int __param[7],
        struct drand48_data *__buffer)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));









extern void *malloc (size_t __size) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__malloc__)) ;

extern void *calloc (size_t __nmemb, size_t __size)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__malloc__)) ;










extern void *realloc (void *__ptr, size_t __size)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__warn_unused_result__));

extern void free (void *__ptr) __attribute__ ((__nothrow__ , __leaf__));




extern void cfree (void *__ptr) __attribute__ ((__nothrow__ , __leaf__));



# 1 "/usr/include/alloca.h" 1 3 4
# 24 "/usr/include/alloca.h" 3 4
# 1 "/usr/lib/gcc/x86_64-linux-gnu/4.8/include/stddef.h" 1 3 4
# 25 "/usr/include/alloca.h" 2 3 4







extern void *alloca (size_t __size) __attribute__ ((__nothrow__ , __leaf__));






# 493 "/usr/include/stdlib.h" 2 3 4





extern void *valloc (size_t __size) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__malloc__)) ;




extern int posix_memalign (void **__memptr, size_t __alignment, size_t __size)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;
# 513 "/usr/include/stdlib.h" 3 4


extern void abort (void) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__noreturn__));



extern int atexit (void (*__func) (void)) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));
# 530 "/usr/include/stdlib.h" 3 4





extern int on_exit (void (*__func) (int __status, void *__arg), void *__arg)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));






extern void exit (int __status) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__noreturn__));













extern void _Exit (int __status) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__noreturn__));






extern char *getenv (const char *__name) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;

# 578 "/usr/include/stdlib.h" 3 4
extern int putenv (char *__string) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));





extern int setenv (const char *__name, const char *__value, int __replace)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));


extern int unsetenv (const char *__name) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));






extern int clearenv (void) __attribute__ ((__nothrow__ , __leaf__));
# 606 "/usr/include/stdlib.h" 3 4
extern char *mktemp (char *__template) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));
# 620 "/usr/include/stdlib.h" 3 4
extern int mkstemp (char *__template) __attribute__ ((__nonnull__ (1))) ;
# 642 "/usr/include/stdlib.h" 3 4
extern int mkstemps (char *__template, int __suffixlen) __attribute__ ((__nonnull__ (1))) ;
# 663 "/usr/include/stdlib.h" 3 4
extern char *mkdtemp (char *__template) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;
# 712 "/usr/include/stdlib.h" 3 4





extern int system (const char *__command) ;

# 734 "/usr/include/stdlib.h" 3 4
extern char *realpath (const char *__restrict __name,
         char *__restrict __resolved) __attribute__ ((__nothrow__ , __leaf__)) ;






typedef int (*__compar_fn_t) (const void *, const void *);
# 752 "/usr/include/stdlib.h" 3 4



extern void *bsearch (const void *__key, const void *__base,
        size_t __nmemb, size_t __size, __compar_fn_t __compar)
     __attribute__ ((__nonnull__ (1, 2, 5))) ;







extern void qsort (void *__base, size_t __nmemb, size_t __size,
     __compar_fn_t __compar) __attribute__ ((__nonnull__ (1, 4)));
# 775 "/usr/include/stdlib.h" 3 4
extern int abs (int __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)) ;
extern long int labs (long int __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)) ;



__extension__ extern long long int llabs (long long int __x)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)) ;







extern div_t div (int __numer, int __denom)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)) ;
extern ldiv_t ldiv (long int __numer, long int __denom)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)) ;




__extension__ extern lldiv_t lldiv (long long int __numer,
        long long int __denom)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)) ;

# 812 "/usr/include/stdlib.h" 3 4
extern char *ecvt (double __value, int __ndigit, int *__restrict __decpt,
     int *__restrict __sign) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3, 4))) ;




extern char *fcvt (double __value, int __ndigit, int *__restrict __decpt,
     int *__restrict __sign) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3, 4))) ;




extern char *gcvt (double __value, int __ndigit, char *__buf)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3))) ;




extern char *qecvt (long double __value, int __ndigit,
      int *__restrict __decpt, int *__restrict __sign)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3, 4))) ;
extern char *qfcvt (long double __value, int __ndigit,
      int *__restrict __decpt, int *__restrict __sign)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3, 4))) ;
extern char *qgcvt (long double __value, int __ndigit, char *__buf)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3))) ;




extern int ecvt_r (double __value, int __ndigit, int *__restrict __decpt,
     int *__restrict __sign, char *__restrict __buf,
     size_t __len) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3, 4, 5)));
extern int fcvt_r (double __value, int __ndigit, int *__restrict __decpt,
     int *__restrict __sign, char *__restrict __buf,
     size_t __len) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3, 4, 5)));

extern int qecvt_r (long double __value, int __ndigit,
      int *__restrict __decpt, int *__restrict __sign,
      char *__restrict __buf, size_t __len)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3, 4, 5)));
extern int qfcvt_r (long double __value, int __ndigit,
      int *__restrict __decpt, int *__restrict __sign,
      char *__restrict __buf, size_t __len)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3, 4, 5)));






extern int mblen (const char *__s, size_t __n) __attribute__ ((__nothrow__ , __leaf__));


extern int mbtowc (wchar_t *__restrict __pwc,
     const char *__restrict __s, size_t __n) __attribute__ ((__nothrow__ , __leaf__));


extern int wctomb (char *__s, wchar_t __wchar) __attribute__ ((__nothrow__ , __leaf__));



extern size_t mbstowcs (wchar_t *__restrict __pwcs,
   const char *__restrict __s, size_t __n) __attribute__ ((__nothrow__ , __leaf__));

extern size_t wcstombs (char *__restrict __s,
   const wchar_t *__restrict __pwcs, size_t __n)
     __attribute__ ((__nothrow__ , __leaf__));








extern int rpmatch (const char *__response) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;
# 899 "/usr/include/stdlib.h" 3 4
extern int getsubopt (char **__restrict __optionp,
        char *const *__restrict __tokens,
        char **__restrict __valuep)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2, 3))) ;
# 951 "/usr/include/stdlib.h" 3 4
extern int getloadavg (double __loadavg[], int __nelem)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));


# 1 "/usr/include/x86_64-linux-gnu/bits/stdlib-float.h" 1 3 4
# 956 "/usr/include/stdlib.h" 2 3 4
# 968 "/usr/include/stdlib.h" 3 4

# 28 "cligen_var.c" 2
# 1 "/usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdint.h" 1 3 4
# 9 "/usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdint.h" 3 4
# 1 "/usr/include/stdint.h" 1 3 4
# 26 "/usr/include/stdint.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/wchar.h" 1 3 4
# 27 "/usr/include/stdint.h" 2 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/wordsize.h" 1 3 4
# 28 "/usr/include/stdint.h" 2 3 4
# 48 "/usr/include/stdint.h" 3 4
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;

typedef unsigned int uint32_t;



typedef unsigned long int uint64_t;
# 65 "/usr/include/stdint.h" 3 4
typedef signed char int_least8_t;
typedef short int int_least16_t;
typedef int int_least32_t;

typedef long int int_least64_t;






typedef unsigned char uint_least8_t;
typedef unsigned short int uint_least16_t;
typedef unsigned int uint_least32_t;

typedef unsigned long int uint_least64_t;
# 90 "/usr/include/stdint.h" 3 4
typedef signed char int_fast8_t;

typedef long int int_fast16_t;
typedef long int int_fast32_t;
typedef long int int_fast64_t;
# 103 "/usr/include/stdint.h" 3 4
typedef unsigned char uint_fast8_t;

typedef unsigned long int uint_fast16_t;
typedef unsigned long int uint_fast32_t;
typedef unsigned long int uint_fast64_t;
# 119 "/usr/include/stdint.h" 3 4
typedef long int intptr_t;


typedef unsigned long int uintptr_t;
# 134 "/usr/include/stdint.h" 3 4
typedef long int intmax_t;
typedef unsigned long int uintmax_t;
# 10 "/usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdint.h" 2 3 4
# 29 "cligen_var.c" 2
# 1 "/usr/lib/gcc/x86_64-linux-gnu/4.8/include-fixed/limits.h" 1 3 4
# 34 "/usr/lib/gcc/x86_64-linux-gnu/4.8/include-fixed/limits.h" 3 4
# 1 "/usr/lib/gcc/x86_64-linux-gnu/4.8/include-fixed/syslimits.h" 1 3 4






# 1 "/usr/lib/gcc/x86_64-linux-gnu/4.8/include-fixed/limits.h" 1 3 4
# 168 "/usr/lib/gcc/x86_64-linux-gnu/4.8/include-fixed/limits.h" 3 4
# 1 "/usr/include/limits.h" 1 3 4
# 143 "/usr/include/limits.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/posix1_lim.h" 1 3 4
# 160 "/usr/include/x86_64-linux-gnu/bits/posix1_lim.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/local_lim.h" 1 3 4
# 38 "/usr/include/x86_64-linux-gnu/bits/local_lim.h" 3 4
# 1 "/usr/include/linux/limits.h" 1 3 4
# 39 "/usr/include/x86_64-linux-gnu/bits/local_lim.h" 2 3 4
# 161 "/usr/include/x86_64-linux-gnu/bits/posix1_lim.h" 2 3 4
# 144 "/usr/include/limits.h" 2 3 4



# 1 "/usr/include/x86_64-linux-gnu/bits/posix2_lim.h" 1 3 4
# 148 "/usr/include/limits.h" 2 3 4
# 169 "/usr/lib/gcc/x86_64-linux-gnu/4.8/include-fixed/limits.h" 2 3 4
# 8 "/usr/lib/gcc/x86_64-linux-gnu/4.8/include-fixed/syslimits.h" 2 3 4
# 35 "/usr/lib/gcc/x86_64-linux-gnu/4.8/include-fixed/limits.h" 2 3 4
# 30 "cligen_var.c" 2
# 1 "/usr/include/inttypes.h" 1 3 4
# 34 "/usr/include/inttypes.h" 3 4
typedef int __gwchar_t;
# 266 "/usr/include/inttypes.h" 3 4





typedef struct
  {
    long int quot;
    long int rem;
  } imaxdiv_t;
# 290 "/usr/include/inttypes.h" 3 4
extern intmax_t imaxabs (intmax_t __n) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));


extern imaxdiv_t imaxdiv (intmax_t __numer, intmax_t __denom)
      __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));


extern intmax_t strtoimax (const char *__restrict __nptr,
      char **__restrict __endptr, int __base) __attribute__ ((__nothrow__ , __leaf__));


extern uintmax_t strtoumax (const char *__restrict __nptr,
       char ** __restrict __endptr, int __base) __attribute__ ((__nothrow__ , __leaf__));


extern intmax_t wcstoimax (const __gwchar_t *__restrict __nptr,
      __gwchar_t **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__ , __leaf__));


extern uintmax_t wcstoumax (const __gwchar_t *__restrict __nptr,
       __gwchar_t ** __restrict __endptr, int __base)
     __attribute__ ((__nothrow__ , __leaf__));
# 432 "/usr/include/inttypes.h" 3 4

# 31 "cligen_var.c" 2
# 1 "/usr/include/unistd.h" 1 3 4
# 27 "/usr/include/unistd.h" 3 4

# 202 "/usr/include/unistd.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/posix_opt.h" 1 3 4
# 203 "/usr/include/unistd.h" 2 3 4



# 1 "/usr/include/x86_64-linux-gnu/bits/environments.h" 1 3 4
# 22 "/usr/include/x86_64-linux-gnu/bits/environments.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/wordsize.h" 1 3 4
# 23 "/usr/include/x86_64-linux-gnu/bits/environments.h" 2 3 4
# 207 "/usr/include/unistd.h" 2 3 4
# 226 "/usr/include/unistd.h" 3 4
# 1 "/usr/lib/gcc/x86_64-linux-gnu/4.8/include/stddef.h" 1 3 4
# 227 "/usr/include/unistd.h" 2 3 4
# 255 "/usr/include/unistd.h" 3 4
typedef __useconds_t useconds_t;
# 274 "/usr/include/unistd.h" 3 4
typedef __socklen_t socklen_t;
# 287 "/usr/include/unistd.h" 3 4
extern int access (const char *__name, int __type) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));
# 304 "/usr/include/unistd.h" 3 4
extern int faccessat (int __fd, const char *__file, int __type, int __flag)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2))) ;
# 334 "/usr/include/unistd.h" 3 4
extern __off_t lseek (int __fd, __off_t __offset, int __whence) __attribute__ ((__nothrow__ , __leaf__));
# 353 "/usr/include/unistd.h" 3 4
extern int close (int __fd);






extern ssize_t read (int __fd, void *__buf, size_t __nbytes) ;





extern ssize_t write (int __fd, const void *__buf, size_t __n) ;
# 376 "/usr/include/unistd.h" 3 4
extern ssize_t pread (int __fd, void *__buf, size_t __nbytes,
        __off_t __offset) ;






extern ssize_t pwrite (int __fd, const void *__buf, size_t __n,
         __off_t __offset) ;
# 417 "/usr/include/unistd.h" 3 4
extern int pipe (int __pipedes[2]) __attribute__ ((__nothrow__ , __leaf__)) ;
# 432 "/usr/include/unistd.h" 3 4
extern unsigned int alarm (unsigned int __seconds) __attribute__ ((__nothrow__ , __leaf__));
# 444 "/usr/include/unistd.h" 3 4
extern unsigned int sleep (unsigned int __seconds);







extern __useconds_t ualarm (__useconds_t __value, __useconds_t __interval)
     __attribute__ ((__nothrow__ , __leaf__));






extern int usleep (__useconds_t __useconds);
# 469 "/usr/include/unistd.h" 3 4
extern int pause (void);



extern int chown (const char *__file, __uid_t __owner, __gid_t __group)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;



extern int fchown (int __fd, __uid_t __owner, __gid_t __group) __attribute__ ((__nothrow__ , __leaf__)) ;




extern int lchown (const char *__file, __uid_t __owner, __gid_t __group)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;






extern int fchownat (int __fd, const char *__file, __uid_t __owner,
       __gid_t __group, int __flag)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2))) ;



extern int chdir (const char *__path) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;



extern int fchdir (int __fd) __attribute__ ((__nothrow__ , __leaf__)) ;
# 511 "/usr/include/unistd.h" 3 4
extern char *getcwd (char *__buf, size_t __size) __attribute__ ((__nothrow__ , __leaf__)) ;
# 525 "/usr/include/unistd.h" 3 4
extern char *getwd (char *__buf)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) __attribute__ ((__deprecated__)) ;




extern int dup (int __fd) __attribute__ ((__nothrow__ , __leaf__)) ;


extern int dup2 (int __fd, int __fd2) __attribute__ ((__nothrow__ , __leaf__));
# 543 "/usr/include/unistd.h" 3 4
extern char **__environ;







extern int execve (const char *__path, char *const __argv[],
     char *const __envp[]) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));




extern int fexecve (int __fd, char *const __argv[], char *const __envp[])
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));




extern int execv (const char *__path, char *const __argv[])
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));



extern int execle (const char *__path, const char *__arg, ...)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));



extern int execl (const char *__path, const char *__arg, ...)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));



extern int execvp (const char *__file, char *const __argv[])
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));




extern int execlp (const char *__file, const char *__arg, ...)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));
# 598 "/usr/include/unistd.h" 3 4
extern int nice (int __inc) __attribute__ ((__nothrow__ , __leaf__)) ;




extern void _exit (int __status) __attribute__ ((__noreturn__));





# 1 "/usr/include/x86_64-linux-gnu/bits/confname.h" 1 3 4
# 24 "/usr/include/x86_64-linux-gnu/bits/confname.h" 3 4
enum
  {
    _PC_LINK_MAX,

    _PC_MAX_CANON,

    _PC_MAX_INPUT,

    _PC_NAME_MAX,

    _PC_PATH_MAX,

    _PC_PIPE_BUF,

    _PC_CHOWN_RESTRICTED,

    _PC_NO_TRUNC,

    _PC_VDISABLE,

    _PC_SYNC_IO,

    _PC_ASYNC_IO,

    _PC_PRIO_IO,

    _PC_SOCK_MAXBUF,

    _PC_FILESIZEBITS,

    _PC_REC_INCR_XFER_SIZE,

    _PC_REC_MAX_XFER_SIZE,

    _PC_REC_MIN_XFER_SIZE,

    _PC_REC_XFER_ALIGN,

    _PC_ALLOC_SIZE_MIN,

    _PC_SYMLINK_MAX,

    _PC_2_SYMLINKS

  };


enum
  {
    _SC_ARG_MAX,

    _SC_CHILD_MAX,

    _SC_CLK_TCK,

    _SC_NGROUPS_MAX,

    _SC_OPEN_MAX,

    _SC_STREAM_MAX,

    _SC_TZNAME_MAX,

    _SC_JOB_CONTROL,

    _SC_SAVED_IDS,

    _SC_REALTIME_SIGNALS,

    _SC_PRIORITY_SCHEDULING,

    _SC_TIMERS,

    _SC_ASYNCHRONOUS_IO,

    _SC_PRIORITIZED_IO,

    _SC_SYNCHRONIZED_IO,

    _SC_FSYNC,

    _SC_MAPPED_FILES,

    _SC_MEMLOCK,

    _SC_MEMLOCK_RANGE,

    _SC_MEMORY_PROTECTION,

    _SC_MESSAGE_PASSING,

    _SC_SEMAPHORES,

    _SC_SHARED_MEMORY_OBJECTS,

    _SC_AIO_LISTIO_MAX,

    _SC_AIO_MAX,

    _SC_AIO_PRIO_DELTA_MAX,

    _SC_DELAYTIMER_MAX,

    _SC_MQ_OPEN_MAX,

    _SC_MQ_PRIO_MAX,

    _SC_VERSION,

    _SC_PAGESIZE,


    _SC_RTSIG_MAX,

    _SC_SEM_NSEMS_MAX,

    _SC_SEM_VALUE_MAX,

    _SC_SIGQUEUE_MAX,

    _SC_TIMER_MAX,




    _SC_BC_BASE_MAX,

    _SC_BC_DIM_MAX,

    _SC_BC_SCALE_MAX,

    _SC_BC_STRING_MAX,

    _SC_COLL_WEIGHTS_MAX,

    _SC_EQUIV_CLASS_MAX,

    _SC_EXPR_NEST_MAX,

    _SC_LINE_MAX,

    _SC_RE_DUP_MAX,

    _SC_CHARCLASS_NAME_MAX,


    _SC_2_VERSION,

    _SC_2_C_BIND,

    _SC_2_C_DEV,

    _SC_2_FORT_DEV,

    _SC_2_FORT_RUN,

    _SC_2_SW_DEV,

    _SC_2_LOCALEDEF,


    _SC_PII,

    _SC_PII_XTI,

    _SC_PII_SOCKET,

    _SC_PII_INTERNET,

    _SC_PII_OSI,

    _SC_POLL,

    _SC_SELECT,

    _SC_UIO_MAXIOV,

    _SC_IOV_MAX = _SC_UIO_MAXIOV,

    _SC_PII_INTERNET_STREAM,

    _SC_PII_INTERNET_DGRAM,

    _SC_PII_OSI_COTS,

    _SC_PII_OSI_CLTS,

    _SC_PII_OSI_M,

    _SC_T_IOV_MAX,



    _SC_THREADS,

    _SC_THREAD_SAFE_FUNCTIONS,

    _SC_GETGR_R_SIZE_MAX,

    _SC_GETPW_R_SIZE_MAX,

    _SC_LOGIN_NAME_MAX,

    _SC_TTY_NAME_MAX,

    _SC_THREAD_DESTRUCTOR_ITERATIONS,

    _SC_THREAD_KEYS_MAX,

    _SC_THREAD_STACK_MIN,

    _SC_THREAD_THREADS_MAX,

    _SC_THREAD_ATTR_STACKADDR,

    _SC_THREAD_ATTR_STACKSIZE,

    _SC_THREAD_PRIORITY_SCHEDULING,

    _SC_THREAD_PRIO_INHERIT,

    _SC_THREAD_PRIO_PROTECT,

    _SC_THREAD_PROCESS_SHARED,


    _SC_NPROCESSORS_CONF,

    _SC_NPROCESSORS_ONLN,

    _SC_PHYS_PAGES,

    _SC_AVPHYS_PAGES,

    _SC_ATEXIT_MAX,

    _SC_PASS_MAX,


    _SC_XOPEN_VERSION,

    _SC_XOPEN_XCU_VERSION,

    _SC_XOPEN_UNIX,

    _SC_XOPEN_CRYPT,

    _SC_XOPEN_ENH_I18N,

    _SC_XOPEN_SHM,


    _SC_2_CHAR_TERM,

    _SC_2_C_VERSION,

    _SC_2_UPE,


    _SC_XOPEN_XPG2,

    _SC_XOPEN_XPG3,

    _SC_XOPEN_XPG4,


    _SC_CHAR_BIT,

    _SC_CHAR_MAX,

    _SC_CHAR_MIN,

    _SC_INT_MAX,

    _SC_INT_MIN,

    _SC_LONG_BIT,

    _SC_WORD_BIT,

    _SC_MB_LEN_MAX,

    _SC_NZERO,

    _SC_SSIZE_MAX,

    _SC_SCHAR_MAX,

    _SC_SCHAR_MIN,

    _SC_SHRT_MAX,

    _SC_SHRT_MIN,

    _SC_UCHAR_MAX,

    _SC_UINT_MAX,

    _SC_ULONG_MAX,

    _SC_USHRT_MAX,


    _SC_NL_ARGMAX,

    _SC_NL_LANGMAX,

    _SC_NL_MSGMAX,

    _SC_NL_NMAX,

    _SC_NL_SETMAX,

    _SC_NL_TEXTMAX,


    _SC_XBS5_ILP32_OFF32,

    _SC_XBS5_ILP32_OFFBIG,

    _SC_XBS5_LP64_OFF64,

    _SC_XBS5_LPBIG_OFFBIG,


    _SC_XOPEN_LEGACY,

    _SC_XOPEN_REALTIME,

    _SC_XOPEN_REALTIME_THREADS,


    _SC_ADVISORY_INFO,

    _SC_BARRIERS,

    _SC_BASE,

    _SC_C_LANG_SUPPORT,

    _SC_C_LANG_SUPPORT_R,

    _SC_CLOCK_SELECTION,

    _SC_CPUTIME,

    _SC_THREAD_CPUTIME,

    _SC_DEVICE_IO,

    _SC_DEVICE_SPECIFIC,

    _SC_DEVICE_SPECIFIC_R,

    _SC_FD_MGMT,

    _SC_FIFO,

    _SC_PIPE,

    _SC_FILE_ATTRIBUTES,

    _SC_FILE_LOCKING,

    _SC_FILE_SYSTEM,

    _SC_MONOTONIC_CLOCK,

    _SC_MULTI_PROCESS,

    _SC_SINGLE_PROCESS,

    _SC_NETWORKING,

    _SC_READER_WRITER_LOCKS,

    _SC_SPIN_LOCKS,

    _SC_REGEXP,

    _SC_REGEX_VERSION,

    _SC_SHELL,

    _SC_SIGNALS,

    _SC_SPAWN,

    _SC_SPORADIC_SERVER,

    _SC_THREAD_SPORADIC_SERVER,

    _SC_SYSTEM_DATABASE,

    _SC_SYSTEM_DATABASE_R,

    _SC_TIMEOUTS,

    _SC_TYPED_MEMORY_OBJECTS,

    _SC_USER_GROUPS,

    _SC_USER_GROUPS_R,

    _SC_2_PBS,

    _SC_2_PBS_ACCOUNTING,

    _SC_2_PBS_LOCATE,

    _SC_2_PBS_MESSAGE,

    _SC_2_PBS_TRACK,

    _SC_SYMLOOP_MAX,

    _SC_STREAMS,

    _SC_2_PBS_CHECKPOINT,


    _SC_V6_ILP32_OFF32,

    _SC_V6_ILP32_OFFBIG,

    _SC_V6_LP64_OFF64,

    _SC_V6_LPBIG_OFFBIG,


    _SC_HOST_NAME_MAX,

    _SC_TRACE,

    _SC_TRACE_EVENT_FILTER,

    _SC_TRACE_INHERIT,

    _SC_TRACE_LOG,


    _SC_LEVEL1_ICACHE_SIZE,

    _SC_LEVEL1_ICACHE_ASSOC,

    _SC_LEVEL1_ICACHE_LINESIZE,

    _SC_LEVEL1_DCACHE_SIZE,

    _SC_LEVEL1_DCACHE_ASSOC,

    _SC_LEVEL1_DCACHE_LINESIZE,

    _SC_LEVEL2_CACHE_SIZE,

    _SC_LEVEL2_CACHE_ASSOC,

    _SC_LEVEL2_CACHE_LINESIZE,

    _SC_LEVEL3_CACHE_SIZE,

    _SC_LEVEL3_CACHE_ASSOC,

    _SC_LEVEL3_CACHE_LINESIZE,

    _SC_LEVEL4_CACHE_SIZE,

    _SC_LEVEL4_CACHE_ASSOC,

    _SC_LEVEL4_CACHE_LINESIZE,



    _SC_IPV6 = _SC_LEVEL1_ICACHE_SIZE + 50,

    _SC_RAW_SOCKETS,


    _SC_V7_ILP32_OFF32,

    _SC_V7_ILP32_OFFBIG,

    _SC_V7_LP64_OFF64,

    _SC_V7_LPBIG_OFFBIG,


    _SC_SS_REPL_MAX,


    _SC_TRACE_EVENT_NAME_MAX,

    _SC_TRACE_NAME_MAX,

    _SC_TRACE_SYS_MAX,

    _SC_TRACE_USER_EVENT_MAX,


    _SC_XOPEN_STREAMS,


    _SC_THREAD_ROBUST_PRIO_INHERIT,

    _SC_THREAD_ROBUST_PRIO_PROTECT

  };


enum
  {
    _CS_PATH,


    _CS_V6_WIDTH_RESTRICTED_ENVS,



    _CS_GNU_LIBC_VERSION,

    _CS_GNU_LIBPTHREAD_VERSION,


    _CS_V5_WIDTH_RESTRICTED_ENVS,



    _CS_V7_WIDTH_RESTRICTED_ENVS,



    _CS_LFS_CFLAGS = 1000,

    _CS_LFS_LDFLAGS,

    _CS_LFS_LIBS,

    _CS_LFS_LINTFLAGS,

    _CS_LFS64_CFLAGS,

    _CS_LFS64_LDFLAGS,

    _CS_LFS64_LIBS,

    _CS_LFS64_LINTFLAGS,


    _CS_XBS5_ILP32_OFF32_CFLAGS = 1100,

    _CS_XBS5_ILP32_OFF32_LDFLAGS,

    _CS_XBS5_ILP32_OFF32_LIBS,

    _CS_XBS5_ILP32_OFF32_LINTFLAGS,

    _CS_XBS5_ILP32_OFFBIG_CFLAGS,

    _CS_XBS5_ILP32_OFFBIG_LDFLAGS,

    _CS_XBS5_ILP32_OFFBIG_LIBS,

    _CS_XBS5_ILP32_OFFBIG_LINTFLAGS,

    _CS_XBS5_LP64_OFF64_CFLAGS,

    _CS_XBS5_LP64_OFF64_LDFLAGS,

    _CS_XBS5_LP64_OFF64_LIBS,

    _CS_XBS5_LP64_OFF64_LINTFLAGS,

    _CS_XBS5_LPBIG_OFFBIG_CFLAGS,

    _CS_XBS5_LPBIG_OFFBIG_LDFLAGS,

    _CS_XBS5_LPBIG_OFFBIG_LIBS,

    _CS_XBS5_LPBIG_OFFBIG_LINTFLAGS,


    _CS_POSIX_V6_ILP32_OFF32_CFLAGS,

    _CS_POSIX_V6_ILP32_OFF32_LDFLAGS,

    _CS_POSIX_V6_ILP32_OFF32_LIBS,

    _CS_POSIX_V6_ILP32_OFF32_LINTFLAGS,

    _CS_POSIX_V6_ILP32_OFFBIG_CFLAGS,

    _CS_POSIX_V6_ILP32_OFFBIG_LDFLAGS,

    _CS_POSIX_V6_ILP32_OFFBIG_LIBS,

    _CS_POSIX_V6_ILP32_OFFBIG_LINTFLAGS,

    _CS_POSIX_V6_LP64_OFF64_CFLAGS,

    _CS_POSIX_V6_LP64_OFF64_LDFLAGS,

    _CS_POSIX_V6_LP64_OFF64_LIBS,

    _CS_POSIX_V6_LP64_OFF64_LINTFLAGS,

    _CS_POSIX_V6_LPBIG_OFFBIG_CFLAGS,

    _CS_POSIX_V6_LPBIG_OFFBIG_LDFLAGS,

    _CS_POSIX_V6_LPBIG_OFFBIG_LIBS,

    _CS_POSIX_V6_LPBIG_OFFBIG_LINTFLAGS,


    _CS_POSIX_V7_ILP32_OFF32_CFLAGS,

    _CS_POSIX_V7_ILP32_OFF32_LDFLAGS,

    _CS_POSIX_V7_ILP32_OFF32_LIBS,

    _CS_POSIX_V7_ILP32_OFF32_LINTFLAGS,

    _CS_POSIX_V7_ILP32_OFFBIG_CFLAGS,

    _CS_POSIX_V7_ILP32_OFFBIG_LDFLAGS,

    _CS_POSIX_V7_ILP32_OFFBIG_LIBS,

    _CS_POSIX_V7_ILP32_OFFBIG_LINTFLAGS,

    _CS_POSIX_V7_LP64_OFF64_CFLAGS,

    _CS_POSIX_V7_LP64_OFF64_LDFLAGS,

    _CS_POSIX_V7_LP64_OFF64_LIBS,

    _CS_POSIX_V7_LP64_OFF64_LINTFLAGS,

    _CS_POSIX_V7_LPBIG_OFFBIG_CFLAGS,

    _CS_POSIX_V7_LPBIG_OFFBIG_LDFLAGS,

    _CS_POSIX_V7_LPBIG_OFFBIG_LIBS,

    _CS_POSIX_V7_LPBIG_OFFBIG_LINTFLAGS,


    _CS_V6_ENV,

    _CS_V7_ENV

  };
# 610 "/usr/include/unistd.h" 2 3 4


extern long int pathconf (const char *__path, int __name)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));


extern long int fpathconf (int __fd, int __name) __attribute__ ((__nothrow__ , __leaf__));


extern long int sysconf (int __name) __attribute__ ((__nothrow__ , __leaf__));



extern size_t confstr (int __name, char *__buf, size_t __len) __attribute__ ((__nothrow__ , __leaf__));




extern __pid_t getpid (void) __attribute__ ((__nothrow__ , __leaf__));


extern __pid_t getppid (void) __attribute__ ((__nothrow__ , __leaf__));


extern __pid_t getpgrp (void) __attribute__ ((__nothrow__ , __leaf__));


extern __pid_t __getpgid (__pid_t __pid) __attribute__ ((__nothrow__ , __leaf__));

extern __pid_t getpgid (__pid_t __pid) __attribute__ ((__nothrow__ , __leaf__));






extern int setpgid (__pid_t __pid, __pid_t __pgid) __attribute__ ((__nothrow__ , __leaf__));
# 660 "/usr/include/unistd.h" 3 4
extern int setpgrp (void) __attribute__ ((__nothrow__ , __leaf__));






extern __pid_t setsid (void) __attribute__ ((__nothrow__ , __leaf__));



extern __pid_t getsid (__pid_t __pid) __attribute__ ((__nothrow__ , __leaf__));



extern __uid_t getuid (void) __attribute__ ((__nothrow__ , __leaf__));


extern __uid_t geteuid (void) __attribute__ ((__nothrow__ , __leaf__));


extern __gid_t getgid (void) __attribute__ ((__nothrow__ , __leaf__));


extern __gid_t getegid (void) __attribute__ ((__nothrow__ , __leaf__));




extern int getgroups (int __size, __gid_t __list[]) __attribute__ ((__nothrow__ , __leaf__)) ;
# 700 "/usr/include/unistd.h" 3 4
extern int setuid (__uid_t __uid) __attribute__ ((__nothrow__ , __leaf__)) ;




extern int setreuid (__uid_t __ruid, __uid_t __euid) __attribute__ ((__nothrow__ , __leaf__)) ;




extern int seteuid (__uid_t __uid) __attribute__ ((__nothrow__ , __leaf__)) ;






extern int setgid (__gid_t __gid) __attribute__ ((__nothrow__ , __leaf__)) ;




extern int setregid (__gid_t __rgid, __gid_t __egid) __attribute__ ((__nothrow__ , __leaf__)) ;




extern int setegid (__gid_t __gid) __attribute__ ((__nothrow__ , __leaf__)) ;
# 756 "/usr/include/unistd.h" 3 4
extern __pid_t fork (void) __attribute__ ((__nothrow__));







extern __pid_t vfork (void) __attribute__ ((__nothrow__ , __leaf__));





extern char *ttyname (int __fd) __attribute__ ((__nothrow__ , __leaf__));



extern int ttyname_r (int __fd, char *__buf, size_t __buflen)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2))) ;



extern int isatty (int __fd) __attribute__ ((__nothrow__ , __leaf__));





extern int ttyslot (void) __attribute__ ((__nothrow__ , __leaf__));




extern int link (const char *__from, const char *__to)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2))) ;




extern int linkat (int __fromfd, const char *__from, int __tofd,
     const char *__to, int __flags)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2, 4))) ;




extern int symlink (const char *__from, const char *__to)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2))) ;




extern ssize_t readlink (const char *__restrict __path,
    char *__restrict __buf, size_t __len)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2))) ;




extern int symlinkat (const char *__from, int __tofd,
        const char *__to) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 3))) ;


extern ssize_t readlinkat (int __fd, const char *__restrict __path,
      char *__restrict __buf, size_t __len)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2, 3))) ;



extern int unlink (const char *__name) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));



extern int unlinkat (int __fd, const char *__name, int __flag)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));



extern int rmdir (const char *__path) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));



extern __pid_t tcgetpgrp (int __fd) __attribute__ ((__nothrow__ , __leaf__));


extern int tcsetpgrp (int __fd, __pid_t __pgrp_id) __attribute__ ((__nothrow__ , __leaf__));






extern char *getlogin (void);







extern int getlogin_r (char *__name, size_t __name_len) __attribute__ ((__nonnull__ (1)));




extern int setlogin (const char *__name) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));
# 871 "/usr/include/unistd.h" 3 4
# 1 "/usr/include/getopt.h" 1 3 4
# 57 "/usr/include/getopt.h" 3 4
extern char *optarg;
# 71 "/usr/include/getopt.h" 3 4
extern int optind;




extern int opterr;



extern int optopt;
# 150 "/usr/include/getopt.h" 3 4
extern int getopt (int ___argc, char *const *___argv, const char *__shortopts)
       __attribute__ ((__nothrow__ , __leaf__));
# 872 "/usr/include/unistd.h" 2 3 4







extern int gethostname (char *__name, size_t __len) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));






extern int sethostname (const char *__name, size_t __len)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;



extern int sethostid (long int __id) __attribute__ ((__nothrow__ , __leaf__)) ;





extern int getdomainname (char *__name, size_t __len)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;
extern int setdomainname (const char *__name, size_t __len)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;





extern int vhangup (void) __attribute__ ((__nothrow__ , __leaf__));


extern int revoke (const char *__file) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;







extern int profil (unsigned short int *__sample_buffer, size_t __size,
     size_t __offset, unsigned int __scale)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));





extern int acct (const char *__name) __attribute__ ((__nothrow__ , __leaf__));



extern char *getusershell (void) __attribute__ ((__nothrow__ , __leaf__));
extern void endusershell (void) __attribute__ ((__nothrow__ , __leaf__));
extern void setusershell (void) __attribute__ ((__nothrow__ , __leaf__));





extern int daemon (int __nochdir, int __noclose) __attribute__ ((__nothrow__ , __leaf__)) ;






extern int chroot (const char *__path) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;



extern char *getpass (const char *__prompt) __attribute__ ((__nonnull__ (1)));







extern int fsync (int __fd);
# 969 "/usr/include/unistd.h" 3 4
extern long int gethostid (void);


extern void sync (void) __attribute__ ((__nothrow__ , __leaf__));





extern int getpagesize (void) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));




extern int getdtablesize (void) __attribute__ ((__nothrow__ , __leaf__));
# 993 "/usr/include/unistd.h" 3 4
extern int truncate (const char *__file, __off_t __length)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) ;
# 1016 "/usr/include/unistd.h" 3 4
extern int ftruncate (int __fd, __off_t __length) __attribute__ ((__nothrow__ , __leaf__)) ;
# 1037 "/usr/include/unistd.h" 3 4
extern int brk (void *__addr) __attribute__ ((__nothrow__ , __leaf__)) ;





extern void *sbrk (intptr_t __delta) __attribute__ ((__nothrow__ , __leaf__));
# 1058 "/usr/include/unistd.h" 3 4
extern long int syscall (long int __sysno, ...) __attribute__ ((__nothrow__ , __leaf__));
# 1081 "/usr/include/unistd.h" 3 4
extern int lockf (int __fd, int __cmd, __off_t __len) ;
# 1112 "/usr/include/unistd.h" 3 4
extern int fdatasync (int __fildes);
# 1151 "/usr/include/unistd.h" 3 4

# 32 "cligen_var.c" 2

# 1 "/usr/include/time.h" 1 3 4
# 29 "/usr/include/time.h" 3 4








# 1 "/usr/lib/gcc/x86_64-linux-gnu/4.8/include/stddef.h" 1 3 4
# 38 "/usr/include/time.h" 2 3 4



# 1 "/usr/include/x86_64-linux-gnu/bits/time.h" 1 3 4
# 42 "/usr/include/time.h" 2 3 4
# 131 "/usr/include/time.h" 3 4


struct tm
{
  int tm_sec;
  int tm_min;
  int tm_hour;
  int tm_mday;
  int tm_mon;
  int tm_year;
  int tm_wday;
  int tm_yday;
  int tm_isdst;


  long int tm_gmtoff;
  const char *tm_zone;




};








struct itimerspec
  {
    struct timespec it_interval;
    struct timespec it_value;
  };


struct sigevent;
# 186 "/usr/include/time.h" 3 4



extern clock_t clock (void) __attribute__ ((__nothrow__ , __leaf__));


extern time_t time (time_t *__timer) __attribute__ ((__nothrow__ , __leaf__));


extern double difftime (time_t __time1, time_t __time0)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));


extern time_t mktime (struct tm *__tp) __attribute__ ((__nothrow__ , __leaf__));





extern size_t strftime (char *__restrict __s, size_t __maxsize,
   const char *__restrict __format,
   const struct tm *__restrict __tp) __attribute__ ((__nothrow__ , __leaf__));

# 221 "/usr/include/time.h" 3 4
# 1 "/usr/include/xlocale.h" 1 3 4
# 27 "/usr/include/xlocale.h" 3 4
typedef struct __locale_struct
{

  struct __locale_data *__locales[13];


  const unsigned short int *__ctype_b;
  const int *__ctype_tolower;
  const int *__ctype_toupper;


  const char *__names[13];
} *__locale_t;


typedef __locale_t locale_t;
# 222 "/usr/include/time.h" 2 3 4

extern size_t strftime_l (char *__restrict __s, size_t __maxsize,
     const char *__restrict __format,
     const struct tm *__restrict __tp,
     __locale_t __loc) __attribute__ ((__nothrow__ , __leaf__));
# 236 "/usr/include/time.h" 3 4



extern struct tm *gmtime (const time_t *__timer) __attribute__ ((__nothrow__ , __leaf__));



extern struct tm *localtime (const time_t *__timer) __attribute__ ((__nothrow__ , __leaf__));





extern struct tm *gmtime_r (const time_t *__restrict __timer,
       struct tm *__restrict __tp) __attribute__ ((__nothrow__ , __leaf__));



extern struct tm *localtime_r (const time_t *__restrict __timer,
          struct tm *__restrict __tp) __attribute__ ((__nothrow__ , __leaf__));





extern char *asctime (const struct tm *__tp) __attribute__ ((__nothrow__ , __leaf__));


extern char *ctime (const time_t *__timer) __attribute__ ((__nothrow__ , __leaf__));







extern char *asctime_r (const struct tm *__restrict __tp,
   char *__restrict __buf) __attribute__ ((__nothrow__ , __leaf__));


extern char *ctime_r (const time_t *__restrict __timer,
        char *__restrict __buf) __attribute__ ((__nothrow__ , __leaf__));




extern char *__tzname[2];
extern int __daylight;
extern long int __timezone;




extern char *tzname[2];



extern void tzset (void) __attribute__ ((__nothrow__ , __leaf__));



extern int daylight;
extern long int timezone;





extern int stime (const time_t *__when) __attribute__ ((__nothrow__ , __leaf__));
# 319 "/usr/include/time.h" 3 4
extern time_t timegm (struct tm *__tp) __attribute__ ((__nothrow__ , __leaf__));


extern time_t timelocal (struct tm *__tp) __attribute__ ((__nothrow__ , __leaf__));


extern int dysize (int __year) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
# 334 "/usr/include/time.h" 3 4
extern int nanosleep (const struct timespec *__requested_time,
        struct timespec *__remaining);



extern int clock_getres (clockid_t __clock_id, struct timespec *__res) __attribute__ ((__nothrow__ , __leaf__));


extern int clock_gettime (clockid_t __clock_id, struct timespec *__tp) __attribute__ ((__nothrow__ , __leaf__));


extern int clock_settime (clockid_t __clock_id, const struct timespec *__tp)
     __attribute__ ((__nothrow__ , __leaf__));






extern int clock_nanosleep (clockid_t __clock_id, int __flags,
       const struct timespec *__req,
       struct timespec *__rem);


extern int clock_getcpuclockid (pid_t __pid, clockid_t *__clock_id) __attribute__ ((__nothrow__ , __leaf__));




extern int timer_create (clockid_t __clock_id,
    struct sigevent *__restrict __evp,
    timer_t *__restrict __timerid) __attribute__ ((__nothrow__ , __leaf__));


extern int timer_delete (timer_t __timerid) __attribute__ ((__nothrow__ , __leaf__));


extern int timer_settime (timer_t __timerid, int __flags,
     const struct itimerspec *__restrict __value,
     struct itimerspec *__restrict __ovalue) __attribute__ ((__nothrow__ , __leaf__));


extern int timer_gettime (timer_t __timerid, struct itimerspec *__value)
     __attribute__ ((__nothrow__ , __leaf__));


extern int timer_getoverrun (timer_t __timerid) __attribute__ ((__nothrow__ , __leaf__));
# 430 "/usr/include/time.h" 3 4

# 34 "cligen_var.c" 2
# 1 "/usr/include/x86_64-linux-gnu/sys/time.h" 1 3 4
# 27 "/usr/include/x86_64-linux-gnu/sys/time.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/time.h" 1 3 4
# 28 "/usr/include/x86_64-linux-gnu/sys/time.h" 2 3 4
# 37 "/usr/include/x86_64-linux-gnu/sys/time.h" 3 4

# 55 "/usr/include/x86_64-linux-gnu/sys/time.h" 3 4
struct timezone
  {
    int tz_minuteswest;
    int tz_dsttime;
  };

typedef struct timezone *__restrict __timezone_ptr_t;
# 71 "/usr/include/x86_64-linux-gnu/sys/time.h" 3 4
extern int gettimeofday (struct timeval *__restrict __tv,
    __timezone_ptr_t __tz) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));




extern int settimeofday (const struct timeval *__tv,
    const struct timezone *__tz)
     __attribute__ ((__nothrow__ , __leaf__));





extern int adjtime (const struct timeval *__delta,
      struct timeval *__olddelta) __attribute__ ((__nothrow__ , __leaf__));




enum __itimer_which
  {

    ITIMER_REAL = 0,


    ITIMER_VIRTUAL = 1,



    ITIMER_PROF = 2

  };



struct itimerval
  {

    struct timeval it_interval;

    struct timeval it_value;
  };






typedef int __itimer_which_t;




extern int getitimer (__itimer_which_t __which,
        struct itimerval *__value) __attribute__ ((__nothrow__ , __leaf__));




extern int setitimer (__itimer_which_t __which,
        const struct itimerval *__restrict __new,
        struct itimerval *__restrict __old) __attribute__ ((__nothrow__ , __leaf__));




extern int utimes (const char *__file, const struct timeval __tvp[2])
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));



extern int lutimes (const char *__file, const struct timeval __tvp[2])
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));


extern int futimes (int __fd, const struct timeval __tvp[2]) __attribute__ ((__nothrow__ , __leaf__));
# 189 "/usr/include/x86_64-linux-gnu/sys/time.h" 3 4

# 35 "cligen_var.c" 2
# 1 "/usr/include/x86_64-linux-gnu/sys/socket.h" 1 3 4
# 24 "/usr/include/x86_64-linux-gnu/sys/socket.h" 3 4


# 1 "/usr/include/x86_64-linux-gnu/sys/uio.h" 1 3 4
# 25 "/usr/include/x86_64-linux-gnu/sys/uio.h" 3 4



# 1 "/usr/include/x86_64-linux-gnu/bits/uio.h" 1 3 4
# 43 "/usr/include/x86_64-linux-gnu/bits/uio.h" 3 4
struct iovec
  {
    void *iov_base;
    size_t iov_len;
  };
# 29 "/usr/include/x86_64-linux-gnu/sys/uio.h" 2 3 4
# 39 "/usr/include/x86_64-linux-gnu/sys/uio.h" 3 4
extern ssize_t readv (int __fd, const struct iovec *__iovec, int __count)
  ;
# 50 "/usr/include/x86_64-linux-gnu/sys/uio.h" 3 4
extern ssize_t writev (int __fd, const struct iovec *__iovec, int __count)
  ;
# 65 "/usr/include/x86_64-linux-gnu/sys/uio.h" 3 4
extern ssize_t preadv (int __fd, const struct iovec *__iovec, int __count,
         __off_t __offset) ;
# 77 "/usr/include/x86_64-linux-gnu/sys/uio.h" 3 4
extern ssize_t pwritev (int __fd, const struct iovec *__iovec, int __count,
   __off_t __offset) ;
# 120 "/usr/include/x86_64-linux-gnu/sys/uio.h" 3 4

# 27 "/usr/include/x86_64-linux-gnu/sys/socket.h" 2 3 4

# 1 "/usr/lib/gcc/x86_64-linux-gnu/4.8/include/stddef.h" 1 3 4
# 29 "/usr/include/x86_64-linux-gnu/sys/socket.h" 2 3 4
# 38 "/usr/include/x86_64-linux-gnu/sys/socket.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/socket.h" 1 3 4
# 27 "/usr/include/x86_64-linux-gnu/bits/socket.h" 3 4
# 1 "/usr/lib/gcc/x86_64-linux-gnu/4.8/include/stddef.h" 1 3 4
# 28 "/usr/include/x86_64-linux-gnu/bits/socket.h" 2 3 4
# 38 "/usr/include/x86_64-linux-gnu/bits/socket.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/socket_type.h" 1 3 4
# 24 "/usr/include/x86_64-linux-gnu/bits/socket_type.h" 3 4
enum __socket_type
{
  SOCK_STREAM = 1,


  SOCK_DGRAM = 2,


  SOCK_RAW = 3,

  SOCK_RDM = 4,

  SOCK_SEQPACKET = 5,


  SOCK_DCCP = 6,

  SOCK_PACKET = 10,







  SOCK_CLOEXEC = 02000000,


  SOCK_NONBLOCK = 00004000


};
# 39 "/usr/include/x86_64-linux-gnu/bits/socket.h" 2 3 4
# 146 "/usr/include/x86_64-linux-gnu/bits/socket.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/sockaddr.h" 1 3 4
# 28 "/usr/include/x86_64-linux-gnu/bits/sockaddr.h" 3 4
typedef unsigned short int sa_family_t;
# 147 "/usr/include/x86_64-linux-gnu/bits/socket.h" 2 3 4


struct sockaddr
  {
    sa_family_t sa_family;
    char sa_data[14];
  };
# 162 "/usr/include/x86_64-linux-gnu/bits/socket.h" 3 4
struct sockaddr_storage
  {
    sa_family_t ss_family;
    unsigned long int __ss_align;
    char __ss_padding[(128 - (2 * sizeof (unsigned long int)))];
  };



enum
  {
    MSG_OOB = 0x01,

    MSG_PEEK = 0x02,

    MSG_DONTROUTE = 0x04,






    MSG_CTRUNC = 0x08,

    MSG_PROXY = 0x10,

    MSG_TRUNC = 0x20,

    MSG_DONTWAIT = 0x40,

    MSG_EOR = 0x80,

    MSG_WAITALL = 0x100,

    MSG_FIN = 0x200,

    MSG_SYN = 0x400,

    MSG_CONFIRM = 0x800,

    MSG_RST = 0x1000,

    MSG_ERRQUEUE = 0x2000,

    MSG_NOSIGNAL = 0x4000,

    MSG_MORE = 0x8000,

    MSG_WAITFORONE = 0x10000,

    MSG_FASTOPEN = 0x20000000,


    MSG_CMSG_CLOEXEC = 0x40000000



  };




struct msghdr
  {
    void *msg_name;
    socklen_t msg_namelen;

    struct iovec *msg_iov;
    size_t msg_iovlen;

    void *msg_control;
    size_t msg_controllen;




    int msg_flags;
  };


struct cmsghdr
  {
    size_t cmsg_len;




    int cmsg_level;
    int cmsg_type;

    __extension__ unsigned char __cmsg_data [];

  };
# 272 "/usr/include/x86_64-linux-gnu/bits/socket.h" 3 4
extern struct cmsghdr *__cmsg_nxthdr (struct msghdr *__mhdr,
          struct cmsghdr *__cmsg) __attribute__ ((__nothrow__ , __leaf__));
# 299 "/usr/include/x86_64-linux-gnu/bits/socket.h" 3 4
enum
  {
    SCM_RIGHTS = 0x01





  };
# 345 "/usr/include/x86_64-linux-gnu/bits/socket.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/asm/socket.h" 1 3 4
# 1 "/usr/include/asm-generic/socket.h" 1 3 4



# 1 "/usr/include/x86_64-linux-gnu/asm/sockios.h" 1 3 4
# 1 "/usr/include/asm-generic/sockios.h" 1 3 4
# 1 "/usr/include/x86_64-linux-gnu/asm/sockios.h" 2 3 4
# 5 "/usr/include/asm-generic/socket.h" 2 3 4
# 1 "/usr/include/x86_64-linux-gnu/asm/socket.h" 2 3 4
# 346 "/usr/include/x86_64-linux-gnu/bits/socket.h" 2 3 4
# 379 "/usr/include/x86_64-linux-gnu/bits/socket.h" 3 4
struct linger
  {
    int l_onoff;
    int l_linger;
  };
# 39 "/usr/include/x86_64-linux-gnu/sys/socket.h" 2 3 4




struct osockaddr
  {
    unsigned short int sa_family;
    unsigned char sa_data[14];
  };




enum
{
  SHUT_RD = 0,

  SHUT_WR,

  SHUT_RDWR

};
# 113 "/usr/include/x86_64-linux-gnu/sys/socket.h" 3 4
extern int socket (int __domain, int __type, int __protocol) __attribute__ ((__nothrow__ , __leaf__));





extern int socketpair (int __domain, int __type, int __protocol,
         int __fds[2]) __attribute__ ((__nothrow__ , __leaf__));


extern int bind (int __fd, const struct sockaddr * __addr, socklen_t __len)
     __attribute__ ((__nothrow__ , __leaf__));


extern int getsockname (int __fd, struct sockaddr *__restrict __addr,
   socklen_t *__restrict __len) __attribute__ ((__nothrow__ , __leaf__));
# 137 "/usr/include/x86_64-linux-gnu/sys/socket.h" 3 4
extern int connect (int __fd, const struct sockaddr * __addr, socklen_t __len);



extern int getpeername (int __fd, struct sockaddr *__restrict __addr,
   socklen_t *__restrict __len) __attribute__ ((__nothrow__ , __leaf__));






extern ssize_t send (int __fd, const void *__buf, size_t __n, int __flags);






extern ssize_t recv (int __fd, void *__buf, size_t __n, int __flags);






extern ssize_t sendto (int __fd, const void *__buf, size_t __n,
         int __flags, const struct sockaddr * __addr,
         socklen_t __addr_len);
# 174 "/usr/include/x86_64-linux-gnu/sys/socket.h" 3 4
extern ssize_t recvfrom (int __fd, void *__restrict __buf, size_t __n,
    int __flags, struct sockaddr *__restrict __addr,
    socklen_t *__restrict __addr_len);







extern ssize_t sendmsg (int __fd, const struct msghdr *__message,
   int __flags);
# 202 "/usr/include/x86_64-linux-gnu/sys/socket.h" 3 4
extern ssize_t recvmsg (int __fd, struct msghdr *__message, int __flags);
# 219 "/usr/include/x86_64-linux-gnu/sys/socket.h" 3 4
extern int getsockopt (int __fd, int __level, int __optname,
         void *__restrict __optval,
         socklen_t *__restrict __optlen) __attribute__ ((__nothrow__ , __leaf__));




extern int setsockopt (int __fd, int __level, int __optname,
         const void *__optval, socklen_t __optlen) __attribute__ ((__nothrow__ , __leaf__));





extern int listen (int __fd, int __n) __attribute__ ((__nothrow__ , __leaf__));
# 243 "/usr/include/x86_64-linux-gnu/sys/socket.h" 3 4
extern int accept (int __fd, struct sockaddr *__restrict __addr,
     socklen_t *__restrict __addr_len);
# 261 "/usr/include/x86_64-linux-gnu/sys/socket.h" 3 4
extern int shutdown (int __fd, int __how) __attribute__ ((__nothrow__ , __leaf__));




extern int sockatmark (int __fd) __attribute__ ((__nothrow__ , __leaf__));







extern int isfdtype (int __fd, int __fdtype) __attribute__ ((__nothrow__ , __leaf__));
# 283 "/usr/include/x86_64-linux-gnu/sys/socket.h" 3 4

# 36 "cligen_var.c" 2
# 1 "/usr/include/netinet/in.h" 1 3 4
# 27 "/usr/include/netinet/in.h" 3 4



typedef uint32_t in_addr_t;
struct in_addr
  {
    in_addr_t s_addr;
  };


# 1 "/usr/include/x86_64-linux-gnu/bits/in.h" 1 3 4
# 112 "/usr/include/x86_64-linux-gnu/bits/in.h" 3 4
struct ip_opts
  {
    struct in_addr ip_dst;
    char ip_opts[40];
  };


struct ip_mreqn
  {
    struct in_addr imr_multiaddr;
    struct in_addr imr_address;
    int imr_ifindex;
  };


struct in_pktinfo
  {
    int ipi_ifindex;
    struct in_addr ipi_spec_dst;
    struct in_addr ipi_addr;
  };
# 38 "/usr/include/netinet/in.h" 2 3 4


enum
  {
    IPPROTO_IP = 0,

    IPPROTO_ICMP = 1,

    IPPROTO_IGMP = 2,

    IPPROTO_IPIP = 4,

    IPPROTO_TCP = 6,

    IPPROTO_EGP = 8,

    IPPROTO_PUP = 12,

    IPPROTO_UDP = 17,

    IPPROTO_IDP = 22,

    IPPROTO_TP = 29,

    IPPROTO_DCCP = 33,

    IPPROTO_IPV6 = 41,

    IPPROTO_RSVP = 46,

    IPPROTO_GRE = 47,

    IPPROTO_ESP = 50,

    IPPROTO_AH = 51,

    IPPROTO_MTP = 92,

    IPPROTO_BEETPH = 94,

    IPPROTO_ENCAP = 98,

    IPPROTO_PIM = 103,

    IPPROTO_COMP = 108,

    IPPROTO_SCTP = 132,

    IPPROTO_UDPLITE = 136,

    IPPROTO_RAW = 255,

    IPPROTO_MAX
  };





enum
  {
    IPPROTO_HOPOPTS = 0,

    IPPROTO_ROUTING = 43,

    IPPROTO_FRAGMENT = 44,

    IPPROTO_ICMPV6 = 58,

    IPPROTO_NONE = 59,

    IPPROTO_DSTOPTS = 60,

    IPPROTO_MH = 135

  };



typedef uint16_t in_port_t;


enum
  {
    IPPORT_ECHO = 7,
    IPPORT_DISCARD = 9,
    IPPORT_SYSTAT = 11,
    IPPORT_DAYTIME = 13,
    IPPORT_NETSTAT = 15,
    IPPORT_FTP = 21,
    IPPORT_TELNET = 23,
    IPPORT_SMTP = 25,
    IPPORT_TIMESERVER = 37,
    IPPORT_NAMESERVER = 42,
    IPPORT_WHOIS = 43,
    IPPORT_MTP = 57,

    IPPORT_TFTP = 69,
    IPPORT_RJE = 77,
    IPPORT_FINGER = 79,
    IPPORT_TTYLINK = 87,
    IPPORT_SUPDUP = 95,


    IPPORT_EXECSERVER = 512,
    IPPORT_LOGINSERVER = 513,
    IPPORT_CMDSERVER = 514,
    IPPORT_EFSSERVER = 520,


    IPPORT_BIFFUDP = 512,
    IPPORT_WHOSERVER = 513,
    IPPORT_ROUTESERVER = 520,


    IPPORT_RESERVED = 1024,


    IPPORT_USERRESERVED = 5000
  };
# 209 "/usr/include/netinet/in.h" 3 4
struct in6_addr
  {
    union
      {
 uint8_t __u6_addr8[16];

 uint16_t __u6_addr16[8];
 uint32_t __u6_addr32[4];

      } __in6_u;





  };


extern const struct in6_addr in6addr_any;
extern const struct in6_addr in6addr_loopback;
# 237 "/usr/include/netinet/in.h" 3 4
struct sockaddr_in
  {
    sa_family_t sin_family;
    in_port_t sin_port;
    struct in_addr sin_addr;


    unsigned char sin_zero[sizeof (struct sockaddr) -
      (sizeof (unsigned short int)) -
      sizeof (in_port_t) -
      sizeof (struct in_addr)];
  };



struct sockaddr_in6
  {
    sa_family_t sin6_family;
    in_port_t sin6_port;
    uint32_t sin6_flowinfo;
    struct in6_addr sin6_addr;
    uint32_t sin6_scope_id;
  };




struct ip_mreq
  {

    struct in_addr imr_multiaddr;


    struct in_addr imr_interface;
  };

struct ip_mreq_source
  {

    struct in_addr imr_multiaddr;


    struct in_addr imr_interface;


    struct in_addr imr_sourceaddr;
  };




struct ipv6_mreq
  {

    struct in6_addr ipv6mr_multiaddr;


    unsigned int ipv6mr_interface;
  };




struct group_req
  {

    uint32_t gr_interface;


    struct sockaddr_storage gr_group;
  };

struct group_source_req
  {

    uint32_t gsr_interface;


    struct sockaddr_storage gsr_group;


    struct sockaddr_storage gsr_source;
  };



struct ip_msfilter
  {

    struct in_addr imsf_multiaddr;


    struct in_addr imsf_interface;


    uint32_t imsf_fmode;


    uint32_t imsf_numsrc;

    struct in_addr imsf_slist[1];
  };





struct group_filter
  {

    uint32_t gf_interface;


    struct sockaddr_storage gf_group;


    uint32_t gf_fmode;


    uint32_t gf_numsrc;

    struct sockaddr_storage gf_slist[1];
};
# 374 "/usr/include/netinet/in.h" 3 4
extern uint32_t ntohl (uint32_t __netlong) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern uint16_t ntohs (uint16_t __netshort)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern uint32_t htonl (uint32_t __hostlong)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
extern uint16_t htons (uint16_t __hostshort)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));




# 1 "/usr/include/x86_64-linux-gnu/bits/byteswap.h" 1 3 4
# 386 "/usr/include/netinet/in.h" 2 3 4
# 501 "/usr/include/netinet/in.h" 3 4
extern int bindresvport (int __sockfd, struct sockaddr_in *__sock_in) __attribute__ ((__nothrow__ , __leaf__));


extern int bindresvport6 (int __sockfd, struct sockaddr_in6 *__sock_in)
     __attribute__ ((__nothrow__ , __leaf__));
# 628 "/usr/include/netinet/in.h" 3 4

# 37 "cligen_var.c" 2
# 1 "/usr/include/arpa/inet.h" 1 3 4
# 30 "/usr/include/arpa/inet.h" 3 4




extern in_addr_t inet_addr (const char *__cp) __attribute__ ((__nothrow__ , __leaf__));


extern in_addr_t inet_lnaof (struct in_addr __in) __attribute__ ((__nothrow__ , __leaf__));



extern struct in_addr inet_makeaddr (in_addr_t __net, in_addr_t __host)
     __attribute__ ((__nothrow__ , __leaf__));


extern in_addr_t inet_netof (struct in_addr __in) __attribute__ ((__nothrow__ , __leaf__));



extern in_addr_t inet_network (const char *__cp) __attribute__ ((__nothrow__ , __leaf__));



extern char *inet_ntoa (struct in_addr __in) __attribute__ ((__nothrow__ , __leaf__));




extern int inet_pton (int __af, const char *__restrict __cp,
        void *__restrict __buf) __attribute__ ((__nothrow__ , __leaf__));




extern const char *inet_ntop (int __af, const void *__restrict __cp,
         char *__restrict __buf, socklen_t __len)
     __attribute__ ((__nothrow__ , __leaf__));






extern int inet_aton (const char *__cp, struct in_addr *__inp) __attribute__ ((__nothrow__ , __leaf__));



extern char *inet_neta (in_addr_t __net, char *__buf, size_t __len) __attribute__ ((__nothrow__ , __leaf__));




extern char *inet_net_ntop (int __af, const void *__cp, int __bits,
       char *__buf, size_t __len) __attribute__ ((__nothrow__ , __leaf__));




extern int inet_net_pton (int __af, const char *__cp,
     void *__buf, size_t __len) __attribute__ ((__nothrow__ , __leaf__));




extern unsigned int inet_nsap_addr (const char *__cp,
        unsigned char *__buf, int __len) __attribute__ ((__nothrow__ , __leaf__));



extern char *inet_nsap_ntoa (int __len, const unsigned char *__cp,
        char *__buf) __attribute__ ((__nothrow__ , __leaf__));



# 38 "cligen_var.c" 2
# 1 "/usr/include/assert.h" 1 3 4
# 66 "/usr/include/assert.h" 3 4



extern void __assert_fail (const char *__assertion, const char *__file,
      unsigned int __line, const char *__function)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__noreturn__));


extern void __assert_perror_fail (int __errnum, const char *__file,
      unsigned int __line, const char *__function)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__noreturn__));




extern void __assert (const char *__assertion, const char *__file, int __line)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__noreturn__));



# 39 "cligen_var.c" 2
# 1 "/usr/include/string.h" 1 3 4
# 27 "/usr/include/string.h" 3 4





# 1 "/usr/lib/gcc/x86_64-linux-gnu/4.8/include/stddef.h" 1 3 4
# 33 "/usr/include/string.h" 2 3 4
# 44 "/usr/include/string.h" 3 4


extern void *memcpy (void *__restrict __dest, const void *__restrict __src,
       size_t __n) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));


extern void *memmove (void *__dest, const void *__src, size_t __n)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));






extern void *memccpy (void *__restrict __dest, const void *__restrict __src,
        int __c, size_t __n)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));





extern void *memset (void *__s, int __c, size_t __n) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));


extern int memcmp (const void *__s1, const void *__s2, size_t __n)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
# 96 "/usr/include/string.h" 3 4
extern void *memchr (const void *__s, int __c, size_t __n)
      __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));


# 127 "/usr/include/string.h" 3 4


extern char *strcpy (char *__restrict __dest, const char *__restrict __src)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));

extern char *strncpy (char *__restrict __dest,
        const char *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));


extern char *strcat (char *__restrict __dest, const char *__restrict __src)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));

extern char *strncat (char *__restrict __dest, const char *__restrict __src,
        size_t __n) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));


extern int strcmp (const char *__s1, const char *__s2)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));

extern int strncmp (const char *__s1, const char *__s2, size_t __n)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern int strcoll (const char *__s1, const char *__s2)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));

extern size_t strxfrm (char *__restrict __dest,
         const char *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));

# 166 "/usr/include/string.h" 3 4
extern int strcoll_l (const char *__s1, const char *__s2, __locale_t __l)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2, 3)));

extern size_t strxfrm_l (char *__dest, const char *__src, size_t __n,
    __locale_t __l) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2, 4)));





extern char *strdup (const char *__s)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__malloc__)) __attribute__ ((__nonnull__ (1)));






extern char *strndup (const char *__string, size_t __n)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__malloc__)) __attribute__ ((__nonnull__ (1)));
# 211 "/usr/include/string.h" 3 4

# 236 "/usr/include/string.h" 3 4
extern char *strchr (const char *__s, int __c)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));
# 263 "/usr/include/string.h" 3 4
extern char *strrchr (const char *__s, int __c)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));


# 282 "/usr/include/string.h" 3 4



extern size_t strcspn (const char *__s, const char *__reject)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern size_t strspn (const char *__s, const char *__accept)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
# 315 "/usr/include/string.h" 3 4
extern char *strpbrk (const char *__s, const char *__accept)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
# 342 "/usr/include/string.h" 3 4
extern char *strstr (const char *__haystack, const char *__needle)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));




extern char *strtok (char *__restrict __s, const char *__restrict __delim)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));




extern char *__strtok_r (char *__restrict __s,
    const char *__restrict __delim,
    char **__restrict __save_ptr)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2, 3)));

extern char *strtok_r (char *__restrict __s, const char *__restrict __delim,
         char **__restrict __save_ptr)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2, 3)));
# 397 "/usr/include/string.h" 3 4


extern size_t strlen (const char *__s)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));





extern size_t strnlen (const char *__string, size_t __maxlen)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));





extern char *strerror (int __errnum) __attribute__ ((__nothrow__ , __leaf__));

# 427 "/usr/include/string.h" 3 4
extern int strerror_r (int __errnum, char *__buf, size_t __buflen) __asm__ ("" "__xpg_strerror_r") __attribute__ ((__nothrow__ , __leaf__))

                        __attribute__ ((__nonnull__ (2)));
# 445 "/usr/include/string.h" 3 4
extern char *strerror_l (int __errnum, __locale_t __l) __attribute__ ((__nothrow__ , __leaf__));





extern void __bzero (void *__s, size_t __n) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));



extern void bcopy (const void *__src, void *__dest, size_t __n)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));


extern void bzero (void *__s, size_t __n) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));


extern int bcmp (const void *__s1, const void *__s2, size_t __n)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
# 489 "/usr/include/string.h" 3 4
extern char *index (const char *__s, int __c)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));
# 517 "/usr/include/string.h" 3 4
extern char *rindex (const char *__s, int __c)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));




extern int ffs (int __i) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
# 534 "/usr/include/string.h" 3 4
extern int strcasecmp (const char *__s1, const char *__s2)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern int strncasecmp (const char *__s1, const char *__s2, size_t __n)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
# 557 "/usr/include/string.h" 3 4
extern char *strsep (char **__restrict __stringp,
       const char *__restrict __delim)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));




extern char *strsignal (int __sig) __attribute__ ((__nothrow__ , __leaf__));


extern char *__stpcpy (char *__restrict __dest, const char *__restrict __src)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));
extern char *stpcpy (char *__restrict __dest, const char *__restrict __src)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));



extern char *__stpncpy (char *__restrict __dest,
   const char *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));
extern char *stpncpy (char *__restrict __dest,
        const char *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));
# 644 "/usr/include/string.h" 3 4

# 40 "cligen_var.c" 2
# 1 "/usr/include/errno.h" 1 3 4
# 31 "/usr/include/errno.h" 3 4




# 1 "/usr/include/x86_64-linux-gnu/bits/errno.h" 1 3 4
# 24 "/usr/include/x86_64-linux-gnu/bits/errno.h" 3 4
# 1 "/usr/include/linux/errno.h" 1 3 4
# 1 "/usr/include/x86_64-linux-gnu/asm/errno.h" 1 3 4
# 1 "/usr/include/asm-generic/errno.h" 1 3 4



# 1 "/usr/include/asm-generic/errno-base.h" 1 3 4
# 5 "/usr/include/asm-generic/errno.h" 2 3 4
# 1 "/usr/include/x86_64-linux-gnu/asm/errno.h" 2 3 4
# 1 "/usr/include/linux/errno.h" 2 3 4
# 25 "/usr/include/x86_64-linux-gnu/bits/errno.h" 2 3 4
# 50 "/usr/include/x86_64-linux-gnu/bits/errno.h" 3 4
extern int *__errno_location (void) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
# 36 "/usr/include/errno.h" 2 3 4
# 58 "/usr/include/errno.h" 3 4

# 41 "cligen_var.c" 2
# 1 "/usr/include/math.h" 1 3 4
# 28 "/usr/include/math.h" 3 4




# 1 "/usr/include/x86_64-linux-gnu/bits/huge_val.h" 1 3 4
# 33 "/usr/include/math.h" 2 3 4

# 1 "/usr/include/x86_64-linux-gnu/bits/huge_valf.h" 1 3 4
# 35 "/usr/include/math.h" 2 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/huge_vall.h" 1 3 4
# 36 "/usr/include/math.h" 2 3 4


# 1 "/usr/include/x86_64-linux-gnu/bits/inf.h" 1 3 4
# 39 "/usr/include/math.h" 2 3 4


# 1 "/usr/include/x86_64-linux-gnu/bits/nan.h" 1 3 4
# 42 "/usr/include/math.h" 2 3 4



# 1 "/usr/include/x86_64-linux-gnu/bits/mathdef.h" 1 3 4
# 28 "/usr/include/x86_64-linux-gnu/bits/mathdef.h" 3 4
typedef float float_t;
typedef double double_t;
# 46 "/usr/include/math.h" 2 3 4
# 69 "/usr/include/math.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/mathcalls.h" 1 3 4
# 52 "/usr/include/x86_64-linux-gnu/bits/mathcalls.h" 3 4


extern double acos (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __acos (double __x) __attribute__ ((__nothrow__ , __leaf__));

extern double asin (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __asin (double __x) __attribute__ ((__nothrow__ , __leaf__));

extern double atan (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __atan (double __x) __attribute__ ((__nothrow__ , __leaf__));

extern double atan2 (double __y, double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __atan2 (double __y, double __x) __attribute__ ((__nothrow__ , __leaf__));


extern double cos (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __cos (double __x) __attribute__ ((__nothrow__ , __leaf__));

extern double sin (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __sin (double __x) __attribute__ ((__nothrow__ , __leaf__));

extern double tan (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __tan (double __x) __attribute__ ((__nothrow__ , __leaf__));




extern double cosh (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __cosh (double __x) __attribute__ ((__nothrow__ , __leaf__));

extern double sinh (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __sinh (double __x) __attribute__ ((__nothrow__ , __leaf__));

extern double tanh (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __tanh (double __x) __attribute__ ((__nothrow__ , __leaf__));

# 86 "/usr/include/x86_64-linux-gnu/bits/mathcalls.h" 3 4


extern double acosh (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __acosh (double __x) __attribute__ ((__nothrow__ , __leaf__));

extern double asinh (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __asinh (double __x) __attribute__ ((__nothrow__ , __leaf__));

extern double atanh (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __atanh (double __x) __attribute__ ((__nothrow__ , __leaf__));







extern double exp (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __exp (double __x) __attribute__ ((__nothrow__ , __leaf__));


extern double frexp (double __x, int *__exponent) __attribute__ ((__nothrow__ , __leaf__)); extern double __frexp (double __x, int *__exponent) __attribute__ ((__nothrow__ , __leaf__));


extern double ldexp (double __x, int __exponent) __attribute__ ((__nothrow__ , __leaf__)); extern double __ldexp (double __x, int __exponent) __attribute__ ((__nothrow__ , __leaf__));


extern double log (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __log (double __x) __attribute__ ((__nothrow__ , __leaf__));


extern double log10 (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __log10 (double __x) __attribute__ ((__nothrow__ , __leaf__));


extern double modf (double __x, double *__iptr) __attribute__ ((__nothrow__ , __leaf__)); extern double __modf (double __x, double *__iptr) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));

# 126 "/usr/include/x86_64-linux-gnu/bits/mathcalls.h" 3 4


extern double expm1 (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __expm1 (double __x) __attribute__ ((__nothrow__ , __leaf__));


extern double log1p (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __log1p (double __x) __attribute__ ((__nothrow__ , __leaf__));


extern double logb (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __logb (double __x) __attribute__ ((__nothrow__ , __leaf__));






extern double exp2 (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __exp2 (double __x) __attribute__ ((__nothrow__ , __leaf__));


extern double log2 (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __log2 (double __x) __attribute__ ((__nothrow__ , __leaf__));








extern double pow (double __x, double __y) __attribute__ ((__nothrow__ , __leaf__)); extern double __pow (double __x, double __y) __attribute__ ((__nothrow__ , __leaf__));


extern double sqrt (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __sqrt (double __x) __attribute__ ((__nothrow__ , __leaf__));





extern double hypot (double __x, double __y) __attribute__ ((__nothrow__ , __leaf__)); extern double __hypot (double __x, double __y) __attribute__ ((__nothrow__ , __leaf__));






extern double cbrt (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __cbrt (double __x) __attribute__ ((__nothrow__ , __leaf__));








extern double ceil (double __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern double __ceil (double __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));


extern double fabs (double __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern double __fabs (double __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));


extern double floor (double __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern double __floor (double __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));


extern double fmod (double __x, double __y) __attribute__ ((__nothrow__ , __leaf__)); extern double __fmod (double __x, double __y) __attribute__ ((__nothrow__ , __leaf__));




extern int __isinf (double __value) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));


extern int __finite (double __value) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));





extern int isinf (double __value) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));


extern int finite (double __value) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));


extern double drem (double __x, double __y) __attribute__ ((__nothrow__ , __leaf__)); extern double __drem (double __x, double __y) __attribute__ ((__nothrow__ , __leaf__));



extern double significand (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __significand (double __x) __attribute__ ((__nothrow__ , __leaf__));





extern double copysign (double __x, double __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern double __copysign (double __x, double __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));






extern double nan (const char *__tagb) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern double __nan (const char *__tagb) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));





extern int __isnan (double __value) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));



extern int isnan (double __value) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));


extern double j0 (double) __attribute__ ((__nothrow__ , __leaf__)); extern double __j0 (double) __attribute__ ((__nothrow__ , __leaf__));
extern double j1 (double) __attribute__ ((__nothrow__ , __leaf__)); extern double __j1 (double) __attribute__ ((__nothrow__ , __leaf__));
extern double jn (int, double) __attribute__ ((__nothrow__ , __leaf__)); extern double __jn (int, double) __attribute__ ((__nothrow__ , __leaf__));
extern double y0 (double) __attribute__ ((__nothrow__ , __leaf__)); extern double __y0 (double) __attribute__ ((__nothrow__ , __leaf__));
extern double y1 (double) __attribute__ ((__nothrow__ , __leaf__)); extern double __y1 (double) __attribute__ ((__nothrow__ , __leaf__));
extern double yn (int, double) __attribute__ ((__nothrow__ , __leaf__)); extern double __yn (int, double) __attribute__ ((__nothrow__ , __leaf__));






extern double erf (double) __attribute__ ((__nothrow__ , __leaf__)); extern double __erf (double) __attribute__ ((__nothrow__ , __leaf__));
extern double erfc (double) __attribute__ ((__nothrow__ , __leaf__)); extern double __erfc (double) __attribute__ ((__nothrow__ , __leaf__));
extern double lgamma (double) __attribute__ ((__nothrow__ , __leaf__)); extern double __lgamma (double) __attribute__ ((__nothrow__ , __leaf__));






extern double tgamma (double) __attribute__ ((__nothrow__ , __leaf__)); extern double __tgamma (double) __attribute__ ((__nothrow__ , __leaf__));





extern double gamma (double) __attribute__ ((__nothrow__ , __leaf__)); extern double __gamma (double) __attribute__ ((__nothrow__ , __leaf__));






extern double lgamma_r (double, int *__signgamp) __attribute__ ((__nothrow__ , __leaf__)); extern double __lgamma_r (double, int *__signgamp) __attribute__ ((__nothrow__ , __leaf__));







extern double rint (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __rint (double __x) __attribute__ ((__nothrow__ , __leaf__));


extern double nextafter (double __x, double __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern double __nextafter (double __x, double __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));

extern double nexttoward (double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern double __nexttoward (double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));



extern double remainder (double __x, double __y) __attribute__ ((__nothrow__ , __leaf__)); extern double __remainder (double __x, double __y) __attribute__ ((__nothrow__ , __leaf__));



extern double scalbn (double __x, int __n) __attribute__ ((__nothrow__ , __leaf__)); extern double __scalbn (double __x, int __n) __attribute__ ((__nothrow__ , __leaf__));



extern int ilogb (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern int __ilogb (double __x) __attribute__ ((__nothrow__ , __leaf__));




extern double scalbln (double __x, long int __n) __attribute__ ((__nothrow__ , __leaf__)); extern double __scalbln (double __x, long int __n) __attribute__ ((__nothrow__ , __leaf__));



extern double nearbyint (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern double __nearbyint (double __x) __attribute__ ((__nothrow__ , __leaf__));



extern double round (double __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern double __round (double __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));



extern double trunc (double __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern double __trunc (double __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));




extern double remquo (double __x, double __y, int *__quo) __attribute__ ((__nothrow__ , __leaf__)); extern double __remquo (double __x, double __y, int *__quo) __attribute__ ((__nothrow__ , __leaf__));






extern long int lrint (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long int __lrint (double __x) __attribute__ ((__nothrow__ , __leaf__));
__extension__
extern long long int llrint (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long long int __llrint (double __x) __attribute__ ((__nothrow__ , __leaf__));



extern long int lround (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long int __lround (double __x) __attribute__ ((__nothrow__ , __leaf__));
__extension__
extern long long int llround (double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long long int __llround (double __x) __attribute__ ((__nothrow__ , __leaf__));



extern double fdim (double __x, double __y) __attribute__ ((__nothrow__ , __leaf__)); extern double __fdim (double __x, double __y) __attribute__ ((__nothrow__ , __leaf__));


extern double fmax (double __x, double __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern double __fmax (double __x, double __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));


extern double fmin (double __x, double __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern double __fmin (double __x, double __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));



extern int __fpclassify (double __value) __attribute__ ((__nothrow__ , __leaf__))
     __attribute__ ((__const__));


extern int __signbit (double __value) __attribute__ ((__nothrow__ , __leaf__))
     __attribute__ ((__const__));



extern double fma (double __x, double __y, double __z) __attribute__ ((__nothrow__ , __leaf__)); extern double __fma (double __x, double __y, double __z) __attribute__ ((__nothrow__ , __leaf__));




# 371 "/usr/include/x86_64-linux-gnu/bits/mathcalls.h" 3 4
extern double scalb (double __x, double __n) __attribute__ ((__nothrow__ , __leaf__)); extern double __scalb (double __x, double __n) __attribute__ ((__nothrow__ , __leaf__));
# 70 "/usr/include/math.h" 2 3 4
# 88 "/usr/include/math.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/mathcalls.h" 1 3 4
# 52 "/usr/include/x86_64-linux-gnu/bits/mathcalls.h" 3 4


extern float acosf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __acosf (float __x) __attribute__ ((__nothrow__ , __leaf__));

extern float asinf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __asinf (float __x) __attribute__ ((__nothrow__ , __leaf__));

extern float atanf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __atanf (float __x) __attribute__ ((__nothrow__ , __leaf__));

extern float atan2f (float __y, float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __atan2f (float __y, float __x) __attribute__ ((__nothrow__ , __leaf__));


extern float cosf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __cosf (float __x) __attribute__ ((__nothrow__ , __leaf__));

extern float sinf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __sinf (float __x) __attribute__ ((__nothrow__ , __leaf__));

extern float tanf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __tanf (float __x) __attribute__ ((__nothrow__ , __leaf__));




extern float coshf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __coshf (float __x) __attribute__ ((__nothrow__ , __leaf__));

extern float sinhf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __sinhf (float __x) __attribute__ ((__nothrow__ , __leaf__));

extern float tanhf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __tanhf (float __x) __attribute__ ((__nothrow__ , __leaf__));

# 86 "/usr/include/x86_64-linux-gnu/bits/mathcalls.h" 3 4


extern float acoshf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __acoshf (float __x) __attribute__ ((__nothrow__ , __leaf__));

extern float asinhf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __asinhf (float __x) __attribute__ ((__nothrow__ , __leaf__));

extern float atanhf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __atanhf (float __x) __attribute__ ((__nothrow__ , __leaf__));







extern float expf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __expf (float __x) __attribute__ ((__nothrow__ , __leaf__));


extern float frexpf (float __x, int *__exponent) __attribute__ ((__nothrow__ , __leaf__)); extern float __frexpf (float __x, int *__exponent) __attribute__ ((__nothrow__ , __leaf__));


extern float ldexpf (float __x, int __exponent) __attribute__ ((__nothrow__ , __leaf__)); extern float __ldexpf (float __x, int __exponent) __attribute__ ((__nothrow__ , __leaf__));


extern float logf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __logf (float __x) __attribute__ ((__nothrow__ , __leaf__));


extern float log10f (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __log10f (float __x) __attribute__ ((__nothrow__ , __leaf__));


extern float modff (float __x, float *__iptr) __attribute__ ((__nothrow__ , __leaf__)); extern float __modff (float __x, float *__iptr) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));

# 126 "/usr/include/x86_64-linux-gnu/bits/mathcalls.h" 3 4


extern float expm1f (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __expm1f (float __x) __attribute__ ((__nothrow__ , __leaf__));


extern float log1pf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __log1pf (float __x) __attribute__ ((__nothrow__ , __leaf__));


extern float logbf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __logbf (float __x) __attribute__ ((__nothrow__ , __leaf__));






extern float exp2f (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __exp2f (float __x) __attribute__ ((__nothrow__ , __leaf__));


extern float log2f (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __log2f (float __x) __attribute__ ((__nothrow__ , __leaf__));








extern float powf (float __x, float __y) __attribute__ ((__nothrow__ , __leaf__)); extern float __powf (float __x, float __y) __attribute__ ((__nothrow__ , __leaf__));


extern float sqrtf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __sqrtf (float __x) __attribute__ ((__nothrow__ , __leaf__));





extern float hypotf (float __x, float __y) __attribute__ ((__nothrow__ , __leaf__)); extern float __hypotf (float __x, float __y) __attribute__ ((__nothrow__ , __leaf__));






extern float cbrtf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __cbrtf (float __x) __attribute__ ((__nothrow__ , __leaf__));








extern float ceilf (float __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern float __ceilf (float __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));


extern float fabsf (float __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern float __fabsf (float __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));


extern float floorf (float __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern float __floorf (float __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));


extern float fmodf (float __x, float __y) __attribute__ ((__nothrow__ , __leaf__)); extern float __fmodf (float __x, float __y) __attribute__ ((__nothrow__ , __leaf__));




extern int __isinff (float __value) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));


extern int __finitef (float __value) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));





extern int isinff (float __value) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));


extern int finitef (float __value) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));


extern float dremf (float __x, float __y) __attribute__ ((__nothrow__ , __leaf__)); extern float __dremf (float __x, float __y) __attribute__ ((__nothrow__ , __leaf__));



extern float significandf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __significandf (float __x) __attribute__ ((__nothrow__ , __leaf__));





extern float copysignf (float __x, float __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern float __copysignf (float __x, float __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));






extern float nanf (const char *__tagb) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern float __nanf (const char *__tagb) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));





extern int __isnanf (float __value) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));



extern int isnanf (float __value) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));


extern float j0f (float) __attribute__ ((__nothrow__ , __leaf__)); extern float __j0f (float) __attribute__ ((__nothrow__ , __leaf__));
extern float j1f (float) __attribute__ ((__nothrow__ , __leaf__)); extern float __j1f (float) __attribute__ ((__nothrow__ , __leaf__));
extern float jnf (int, float) __attribute__ ((__nothrow__ , __leaf__)); extern float __jnf (int, float) __attribute__ ((__nothrow__ , __leaf__));
extern float y0f (float) __attribute__ ((__nothrow__ , __leaf__)); extern float __y0f (float) __attribute__ ((__nothrow__ , __leaf__));
extern float y1f (float) __attribute__ ((__nothrow__ , __leaf__)); extern float __y1f (float) __attribute__ ((__nothrow__ , __leaf__));
extern float ynf (int, float) __attribute__ ((__nothrow__ , __leaf__)); extern float __ynf (int, float) __attribute__ ((__nothrow__ , __leaf__));






extern float erff (float) __attribute__ ((__nothrow__ , __leaf__)); extern float __erff (float) __attribute__ ((__nothrow__ , __leaf__));
extern float erfcf (float) __attribute__ ((__nothrow__ , __leaf__)); extern float __erfcf (float) __attribute__ ((__nothrow__ , __leaf__));
extern float lgammaf (float) __attribute__ ((__nothrow__ , __leaf__)); extern float __lgammaf (float) __attribute__ ((__nothrow__ , __leaf__));






extern float tgammaf (float) __attribute__ ((__nothrow__ , __leaf__)); extern float __tgammaf (float) __attribute__ ((__nothrow__ , __leaf__));





extern float gammaf (float) __attribute__ ((__nothrow__ , __leaf__)); extern float __gammaf (float) __attribute__ ((__nothrow__ , __leaf__));






extern float lgammaf_r (float, int *__signgamp) __attribute__ ((__nothrow__ , __leaf__)); extern float __lgammaf_r (float, int *__signgamp) __attribute__ ((__nothrow__ , __leaf__));







extern float rintf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __rintf (float __x) __attribute__ ((__nothrow__ , __leaf__));


extern float nextafterf (float __x, float __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern float __nextafterf (float __x, float __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));

extern float nexttowardf (float __x, long double __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern float __nexttowardf (float __x, long double __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));



extern float remainderf (float __x, float __y) __attribute__ ((__nothrow__ , __leaf__)); extern float __remainderf (float __x, float __y) __attribute__ ((__nothrow__ , __leaf__));



extern float scalbnf (float __x, int __n) __attribute__ ((__nothrow__ , __leaf__)); extern float __scalbnf (float __x, int __n) __attribute__ ((__nothrow__ , __leaf__));



extern int ilogbf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern int __ilogbf (float __x) __attribute__ ((__nothrow__ , __leaf__));




extern float scalblnf (float __x, long int __n) __attribute__ ((__nothrow__ , __leaf__)); extern float __scalblnf (float __x, long int __n) __attribute__ ((__nothrow__ , __leaf__));



extern float nearbyintf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern float __nearbyintf (float __x) __attribute__ ((__nothrow__ , __leaf__));



extern float roundf (float __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern float __roundf (float __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));



extern float truncf (float __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern float __truncf (float __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));




extern float remquof (float __x, float __y, int *__quo) __attribute__ ((__nothrow__ , __leaf__)); extern float __remquof (float __x, float __y, int *__quo) __attribute__ ((__nothrow__ , __leaf__));






extern long int lrintf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern long int __lrintf (float __x) __attribute__ ((__nothrow__ , __leaf__));
__extension__
extern long long int llrintf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern long long int __llrintf (float __x) __attribute__ ((__nothrow__ , __leaf__));



extern long int lroundf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern long int __lroundf (float __x) __attribute__ ((__nothrow__ , __leaf__));
__extension__
extern long long int llroundf (float __x) __attribute__ ((__nothrow__ , __leaf__)); extern long long int __llroundf (float __x) __attribute__ ((__nothrow__ , __leaf__));



extern float fdimf (float __x, float __y) __attribute__ ((__nothrow__ , __leaf__)); extern float __fdimf (float __x, float __y) __attribute__ ((__nothrow__ , __leaf__));


extern float fmaxf (float __x, float __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern float __fmaxf (float __x, float __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));


extern float fminf (float __x, float __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern float __fminf (float __x, float __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));



extern int __fpclassifyf (float __value) __attribute__ ((__nothrow__ , __leaf__))
     __attribute__ ((__const__));


extern int __signbitf (float __value) __attribute__ ((__nothrow__ , __leaf__))
     __attribute__ ((__const__));



extern float fmaf (float __x, float __y, float __z) __attribute__ ((__nothrow__ , __leaf__)); extern float __fmaf (float __x, float __y, float __z) __attribute__ ((__nothrow__ , __leaf__));




# 371 "/usr/include/x86_64-linux-gnu/bits/mathcalls.h" 3 4
extern float scalbf (float __x, float __n) __attribute__ ((__nothrow__ , __leaf__)); extern float __scalbf (float __x, float __n) __attribute__ ((__nothrow__ , __leaf__));
# 89 "/usr/include/math.h" 2 3 4
# 132 "/usr/include/math.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/mathcalls.h" 1 3 4
# 52 "/usr/include/x86_64-linux-gnu/bits/mathcalls.h" 3 4


extern long double acosl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __acosl (long double __x) __attribute__ ((__nothrow__ , __leaf__));

extern long double asinl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __asinl (long double __x) __attribute__ ((__nothrow__ , __leaf__));

extern long double atanl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __atanl (long double __x) __attribute__ ((__nothrow__ , __leaf__));

extern long double atan2l (long double __y, long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __atan2l (long double __y, long double __x) __attribute__ ((__nothrow__ , __leaf__));


extern long double cosl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __cosl (long double __x) __attribute__ ((__nothrow__ , __leaf__));

extern long double sinl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __sinl (long double __x) __attribute__ ((__nothrow__ , __leaf__));

extern long double tanl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __tanl (long double __x) __attribute__ ((__nothrow__ , __leaf__));




extern long double coshl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __coshl (long double __x) __attribute__ ((__nothrow__ , __leaf__));

extern long double sinhl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __sinhl (long double __x) __attribute__ ((__nothrow__ , __leaf__));

extern long double tanhl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __tanhl (long double __x) __attribute__ ((__nothrow__ , __leaf__));

# 86 "/usr/include/x86_64-linux-gnu/bits/mathcalls.h" 3 4


extern long double acoshl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __acoshl (long double __x) __attribute__ ((__nothrow__ , __leaf__));

extern long double asinhl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __asinhl (long double __x) __attribute__ ((__nothrow__ , __leaf__));

extern long double atanhl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __atanhl (long double __x) __attribute__ ((__nothrow__ , __leaf__));







extern long double expl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __expl (long double __x) __attribute__ ((__nothrow__ , __leaf__));


extern long double frexpl (long double __x, int *__exponent) __attribute__ ((__nothrow__ , __leaf__)); extern long double __frexpl (long double __x, int *__exponent) __attribute__ ((__nothrow__ , __leaf__));


extern long double ldexpl (long double __x, int __exponent) __attribute__ ((__nothrow__ , __leaf__)); extern long double __ldexpl (long double __x, int __exponent) __attribute__ ((__nothrow__ , __leaf__));


extern long double logl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __logl (long double __x) __attribute__ ((__nothrow__ , __leaf__));


extern long double log10l (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __log10l (long double __x) __attribute__ ((__nothrow__ , __leaf__));


extern long double modfl (long double __x, long double *__iptr) __attribute__ ((__nothrow__ , __leaf__)); extern long double __modfl (long double __x, long double *__iptr) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));

# 126 "/usr/include/x86_64-linux-gnu/bits/mathcalls.h" 3 4


extern long double expm1l (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __expm1l (long double __x) __attribute__ ((__nothrow__ , __leaf__));


extern long double log1pl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __log1pl (long double __x) __attribute__ ((__nothrow__ , __leaf__));


extern long double logbl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __logbl (long double __x) __attribute__ ((__nothrow__ , __leaf__));






extern long double exp2l (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __exp2l (long double __x) __attribute__ ((__nothrow__ , __leaf__));


extern long double log2l (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __log2l (long double __x) __attribute__ ((__nothrow__ , __leaf__));








extern long double powl (long double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__)); extern long double __powl (long double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__));


extern long double sqrtl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __sqrtl (long double __x) __attribute__ ((__nothrow__ , __leaf__));





extern long double hypotl (long double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__)); extern long double __hypotl (long double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__));






extern long double cbrtl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __cbrtl (long double __x) __attribute__ ((__nothrow__ , __leaf__));








extern long double ceill (long double __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern long double __ceill (long double __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));


extern long double fabsl (long double __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern long double __fabsl (long double __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));


extern long double floorl (long double __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern long double __floorl (long double __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));


extern long double fmodl (long double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__)); extern long double __fmodl (long double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__));




extern int __isinfl (long double __value) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));


extern int __finitel (long double __value) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));





extern int isinfl (long double __value) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));


extern int finitel (long double __value) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));


extern long double dreml (long double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__)); extern long double __dreml (long double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__));



extern long double significandl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __significandl (long double __x) __attribute__ ((__nothrow__ , __leaf__));





extern long double copysignl (long double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern long double __copysignl (long double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));






extern long double nanl (const char *__tagb) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern long double __nanl (const char *__tagb) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));





extern int __isnanl (long double __value) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));



extern int isnanl (long double __value) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));


extern long double j0l (long double) __attribute__ ((__nothrow__ , __leaf__)); extern long double __j0l (long double) __attribute__ ((__nothrow__ , __leaf__));
extern long double j1l (long double) __attribute__ ((__nothrow__ , __leaf__)); extern long double __j1l (long double) __attribute__ ((__nothrow__ , __leaf__));
extern long double jnl (int, long double) __attribute__ ((__nothrow__ , __leaf__)); extern long double __jnl (int, long double) __attribute__ ((__nothrow__ , __leaf__));
extern long double y0l (long double) __attribute__ ((__nothrow__ , __leaf__)); extern long double __y0l (long double) __attribute__ ((__nothrow__ , __leaf__));
extern long double y1l (long double) __attribute__ ((__nothrow__ , __leaf__)); extern long double __y1l (long double) __attribute__ ((__nothrow__ , __leaf__));
extern long double ynl (int, long double) __attribute__ ((__nothrow__ , __leaf__)); extern long double __ynl (int, long double) __attribute__ ((__nothrow__ , __leaf__));






extern long double erfl (long double) __attribute__ ((__nothrow__ , __leaf__)); extern long double __erfl (long double) __attribute__ ((__nothrow__ , __leaf__));
extern long double erfcl (long double) __attribute__ ((__nothrow__ , __leaf__)); extern long double __erfcl (long double) __attribute__ ((__nothrow__ , __leaf__));
extern long double lgammal (long double) __attribute__ ((__nothrow__ , __leaf__)); extern long double __lgammal (long double) __attribute__ ((__nothrow__ , __leaf__));






extern long double tgammal (long double) __attribute__ ((__nothrow__ , __leaf__)); extern long double __tgammal (long double) __attribute__ ((__nothrow__ , __leaf__));





extern long double gammal (long double) __attribute__ ((__nothrow__ , __leaf__)); extern long double __gammal (long double) __attribute__ ((__nothrow__ , __leaf__));






extern long double lgammal_r (long double, int *__signgamp) __attribute__ ((__nothrow__ , __leaf__)); extern long double __lgammal_r (long double, int *__signgamp) __attribute__ ((__nothrow__ , __leaf__));







extern long double rintl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __rintl (long double __x) __attribute__ ((__nothrow__ , __leaf__));


extern long double nextafterl (long double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern long double __nextafterl (long double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));

extern long double nexttowardl (long double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern long double __nexttowardl (long double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));



extern long double remainderl (long double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__)); extern long double __remainderl (long double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__));



extern long double scalbnl (long double __x, int __n) __attribute__ ((__nothrow__ , __leaf__)); extern long double __scalbnl (long double __x, int __n) __attribute__ ((__nothrow__ , __leaf__));



extern int ilogbl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern int __ilogbl (long double __x) __attribute__ ((__nothrow__ , __leaf__));




extern long double scalblnl (long double __x, long int __n) __attribute__ ((__nothrow__ , __leaf__)); extern long double __scalblnl (long double __x, long int __n) __attribute__ ((__nothrow__ , __leaf__));



extern long double nearbyintl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long double __nearbyintl (long double __x) __attribute__ ((__nothrow__ , __leaf__));



extern long double roundl (long double __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern long double __roundl (long double __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));



extern long double truncl (long double __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern long double __truncl (long double __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));




extern long double remquol (long double __x, long double __y, int *__quo) __attribute__ ((__nothrow__ , __leaf__)); extern long double __remquol (long double __x, long double __y, int *__quo) __attribute__ ((__nothrow__ , __leaf__));






extern long int lrintl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long int __lrintl (long double __x) __attribute__ ((__nothrow__ , __leaf__));
__extension__
extern long long int llrintl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long long int __llrintl (long double __x) __attribute__ ((__nothrow__ , __leaf__));



extern long int lroundl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long int __lroundl (long double __x) __attribute__ ((__nothrow__ , __leaf__));
__extension__
extern long long int llroundl (long double __x) __attribute__ ((__nothrow__ , __leaf__)); extern long long int __llroundl (long double __x) __attribute__ ((__nothrow__ , __leaf__));



extern long double fdiml (long double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__)); extern long double __fdiml (long double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__));


extern long double fmaxl (long double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern long double __fmaxl (long double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));


extern long double fminl (long double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)); extern long double __fminl (long double __x, long double __y) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));



extern int __fpclassifyl (long double __value) __attribute__ ((__nothrow__ , __leaf__))
     __attribute__ ((__const__));


extern int __signbitl (long double __value) __attribute__ ((__nothrow__ , __leaf__))
     __attribute__ ((__const__));



extern long double fmal (long double __x, long double __y, long double __z) __attribute__ ((__nothrow__ , __leaf__)); extern long double __fmal (long double __x, long double __y, long double __z) __attribute__ ((__nothrow__ , __leaf__));




# 371 "/usr/include/x86_64-linux-gnu/bits/mathcalls.h" 3 4
extern long double scalbl (long double __x, long double __n) __attribute__ ((__nothrow__ , __leaf__)); extern long double __scalbl (long double __x, long double __n) __attribute__ ((__nothrow__ , __leaf__));
# 133 "/usr/include/math.h" 2 3 4
# 148 "/usr/include/math.h" 3 4
extern int signgam;
# 189 "/usr/include/math.h" 3 4
enum
  {
    FP_NAN =

      0,
    FP_INFINITE =

      1,
    FP_ZERO =

      2,
    FP_SUBNORMAL =

      3,
    FP_NORMAL =

      4
  };
# 301 "/usr/include/math.h" 3 4
typedef enum
{
  _IEEE_ = -1,
  _SVID_,
  _XOPEN_,
  _POSIX_,
  _ISOC_
} _LIB_VERSION_TYPE;




extern _LIB_VERSION_TYPE _LIB_VERSION;
# 326 "/usr/include/math.h" 3 4
struct exception

  {
    int type;
    char *name;
    double arg1;
    double arg2;
    double retval;
  };




extern int matherr (struct exception *__exc);
# 488 "/usr/include/math.h" 3 4

# 42 "cligen_var.c" 2

# 1 "/usr/include/regex.h" 1 3 4
# 24 "/usr/include/regex.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/gnu/option-groups.h" 1 3 4
# 25 "/usr/include/regex.h" 2 3 4
# 35 "/usr/include/regex.h" 3 4
typedef long int s_reg_t;
typedef unsigned long int active_reg_t;






typedef unsigned long int reg_syntax_t;
# 185 "/usr/include/regex.h" 3 4
extern reg_syntax_t re_syntax_options;
# 319 "/usr/include/regex.h" 3 4
typedef enum
{

  REG_ENOSYS = -1,


  REG_NOERROR = 0,
  REG_NOMATCH,



  REG_BADPAT,
  REG_ECOLLATE,
  REG_ECTYPE,
  REG_EESCAPE,
  REG_ESUBREG,
  REG_EBRACK,
  REG_EPAREN,
  REG_EBRACE,
  REG_BADBR,
  REG_ERANGE,
  REG_ESPACE,
  REG_BADRPT,


  REG_EEND,
  REG_ESIZE,
  REG_ERPAREN
} reg_errcode_t;
# 368 "/usr/include/regex.h" 3 4
struct re_pattern_buffer
{



  unsigned char *__buffer;


  unsigned long int __allocated;


  unsigned long int __used;


  reg_syntax_t __syntax;




  char *__fastmap;





  unsigned char * __translate;


  size_t re_nsub;





  unsigned __can_be_null : 1;
# 413 "/usr/include/regex.h" 3 4
  unsigned __regs_allocated : 2;



  unsigned __fastmap_accurate : 1;



  unsigned __no_sub : 1;



  unsigned __not_bol : 1;


  unsigned __not_eol : 1;


  unsigned __newline_anchor : 1;
};

typedef struct re_pattern_buffer regex_t;


typedef int regoff_t;
# 463 "/usr/include/regex.h" 3 4
typedef struct
{
  regoff_t rm_so;
  regoff_t rm_eo;
} regmatch_t;
# 573 "/usr/include/regex.h" 3 4
extern int regcomp (regex_t *__restrict __preg,
      const char *__restrict __pattern,
      int __cflags);

extern int regexec (const regex_t *__restrict __preg,
      const char *__restrict __string, size_t __nmatch,
      regmatch_t __pmatch[__restrict],
      int __eflags);

extern size_t regerror (int __errcode, const regex_t *__restrict __preg,
   char *__restrict __errbuf, size_t __errbuf_size);

extern void regfree (regex_t *__preg);
# 44 "cligen_var.c" 2


# 1 "cligen_var.h" 1
# 30 "cligen_var.h"
typedef unsigned char uuid_t[16];
# 39 "cligen_var.h"
enum cv_type{
  CGV_ERR=0,
  CGV_INT8,
  CGV_INT16,
  CGV_INT32,
  CGV_INT64,
  CGV_UINT8,
  CGV_UINT16,
  CGV_UINT32,
  CGV_UINT64,
  CGV_DEC64,
  CGV_BOOL,
  CGV_REST,
  CGV_STRING,
  CGV_INTERFACE,
  CGV_IPV4ADDR,
  CGV_IPV4PFX,
  CGV_IPV6ADDR,
  CGV_IPV6PFX,
  CGV_MACADDR,
  CGV_URL,
  CGV_UUID,
  CGV_TIME,
  CGV_VOID,
};
# 105 "cligen_var.h"
typedef struct cg_var cg_var;

struct cg_obj;

struct cg_varspec;




char *cv_name_get(cg_var *cv);
char *cv_name_set(cg_var *cv, char *s0);
enum cv_type cv_type_get(cg_var *cv);
enum cv_type cv_type_set(cg_var *cv, enum cv_type x);
char cv_const_get(cg_var *cv);
char cv_const_set(cg_var *cv, int c);

char cv_flag(cg_var *cv, char mask);
char cv_flag_clr(cg_var *cv, char mask);
char cv_flag_set(cg_var *cv, char mask);

void *cv_value_get(cg_var *cv);

char cv_bool_get(cg_var *cv);
char cv_bool_set(cg_var *cv, char x);
int8_t cv_int8_get(cg_var *cv);
int8_t cv_int8_set(cg_var *cv, int8_t x);
int16_t cv_int16_get(cg_var *cv);
int16_t cv_int16_set(cg_var *cv, int16_t x);
int32_t cv_int32_get(cg_var *cv);
int32_t cv_int32_set(cg_var *cv, int32_t x);
int64_t cv_int64_get(cg_var *cv);
int64_t cv_int64_set(cg_var *cv, int64_t x);

uint8_t cv_uint8_get(cg_var *cv);
uint8_t cv_uint8_set(cg_var *cv, uint8_t x);
uint16_t cv_uint16_get(cg_var *cv);
uint16_t cv_uint16_set(cg_var *cv, uint16_t x);
uint32_t cv_uint32_get(cg_var *cv);
uint32_t cv_uint32_set(cg_var *cv, uint32_t x);
uint64_t cv_uint64_get(cg_var *cv);
uint64_t cv_uint64_set(cg_var *cv, uint64_t x);

uint8_t cv_dec64_n_get(cg_var *cv);
uint8_t cv_dec64_n_set(cg_var *cv, uint8_t x);
int64_t cv_dec64_i_get(cg_var *cv);
int64_t cv_dec64_i_set(cg_var *cv, int64_t x);

char *cv_string_get(cg_var *cv);
char *cv_string_set(cg_var *cv, char *s0);
struct in_addr *cv_ipv4addr_get(cg_var *cv);
uint8_t cv_ipv4masklen_get(cg_var *cv);
struct in6_addr *cv_ipv6addr_get(cg_var *cv);
uint8_t cv_ipv6masklen_get(cg_var *cv);
char *cv_mac_get(cg_var *cv);
unsigned char *cv_uuid_get(cg_var *cv);
unsigned char *cv_uuid_set(cg_var *cv, unsigned char *u);
struct timeval cv_time_get(cg_var *cv);
struct timeval cv_time_set(cg_var *cv, struct timeval t);
char *cv_urlproto_get(cg_var *cv);
char *cv_urlproto_set(cg_var *cv, char *s0);
char *cv_urladdr_get(cg_var *cv);
char *cv_urladdr_set(cg_var *cv, char *s0);
char *cv_urlpath_get(cg_var *cv);
char *cv_urlpath_set(cg_var *cv, char *s0);
char *cv_urluser_get(cg_var *cv);
char *cv_urluser_set(cg_var *cv, char *s0);
char *cv_urlpasswd_get(cg_var *cv);
char *cv_urlpasswd_set(cg_var *cv, char *s0);

int parse_uint8(char *str, uint8_t *val, char **reason);
int parse_int64(char *str, int64_t *val, char **reason);
int parse_uint64(char *str, uint64_t *val, char **reason);
int str2urlproto(char *str);
int str2uuid(char *in, uuid_t u);
int uuid2str(uuid_t u, char *in, int len);
int str2time(char *in, struct timeval *tv);
int time2str(struct timeval tv, char *fmt, int len);

enum cv_type cv_str2type(char *str);
char *cv_type2str(enum cv_type type);
int cv_len(cg_var *cgv);
int cv2str(cg_var *cv, char *str, size_t size);
char *cv2str_dup(cg_var *cv);

int cv_print(FILE *f, cg_var *cgv);

int cv_cmp(cg_var *cgv1, cg_var *cgv2);
int cv_cp(cg_var *n, cg_var *old);
cg_var *cv_dup(cg_var *old);
int cv_parse(char *str, cg_var *cgv);
int cv_parse1(char *str, cg_var *cgv, char **reason);

int cv_validate(cg_var *cv, struct cg_varspec *cs, char **reason);
int cv_reset(cg_var *cgv);
int cv_free(cg_var *cv);
cg_var *cv_new(enum cv_type type);
int match_regexp(char *string, char *pattern0);
# 47 "cligen_var.c" 2
# 1 "cligen_cvec.h" 1
# 29 "cligen_cvec.h"
typedef struct cvec cvec;





cvec *cvec_new(int len);
int cvec_free(cvec *vr);
int cvec_init(cvec *vr, int len);
int cvec_reset(cvec *vr);

int cvec_len(cvec *vr);
cg_var *cvec_i(cvec *vr, int i);
cg_var *cvec_next(cvec *vr, cg_var *cv0);
cg_var *cvec_add(cvec *vr, enum cv_type type);
int cvec_del(cvec *vr, cg_var *del);


cg_var *cvec_each(cvec *vr, cg_var *prev);
cg_var *cvec_each1(cvec *vr, cg_var *prev);
cvec *cvec_dup(cvec *old);
int cvec_match(struct cg_obj *co_match, char *cmd, cvec *vr);
cvec *cvec_start(char *cmd);
int cvec_print(FILE *f, cvec *vr);

cg_var *cvec_find(cvec *vr, char *name);
cg_var *cvec_find_var(cvec *vr, char *name);
cg_var *cvec_find_keyword(cvec *vr, char *name);
char *cvec_find_str(cvec *vr, char *name);

int cv_exclude_keys(int status);
# 48 "cligen_var.c" 2
# 1 "cligen_gen.h" 1
# 31 "cligen_gen.h"
enum cg_objtype{
  CO_COMMAND,
  CO_VARIABLE,
  CO_REFERENCE
};







typedef int (cg_fnstype_t)(cligen_handle h, cvec *vars, cg_var *arg);






typedef int (expand_cb)(cligen_handle h,
   char *name,
   cvec *cvec,
   cg_var *arg,
   int *len,
   char ***commands,
   char ***helptexts);
# 71 "cligen_gen.h"
typedef int (cligen_susp_cb_t)(void *h, char *, int, int *);
# 86 "cligen_gen.h"
struct parse_tree{
    struct cg_obj **pt_vec;
    int pt_len;
    struct parse_tree *pt_up;
    int pt_obj;

};
typedef struct parse_tree parse_tree;



struct cg_callback {
    struct cg_callback *cc_next;
    cg_fnstype_t *cc_fn;
    char *cc_fn_str;
    cg_var *cc_arg;
};






struct cg_varspec{
    enum cv_type cgs_vtype;
    char *cgs_expand_fn_str;
    expand_cb *cgs_expand_fn;
    cg_var *cgs_expand_fn_arg;
    char *cgs_choice;
    int cgs_range;
    cg_var *cgs_rangecv_low;
    cg_var *cgs_rangecv_high;
    char *cgs_regex;
    uint8_t cgs_dec64_n;
};
typedef struct cg_varspec cg_varspec;
# 139 "cligen_gen.h"
struct cg_obj{
    parse_tree co_pt;
    struct cg_obj *co_prev;
    enum cg_objtype co_type;
    char co_delimiter;
    char *co_command;
    struct cg_callback *co_callbacks;
    cvec *co_cvec;
    int co_mark;
    char *co_help;



    int co_hide;
    char *co_mode;



    struct parse_tree co_pt_exp;
    struct cg_obj *co_ref;
    char *co_value;


    void *co_userdata;

    size_t co_userlen;




    int co_treeref;
    int co_refdone;



    int co_nonterminal;


    struct parse_tree co_pt_push;
    int co_pushed;


    union {
 struct { } cou_cmd;
 struct cg_varspec cou_var;
    } u;
};
typedef struct cg_obj cg_obj;

typedef cg_obj** pt_vec;


typedef int (cg_applyfn_t)(cg_obj *co, void *arg);
# 217 "cligen_gen.h"
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
# 272 "cligen_gen.h"
typedef cg_fnstype_t *(cg_str2fn_t)(char *str, void *arg, char **err);




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
# 49 "cligen_var.c" 2
# 1 "cligen_match.h" 1
# 32 "cligen_match.h"
int match_pattern(cligen_handle h, char *, parse_tree , int, int, pt_vec *,
    int **, int *, cvec *, char **);
int match_pattern_exact(cligen_handle h, char *, parse_tree, int exact, cvec *, cg_obj **);
int command_levels(char *string);
int extract_substring(char *string0, int level, char **sp);
int extract_substring_rest(char *string0, int level, char **sp);
int match_complete(cligen_handle h, char *string0, parse_tree pt, int maxlen, cvec *cvec);
# 50 "cligen_var.c" 2
# 1 "cligen_cv.h" 1
# 31 "cligen_cv.h"
struct cg_var {
    enum cv_type var_type;
    char *var_name;
    char var_const;
    char var_flag ;
    union {
 char varu_bool;
 int8_t varu_int8;
 int16_t varu_int16;
 int32_t varu_int32;
 int64_t varu_int64;
 uint8_t varu_uint8;
 uint16_t varu_uint16;
 uint32_t varu_uint32;
 uint64_t varu_uint64;
 char *varu_string;
 char *varu_interface;
 struct {
     int64_t vardec64_i;
     uint8_t vardec64_n;
 } varu_dec64;
 struct {
     struct in_addr varipv4_ipv4addr;
     uint8_t varipv4_masklen;
 } varu_ipv4addr;
 struct {
     struct in6_addr varipv6_ipv6addr;
     uint8_t varipv6_masklen;
 } varu_ipv6addr;
 char varu_macaddr[6];
 struct {
     char *varurl_proto;
     char *varurl_addr;
     char *varurl_path;
     char *varurl_user;
     char *varurl_passwd;
 } varu_url;
 uuid_t varu_uuid;
 struct timeval varu_time;
    } u;
};
# 51 "cligen_var.c" 2
# 1 "getline.h" 1
# 11 "getline.h"
typedef size_t (*gl_strwidth_proc)(char *);

int gl_eof(void);
void gl_exitchar_add(char c);

void gl_char_init(void);
void gl_char_cleanup(void);
char *gl_getline(cligen_handle h);
void gl_setwidth(int);
int gl_getwidth(void);
void gl_histadd(char *);
void gl_histclear();
void gl_strwidth(gl_strwidth_proc);
void gl_redraw(cligen_handle h);
int gl_regfd(int, int (*)(int, void*), void *);
int gl_unregfd(int);

int (*gl_in_hook)(void *, char *);
int (*gl_out_hook)(void*, char *);
int (*gl_tab_hook)(void*, char *, int, int *);
cligen_susp_cb_t *gl_susp_hook;
int (*gl_qmark_hook)(void*, char *, int);
# 52 "cligen_var.c" 2




static char *cg_urlprotostr[] = {
    ((void *)0),
    "file",
    "flash",
    "tftp",
    "ftp",
    "telnet",
    "http",
    "ssh",
    ((void *)0)
};



char *
cv_name_get(cg_var *cv)
{
    return cv->var_name;
}



char *
cv_name_set(cg_var *cv, char *s0)
{
    char *s1 = ((void *)0);


    if (s0){
 if ((s1 = strdup(s0)) == ((void *)0))
     return ((void *)0);
    }
    if (cv->var_name != ((void *)0))
 free(cv->var_name);
    cv->var_name = s1;
    return s1;
}



enum cv_type
cv_type_get(cg_var *cv)
{
    return cv->var_type;
}

enum cv_type
cv_type_set(cg_var *cv, enum cv_type x)
{
    return (cv->var_type = x);
}

char
cv_const_get(cg_var *cv)
{
    return cv->var_const;
}

char
cv_const_set(cg_var *cv, int c)
{
    return (cv->var_const = c);
}



char
cv_flag(cg_var *cv, char mask)
{
    return cv->var_flag & mask;
}



char
cv_flag_clr(cg_var *cv, char mask)
{
    return cv->var_flag ^= mask;
}



char
cv_flag_set(cg_var *cv, char mask)
{
    return cv->var_flag |= mask;
}



void *
cv_value_get(cg_var *cv)
{
    return &cv->u;
}



char
cv_bool_get(cg_var *cv)
{
    return ((cv)->u.varu_bool);
}

char
cv_bool_set(cg_var *cv, char x)
{
    return (cv->u.varu_bool = x);
}



int8_t
cv_int8_get(cg_var *cv)
{
    return ((cv)->u.varu_int8);
}



int8_t
cv_int8_set(cg_var *cv, int8_t x)
{
    return (cv->u.varu_int8 = x);
}



int16_t
cv_int16_get(cg_var *cv)
{
    return ((cv)->u.varu_int16);
}



int16_t
cv_int16_set(cg_var *cv, int16_t x)
{
    return (cv->u.varu_int16 = x);
}



int32_t
cv_int32_get(cg_var *cv)
{
    return ((cv)->u.varu_int32);
}



int32_t
cv_int32_set(cg_var *cv, int32_t x)
{
    return (cv->u.varu_int32 = x);
}



int64_t
cv_int64_get(cg_var *cv)
{
    return ((cv)->u.varu_int64);
}



int64_t
cv_int64_set(cg_var *cv, int64_t x)
{
    return (cv->u.varu_int64 = x);
}
# 239 "cligen_var.c"
int32_t
cv_int_get(cg_var *cv)
{
    return cv_int32_get(cv);
}
int32_t
cv_int_set(cg_var *cv, int32_t x)
{
    return cv_int32_set(cv, x);
}
int64_t
cv_long_get(cg_var *cv)
{
    return cv_int64_get(cv);
}
int64_t
cv_long_set(cg_var *cv, int64_t x)
{
    return cv_int64_set(cv, x);
}




uint8_t
cv_uint8_get(cg_var *cv)
{
    return ((cv)->u.varu_uint8);
}



uint8_t
cv_uint8_set(cg_var *cv, uint8_t x)
{
    return (cv->u.varu_uint8 = x);
}



uint16_t
cv_uint16_get(cg_var *cv)
{
    return ((cv)->u.varu_uint16);
}



uint16_t
cv_uint16_set(cg_var *cv, uint16_t x)
{
    return (cv->u.varu_uint16 = x);
}



uint32_t
cv_uint32_get(cg_var *cv)
{
    return ((cv)->u.varu_uint32);
}



uint32_t
cv_uint32_set(cg_var *cv, uint32_t x)
{
    return (cv->u.varu_uint32 = x);
}



uint64_t
cv_uint64_get(cg_var *cv)
{
    return ((cv)->u.varu_uint64);
}



uint64_t
cv_uint64_set(cg_var *cv, uint64_t x)
{
    return (cv->u.varu_uint64 = x);
}

uint8_t
cv_dec64_n_get(cg_var *cv)
{
    return ((cv)->u.varu_dec64.vardec64_n);
}


uint8_t
cv_dec64_n_set(cg_var *cv, uint8_t x)
{
    return (cv->u.varu_dec64.vardec64_n = x);
}

int64_t
cv_dec64_i_get(cg_var *cv)
{
    return ((cv)->u.varu_dec64.vardec64_i);
}

int64_t
cv_dec64_i_set(cg_var *cv, int64_t x)
{
    return (cv->u.varu_dec64.vardec64_i = x);
}





char *
cv_string_get(cg_var *cv)
{
    return ((cv)->u.varu_string);
}



char *
cv_string_set(cg_var *cv, char *s0)
{
    char *s1 = ((void *)0);


    if (s0){
 if ((s1 = strdup(s0)) == ((void *)0))
     return ((void *)0);
    }
    if (cv->u.varu_string != ((void *)0))
 free(cv->u.varu_string);
    cv->u.varu_string = s1;
    return s1;
}



struct in_addr *
cv_ipv4addr_get(cg_var *cv)
{
    return &cv->u.varu_ipv4addr.varipv4_ipv4addr;
}



uint8_t
cv_ipv4masklen_get(cg_var *cv)
{
    return cv->u.varu_ipv4addr.varipv4_masklen;
}



struct in6_addr *
cv_ipv6addr_get(cg_var *cv)
{
    return &cv->u.varu_ipv6addr.varipv6_ipv6addr;
}



uint8_t
cv_ipv6masklen_get(cg_var *cv)
{
    return cv->u.varu_ipv6addr.varipv6_masklen;
}





char *
cv_mac_get(cg_var *cv)
{
    return cv->u.varu_macaddr;
}





unsigned char *
cv_uuid_get(cg_var *cv)
{
    return cv->u.varu_uuid;
}

unsigned char *
cv_uuid_set(cg_var *cv, unsigned char *u)
{
    memcpy((char*)&cv->u.varu_uuid, u, 16);
    return cv->u.varu_uuid;
}



struct timeval
cv_time_get(cg_var *cv)
{
    return cv->u.varu_time;
}





struct timeval
cv_time_set(cg_var *cv, struct timeval t)
{
    cv->u.varu_time = t;
    return t;
}





char *
cv_urlproto_get(cg_var *cv)
{
    return (cv)->u.varu_url.varurl_proto;
}

char *
cv_urlproto_set(cg_var *cv, char *s0)
{
    char *s1 = ((void *)0);


    if (s0){
 if ((s1 = strdup(s0)) == ((void *)0))
     return ((void *)0);
    }
    if (cv->u.varu_url.varurl_proto != ((void *)0))
 free(cv->u.varu_url.varurl_proto);
    cv->u.varu_url.varurl_proto = s1;
    return s1;
}





char *
cv_urladdr_get(cg_var *cv)
{
    return (cv)->u.varu_url.varurl_addr;
}





char *
cv_urladdr_set(cg_var *cv, char *s0)
{
    char *s1 = ((void *)0);


    if (s0){
 if ((s1 = strdup(s0)) == ((void *)0))
     return ((void *)0);
    }
    if (cv->u.varu_url.varurl_addr != ((void *)0))
 free(cv->u.varu_url.varurl_addr);
    cv->u.varu_url.varurl_addr = s1;
    return s1;
}





char *
cv_urlpath_get(cg_var *cv)
{
    return (cv)->u.varu_url.varurl_path;
}





char *
cv_urlpath_set(cg_var *cv, char *s0)
{
    char *s1 = ((void *)0);


    if (s0){
 if ((s1 = strdup(s0)) == ((void *)0))
     return ((void *)0);
    }
    if (cv->u.varu_url.varurl_path != ((void *)0))
 free(cv->u.varu_url.varurl_path);
    cv->u.varu_url.varurl_path = s1;
    return s1;
}





char *
cv_urluser_get(cg_var *cv)
{
    return (cv)->u.varu_url.varurl_user;
}





char *
cv_urluser_set(cg_var *cv, char *s0)
{
    char *s1 = ((void *)0);


    if (s0){
 if ((s1 = strdup(s0)) == ((void *)0))
     return ((void *)0);
    }
    if (cv->u.varu_url.varurl_user != ((void *)0))
 free(cv->u.varu_url.varurl_user);
    cv->u.varu_url.varurl_user = s1;
    return s1;
}





char *
cv_urlpasswd_get(cg_var *cv)
{
    return (cv)->u.varu_url.varurl_passwd;
}





char *
cv_urlpasswd_set(cg_var *cv, char *s0)
{
    char *s1 = ((void *)0);


    if (s0){
 if ((s1 = strdup(s0)) == ((void *)0))
     return ((void *)0);
    }
    if (cv->u.varu_url.varurl_passwd != ((void *)0))
 free(cv->u.varu_url.varurl_passwd);
    cv->u.varu_url.varurl_passwd = s1;
    return s1;
}
# 614 "cligen_var.c"
static int
parse_int8(char *str, int8_t *val, char **reason)
{
    int64_t i;
    int retval;

    if ((retval = parse_int64(str, &i, reason)) != 1)
 goto done;
    if (i > 127 || i < -128){
 if (reason != ((void *)0))
     if ((*reason = cligen_reason("%s is out of range(type is int8)", str)) == ((void *)0)){
  retval = -1;
  goto done;
     }
 retval = 0;
 goto done;
    }
    *val = (int8_t)i;
  done:
    return retval;
}
# 644 "cligen_var.c"
static int
parse_int16(char *str, int16_t *val, char **reason)
{
    int64_t i;
    int retval;

    if ((retval = parse_int64(str, &i, reason)) != 1)
 goto done;
    if (i > 32676 || i < -32768){
 if (reason != ((void *)0))
     if ((*reason = cligen_reason("%s is out of range(type is int16)", str)) == ((void *)0)){
  retval = -1;
  goto done;
     }
 retval = 0;
 goto done;
    }
    *val = (int16_t)i;
  done:
    return retval;
}
# 674 "cligen_var.c"
static int
parse_int32(char *str, int32_t *val, char **reason)
{
    int64_t i;
    int retval;

    if ((retval = parse_int64(str, &i, reason)) != 1)
 goto done;
    if (i > 2147483647 || i < (-2147483647 - 1)){
 if (reason != ((void *)0))
     if ((*reason = cligen_reason("%s is out of range(type is int32)", str)) == ((void *)0)){
  retval = -1;
  goto done;
     }
 retval = 0;
 goto done;
    }
    *val = (int32_t)i;
  done:
    return retval;

}
# 705 "cligen_var.c"
int
parse_int64(char *str, int64_t *val, char **reason)
{
    int64_t i;
    char *ep;
    int retval = -1;

    (*__errno_location ()) = 0;
    i = strtoll(str, &ep, 0);
    if (str[0] == '\0' || *ep != '\0'){
 if (reason != ((void *)0))
     if ((*reason = cligen_reason("%s is not a number", str)) == ((void *)0)){
  retval = -1;
  goto done;
     }
 retval = 0;
 goto done;
    }
    if ((*__errno_location ()) != 0){
 if ((i == (-9223372036854775807LL -1) || i == 9223372036854775807LL) && (*__errno_location ()) == 34){
     if (reason != ((void *)0))
  if ((*reason = cligen_reason("%s is out of range (type is int64)", str)) == ((void *)0)){
      retval = -1;
      goto done;
  }
     retval = 0;
     goto done;
 }
 else{
     if ((*reason = cligen_reason("%s: %s", str, strerror((*__errno_location ())))) == ((void *)0)){
  retval = -1;
  goto done;
     }
 }
    }
    *val = i;
    retval = 1;
  done:
    return retval;
}
# 755 "cligen_var.c"
int
parse_uint8(char *str, uint8_t *val, char **reason)
{
    uint64_t i;
    int retval;

    if ((retval = parse_uint64(str, &i, reason)) != 1)
 goto done;
    if (i > 255){
 if (reason != ((void *)0))
     if ((*reason = cligen_reason("%s is out of range(type is uint8)", str)) == ((void *)0)){
  retval = -1;
  goto done;
     }
 retval = 0;
 goto done;
    }
    *val = (uint8_t)i;
  done:
    return retval;
}
# 785 "cligen_var.c"
static int
parse_uint16(char *str, uint16_t *val, char **reason)
{
    uint64_t i;
    int retval;

    if ((retval = parse_uint64(str, &i, reason)) != 1)
 goto done;
    if (i > 65535){
 if (reason != ((void *)0))
     if ((*reason = cligen_reason("%s is out of range(type is uint16)", str)) == ((void *)0)){
  retval = -1;
  goto done;
     }
 retval = 0;
 goto done;
    }
    *val = (uint16_t)i;
  done:
    return retval;
}
# 815 "cligen_var.c"
static int
parse_uint32(char *str, uint32_t *val, char **reason)
{
    uint64_t i;
    int retval;

    if ((retval = parse_uint64(str, &i, reason)) != 1)
 goto done;
    if (i > (2147483647 * 2U + 1U)){
 if (reason != ((void *)0))
     if ((*reason = cligen_reason("%s is out of range(type is uint32)", str)) == ((void *)0)){
  retval = -1;
  goto done;
     }
 retval = 0;
 goto done;
    }
    *val = (uint32_t)i;
  done:
    return retval;

}
# 847 "cligen_var.c"
int
parse_uint64(char *str, uint64_t *val, char **reason)
{
    uint64_t i;
    char *ep;
    int retval = -1;

    (*__errno_location ()) = 0;
    i = strtoull(str, &ep, 0);
    if (str[0] == '\0' || *ep != '\0'){
 if (reason != ((void *)0))
     if ((*reason = cligen_reason("%s is not a number", str)) == ((void *)0)){
  retval = -1;
  goto done;
     }
 retval = 0;
 goto done;
    }
    if ((*__errno_location ()) != 0){
 if (i == (9223372036854775807LL * 2ULL + 1) && (*__errno_location ()) == 34){
     if (reason != ((void *)0))
  if ((*reason = cligen_reason("%s is out of range (type is uint64)", str)) == ((void *)0)){
      retval = -1;
      goto done;
  }
     retval = 0;
     goto done;
 }
 else{
     if ((*reason = cligen_reason("%s: %s", str, strerror((*__errno_location ())))) == ((void *)0)){
  retval = -1;
  goto done;
     }
     retval = 0;
     goto done;
 }
    }

    if (strchr(str, '-') != ((void *)0)){
 if (reason != ((void *)0))
     if ((*reason = cligen_reason("%s is out of range (type is uint64)", str)) == ((void *)0)){
  retval = -1;
  goto done;
     }
 retval = 0;
 goto done;
    }
    *val = i;
    retval = 1;
  done:
    return retval;
}



static int
parse_dec64(char *str, uint8_t n, int64_t *dec64_i, char **reason)
{
    int retval = 1;
    char *s0 = ((void *)0);
    char *s1;
    char *s2;
    char ss[64];
    int len1;
    int len2 = 0;
    int i;
# 923 "cligen_var.c"
    if (n<=0 || n>18){
 if (reason != ((void *)0))
     if ((*reason = cligen_reason("%s: %d fraction-digits given but should be in interval [1:18]", __FUNCTION__, n)) == ((void *)0)){
  retval = -1;
  goto done;
     }
 retval = 0;
 goto done;
    }
    if ((s0 = strdup(str)) == ((void *)0)){
 retval = -1;
 goto done;
    }
    s2 = s0;
    s1 = strsep(&s2, ".");

    len1 = strlen(s1);
    memcpy(ss, s1, len1);






    if (s2){
 len2 = strlen(s2);
 if (len2 > n){
     if (reason != ((void *)0))
  if ((*reason = cligen_reason("%s has %d fraction-digits but may only have %d", str, len2, n)) == ((void *)0)){
      retval = -1;
      goto done;
  }
     retval = 0;
     goto done;
 }
 memcpy(ss+len1, s2, len2);
    }




    for (i=len1+len2; i<len1+n; i++)
 ss[i] = '0';
    ss[len1+n] = '\0';
    if ((retval = parse_int64(ss, dec64_i, reason)) != 1)
 goto done;
  done:
    if (s0)
 free(s0);
    return retval;
}

static int
parse_bool(char *str, char *val, char **reason)
{
    int i;
    int retval = 1;

    if (strcmp(str, "true") == 0 || strcmp(str, "on") == 0)
 i = 1;
    else
 if (strcmp(str, "false") == 0 || strcmp(str, "off") == 0)
     i = 0;
 else{
     if (reason)
  if ((*reason = cligen_reason("%s is not a boolean value", str)) == ((void *)0)){
      retval = -1;
      goto done;
  }
     retval = 0;
     goto done;
 }
    *val = i;
  done:
    return retval;
}

int
parse_ipv4addr(char *str, struct in_addr *val, char **reason)
{
    int retval = -1;

    if ((retval = inet_pton(2, str, val)) < 0)
 goto done;
    if (retval == 0 && reason)
 if ((*reason = cligen_reason("Invalid IPv4 address")) == ((void *)0))
     retval = -1;
  done:
    return retval;
}

int
parse_ipv6addr(char *str, struct in6_addr *val, char **reason)
{
    int retval = -1;

    if ((retval = inet_pton(10, str, val)) < 0)
 goto done;
    if (retval == 0 && reason)
 if ((*reason = cligen_reason("Invalid IPv6 address")) == ((void *)0))
     retval = -1;
  done:
    return retval;
}







static int
parse_macaddr(char *str, char addr[6], char **reason)
{
    char s[17 +1], *s1, *s2;
    int i=0;
    int tmp;
    int retval = -1;

    if ((str == ((void *)0)) || strlen(str) > 17){
 retval = 0;
 if (reason && (*reason = cligen_reason("%s: Invalid macaddress", str)) == ((void *)0))
     retval = -1;
 goto done;
    }
    strncpy(s, str, 17 +1);
    s2 = s;

    while ((s1 = strsep(&s2, ":")) != ((void *)0))



     {
  if (sscanf(s1, "%x", &tmp) < 1)
      return -1;
  if (tmp < 0 || 255 < tmp){
      retval = 0;
      if (reason && (*reason = cligen_reason("%s: Invalid macaddress", str)) == ((void *)0))
   retval = -1;
      goto done;
  }
  addr[i++] = (uint8_t)tmp;
     }
    if (i!=6){
 retval = 0;
 if (reason &&
     (*reason = cligen_reason("%s: Invalid macaddr", str)) == ((void *)0)){
     retval = -1;
     goto done;
 }
 goto done;
    }
    retval = 1;
  done:
    return retval;
}






static int
parse_url(char *url, cg_var *cv, char **reason)
{
    char *str0;
    char *str;
    char *tmp;
    char *tmp2;
    int retval = -1;

    if ((str0 = strdup(url)) == ((void *)0))
 goto done;
    str = str0;
    if ((tmp = strchr(str, ':')) == ((void *)0))
 goto warn;
    if (strncmp(tmp, "://", 3))
 goto warn;
    *tmp = '\0';
    cv->u.varu_url.varurl_proto = strdup(str);
    str = tmp+3;
    if ((tmp = strchr(str, '@')) != ((void *)0)){

 *tmp = '\0';
 if ((tmp2 = strchr(str, ':')) != ((void *)0)){

     *tmp2 = '\0';
     cv->u.varu_url.varurl_passwd = strdup(tmp2+1);
 }
 else
     cv->u.varu_url.varurl_passwd = strdup("");
 cv->u.varu_url.varurl_user = strdup(str);
 str = tmp+1;
    }
    else{
 cv->u.varu_url.varurl_user = strdup("");
 cv->u.varu_url.varurl_passwd = strdup("");
    }
    if ((tmp = strchr(str, '/')) != ((void *)0))
 *tmp = '\0';
    cv->u.varu_url.varurl_addr = strdup(str);
    if (tmp){
 str = tmp+1;
 cv->u.varu_url.varurl_path = strdup(str);
    }
    else
 cv->u.varu_url.varurl_path = strdup("");
    retval = 1;
  done:
    if (str0)
 free(str0);
    return retval;
  warn:
    if (reason && (*reason = cligen_reason("%s: Invalid URL", url)) == ((void *)0))
 return -1;
    if (str0)
 free(str0);
    return 0;
}

int
str2urlproto(char *str)
{
    int proto;

    for (proto = 1;; proto++){
 if (cg_urlprotostr[proto] == ((void *)0))
     break;
 if (strcmp (str, cg_urlprotostr[proto]) == 0)
     return proto;
    }
    return 0;
}


int
uuid2str(uuid_t u, char *fmt, int len)
{
    snprintf(fmt, len,
     "%02x%02x%02x%02x-" "%02x%02x-" "%02x%02x-" "%02x%02x-"
     "%02x%02x%02x%02x%02x%02x",
     u[0]&0xff, u[1]&0xff, u[2]&0xff, u[3]&0xff,
     u[4]&0xff, u[5]&0xff, u[6]&0xff, u[7]&0xff,
     u[8]&0xff, u[9]&0xff, u[10]&0xff, u[11]&0xff,
     u[12]&0xff, u[13]&0xff, u[14]&0xff, u[15]&0xff);
    return 0;
}

static int
toint(char c)
{
  if (c >= '0' && c <= '9')
      return c - '0';
  if (c >= 'A' && c <= 'F')
      return 10 + c - 'A';
  if (c >= 'a' && c <= 'f')
      return 10 + c - 'a';
  return -1;
}
# 1190 "cligen_var.c"
int
str2uuid(char *in, uuid_t u)
{
    int i = 0, j = 0, k;
    int a, b;
    int retval = -1;

    if (strlen(in) != 36)
 return -1;
    for (k=0; k<4; k++){
 a = toint(in[j++]);
 b = toint(in[j++]);
 if (a < 0 || b < 0)
     goto done;
 u[i++] = (a << 4) | b;
    }
    if (in[j++] != '-')
 goto done;
    for (k=0; k<2; k++){
 a = toint(in[j++]); b = toint(in[j++]);
 if (a < 0 || b < 0)
     goto done;
 u[i++] = (a << 4) | b;
    }
    if (in[j++] != '-')
 goto done;
    for (k=0; k<2; k++){
 a = toint(in[j++]); b = toint(in[j++]);
 if (a < 0 || b < 0)
     goto done;
 u[i++] = (a << 4) | b;
    }
    if (in[j++] != '-')
 goto done;
    for (k=0; k<2; k++){
 a = toint(in[j++]); b = toint(in[j++]);
 if (a < 0 || b < 0)
     goto done;
 u[i++] = (a << 4) | b;
    }
    if (in[j++] != '-')
 goto done;
    for (k=0; k<6; k++){
 a = toint(in[j++]); b = toint(in[j++]);
 if (a < 0 || b < 0)
     goto done;
 u[i++] = (a << 4) | b;
    }
    if (in[j] != '\0')
 goto done;
    retval = 0;
done:
    return retval;
}

static int
todig(char c)
{
  if (c >= '0' && c <= '9')
      return c - '0';
  return -1;
}
# 1260 "cligen_var.c"
static int
tonum(int n, char *s)
{
    int i, a, sum = 0, retval = -1;

    for (i=0; i<n; i++){
 if ((a = todig(s[i])) < 0)
     goto done;
 sum *= 10;
 sum += a;
    }
    retval = sum;
  done:
    return retval;
}
# 1291 "cligen_var.c"
int
str2time(char *in, struct timeval *tv)
{
    int retval = -1;
    int i = 0;
    int j;
    int len;
    int year,
 month,
 day,
 hour,
 min,
 sec,
 usec = 0;
    struct tm *tm;
    time_t t;

    if ((year = tonum(4, &in[i])) < 0)
 goto done;
    if (year < 1970 || year > 2104)
 goto done;
    i += 4;
    if (in[i++] != '-')
 goto done;
    if ((month = tonum(2, &in[i])) < 0)
 goto done;
    if (month < 1 || month > 12)
 goto done;
    i += 2;
    if (in[i++] != '-')
 goto done;
    if ((day = tonum(2, &in[i])) < 0)
 goto done;
    if (day < 1 || day > 31)
 goto done;
    i += 2;
    if (in[i] != 'T' && in[i] != ' ')
 goto done;
    i++;
    if ((hour = tonum(2, &in[i])) < 0)
 goto done;
    if (hour > 23)
 goto done;
    i += 2;
    if (in[i++] != ':')
 goto done;
    if ((min = tonum(2, &in[i])) < 0)
 goto done;
    if (min > 59)
 goto done;
    i += 2;
    if (in[i++] != ':')
 goto done;
    if ((sec = tonum(2, &in[i])) < 0)
 goto done;
    if (sec > 59)
 goto done;
    i += 2;
    if (in[i] == '\0')
 goto mkdate;
    if (in[i++] != '.')
 goto done;
    len = strlen(&in[i]);
    if (len > 6 || len < 1)
 goto done;
    if ((usec = tonum(len, &in[i])) < 0)
 goto done;
    for (j=0; j<6-len; j++)
 usec *= 10;
    if (usec > 999999)
 goto done;
    i += len;
    if (in[i] != '\0')
 goto done;
  mkdate:
    if ((tm = calloc(1, sizeof(struct tm))) == ((void *)0)){
 fprintf(stderr, "calloc: %s\n", strerror((*__errno_location ())));
 goto done;
    }
    tm->tm_year = year - 1900;
    tm->tm_mon = month - 1;
    tm->tm_mday = day;
    tm->tm_hour = hour;
    tm->tm_min = min;
    tm->tm_sec = sec;
    if ((t = mktime(tm)) < 0)
 goto done;
# 1389 "cligen_var.c"
    t = t - timezone;
# 1398 "cligen_var.c"
    free(tm);
    tv->tv_sec = t;
    tv->tv_usec = usec;
    retval = 0;
done:
    return retval;
}





int
time2str(struct timeval tv, char *fmt, int len)
{
    int retval = -1;
    struct tm *tm;

    tm = gmtime((time_t*)&tv.tv_sec);
    if (snprintf(fmt, len, "%04d-%02d-%02dT%02d:%02d:%02d.%06ld",
      tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday, tm->tm_hour,
   tm->tm_min, tm->tm_sec, tv.tv_usec) < 0)
 goto done;
    retval = 0;
done:
    return retval;
}




enum cv_type
cv_str2type(char *str)
{
    if (strcmp(str, "int8") == 0)
 return CGV_INT8;
    if (strcmp(str, "int16") == 0)
 return CGV_INT16;
    if (strcmp(str, "int32") == 0)
 return CGV_INT32;
    if (strcmp(str, "int64") == 0)
 return CGV_INT64;
    if (strcmp(str, "uint8") == 0)
 return CGV_UINT8;
    if (strcmp(str, "uint16") == 0)
 return CGV_UINT16;
    if (strcmp(str, "uint32") == 0)
 return CGV_UINT32;
    if (strcmp(str, "uint64") == 0)
 return CGV_UINT64;

   if (strcmp(str, "number") == 0 || strcmp(str, "int") == 0)
       return CGV_INT32;
   if (strcmp(str, "long") == 0)
       return CGV_INT64;

  if (strcmp(str, "decimal64") == 0)
     return CGV_DEC64;
  if (strcmp(str, "bool") == 0)
     return CGV_BOOL;
  if (strcmp(str, "string") == 0)
    return CGV_STRING;
  if (strcmp(str, "rest") == 0)
    return CGV_REST;
  if (strcmp(str, "interface") == 0)
    return CGV_INTERFACE;
  if (strcmp(str,"ipaddr") == 0 || strcmp(str,"ipv4addr") == 0)
    return CGV_IPV4ADDR;
  if (strcmp(str,"ipv4prefix") == 0)
    return CGV_IPV4PFX;
  if (strcmp(str,"ipv6addr") == 0)
    return CGV_IPV6ADDR;
  if (strcmp(str,"ipv6prefix") == 0)
    return CGV_IPV6PFX;
  if (strcmp(str,"macaddr") == 0)
      return CGV_MACADDR;
  if (strcmp(str,"url") == 0)
      return CGV_URL;
  if (strcmp(str,"uuid") == 0)
      return CGV_UUID;
  if (strcmp(str,"time") == 0)
      return CGV_TIME;
  if (strcmp(str,"void") == 0)
      return CGV_VOID;
  return CGV_ERR;
}



char *
cv_type2str(enum cv_type type)
{
    char *str = ((void *)0);
    switch (type){
    case CGV_ERR:
 str="err";
 break;
    case CGV_INT8:
 str="int8";
 break;
    case CGV_INT16:
 str="int16";
 break;
    case CGV_INT32:
 str="int32";
 break;
    case CGV_INT64:
 str="int64";
 break;
    case CGV_UINT8:
 str="uint8";
 break;
    case CGV_UINT16:
 str="uint16";
 break;
    case CGV_UINT32:
 str="uint32";
 break;
    case CGV_UINT64:
 str="uint64";
 break;
    case CGV_DEC64:
 str="decimal64";
 break;
    case CGV_BOOL:
 str="bool";
 break;
    case CGV_STRING:
 str="string";
 break;
    case CGV_REST:
 str="rest";
 break;
    case CGV_INTERFACE:
 str="interface";
 break;
    case CGV_IPV4ADDR:
 str="ipv4addr";
 break;
    case CGV_IPV4PFX:
 str="ipv4prefix";
 break;
    case CGV_IPV6ADDR:
        str="ipv6addr";
 break;
    case CGV_IPV6PFX:
 str="ipv6prefix";
 break;
    case CGV_MACADDR:
 str="macaddr";
 break;
    case CGV_URL:
 str="url";
 break;
    case CGV_UUID:
 str="uuid";
 break;
    case CGV_TIME:
 str="time";
 break;
    case CGV_VOID:
 str="void";
 break;
    default:
 fprintf(stderr, "%s: invalid type: %d\n", __FUNCTION__, type);
 break;
    }
    return str;
}







int
cv_len(cg_var *cv)
{
    int len = 0;

    switch (cv->var_type){
    case CGV_INT8:
 len = sizeof(cv->u.varu_int8);
 break;
    case CGV_INT16:
 len = sizeof(cv->u.varu_int16);
 break;
    case CGV_INT32:
 len = sizeof(cv->u.varu_int32);
 break;
    case CGV_INT64:
 len = sizeof(cv->u.varu_int64);
 break;
    case CGV_UINT8:
 len = sizeof(cv->u.varu_uint8);
 break;
    case CGV_UINT16:
 len = sizeof(cv->u.varu_uint16);
 break;
    case CGV_UINT32:
 len = sizeof(cv->u.varu_uint32);
 break;
    case CGV_UINT64:
 len = sizeof(cv->u.varu_uint64);
 break;
    case CGV_DEC64:
 len = sizeof(cv->u.varu_dec64.vardec64_i) + sizeof(cv->u.varu_dec64.vardec64_n);
 break;
    case CGV_BOOL:
 len = sizeof(cv->u.varu_bool);
 break;
    case CGV_REST:
 len = (cv->u.varu_string ? strlen(cv->u.varu_string) : 0) + 1;
 break;
    case CGV_STRING:
 len = (cv->u.varu_string ? strlen(cv->u.varu_string) : 0) + 1;
 break;
    case CGV_INTERFACE:
 len = (cv->u.varu_string ? strlen(cv->u.varu_string) : 0) + 1;
 break;
    case CGV_IPV4ADDR:
 len = sizeof(cv->u.varu_ipv4addr.varipv4_ipv4addr);
 break;
    case CGV_IPV4PFX:
 len = sizeof(cv->u.varu_ipv4addr);
 break;
    case CGV_IPV6ADDR:
 len = sizeof(cv->u.varu_ipv6addr.varipv6_ipv6addr);
 break;
    case CGV_IPV6PFX:
 len = sizeof(cv->u.varu_ipv6addr);
 break;
    case CGV_MACADDR:
 len = sizeof(cv->u.varu_macaddr);
 break;
    case CGV_URL:
 len = (cv->u.varu_url.varurl_proto ? strlen(cv->u.varu_url.varurl_proto):0) + 1 +
     (cv->u.varu_url.varurl_addr ? strlen(cv->u.varu_url.varurl_addr):0) + 1 +
     (cv->u.varu_url.varurl_path ? strlen(cv->u.varu_url.varurl_path):0) + 1 +
     (cv->u.varu_url.varurl_user ? strlen(cv->u.varu_url.varurl_user):0) + 1 +
     (cv->u.varu_url.varurl_passwd ? strlen(cv->u.varu_url.varurl_passwd):0) + 1;
 break;
    case CGV_UUID:
 len = sizeof(cv->u.varu_uuid);
 break;
    case CGV_TIME:
 len = sizeof(cv->u.varu_time);
 break;
    case CGV_VOID:
 len = sizeof(void*);
 break;
    default:
 break;
    }
    return len;
}
# 1665 "cligen_var.c"
static int
cv_dec64_print(cg_var *cv, char *s0, int *s0len)
{
    int i;
    uint8_t n = cv->u.varu_dec64.vardec64_n;;
    int len;

    ((0<n && n<19) ? (void) (0) : __assert_fail ("0<n && n<19", "cligen_var.c", 1672, __PRETTY_FUNCTION__));
    len = snprintf(s0, *s0len, "%" "l" "d", cv_dec64_i_get(cv));
    *s0len -= len;



    for (i=len; i>=len-n; i--)
 s0[i+1] = s0[i];
    (*s0len)--;
    s0[len-n] = '.';
    return 0;
}
# 1698 "cligen_var.c"
int
cv2str(cg_var *cv, char *str, size_t size)
{
    int len = 0;
    char straddr[46];

    switch (cv->var_type){
    case CGV_INT8:
 len = snprintf(str, size, "%" "d", cv->u.varu_int8);
 break;
    case CGV_INT16:
 len = snprintf(str, size, "%" "d", cv->u.varu_int16);
 break;
    case CGV_INT32:
 len = snprintf(str, size, "%" "d", cv->u.varu_int32);
 break;
    case CGV_INT64:
 len = snprintf(str, size, "%" "l" "d", cv->u.varu_int64);
 break;
    case CGV_UINT8:
 len = snprintf(str, size, "%" "u", cv->u.varu_uint8);
 break;
    case CGV_UINT16:
 len = snprintf(str, size, "%" "u", cv->u.varu_uint16);
 break;
    case CGV_UINT32:
 len = snprintf(str, size, "%" "u", cv->u.varu_uint32);
 break;
    case CGV_UINT64:
 len = snprintf(str, size, "%" "l" "u", cv->u.varu_uint64);
 break;
    case CGV_DEC64:{
 char ss[64];
 int sslen = sizeof(ss);

 cv_dec64_print(cv, ss, &sslen);
 len = snprintf(str, size, "%s", ss);
 break;
    }
    case CGV_BOOL:
 if (cv->u.varu_bool)
     len = snprintf(str, size, "true");
 else
     len = snprintf(str, size, "false");
 break;
    case CGV_REST:
 len = snprintf(str, size, "%s", cv->u.varu_string);
 break;
    case CGV_STRING:
 len = snprintf(str, size, "%s", cv->u.varu_string);
 break;
    case CGV_INTERFACE:
 len = snprintf(str, size, "%s", cv->u.varu_string);
 break;
    case CGV_IPV4ADDR:
 len = snprintf(str, size, "%s", inet_ntoa(cv->u.varu_ipv4addr.varipv4_ipv4addr));
 break;
    case CGV_IPV4PFX:
 len = snprintf(str, size, "%s/%u",
         inet_ntoa (cv->u.varu_ipv4addr.varipv4_ipv4addr),
         cv->u.varu_ipv4addr.varipv4_masklen);
 break;
    case CGV_IPV6ADDR:
 if (inet_ntop(10, &cv->u.varu_ipv6addr.varipv6_ipv6addr, straddr, sizeof(straddr)) < 0){
     fprintf(stderr, "inet_ntop: %s\n", strerror((*__errno_location ())));
     return -1;
 }
 len = snprintf(str, size, "%s", straddr);
 break;
    case CGV_IPV6PFX:
 if (inet_ntop(10, &cv->u.varu_ipv6addr.varipv6_ipv6addr, straddr, sizeof(straddr)) < 0){
     fprintf(stderr, "inet_ntop: %s\n", strerror((*__errno_location ())));
     return -1;
 }
 len = snprintf(str, size, "%s/%u", straddr, cv->u.varu_ipv6addr.varipv6_masklen);
 break;
    case CGV_MACADDR:
 len = snprintf(str, size, "%02x:%02x:%02x:%02x:%02x:%02x",
         (uint8_t)cv->u.varu_macaddr[0],
         (uint8_t)cv->u.varu_macaddr[1],
         (uint8_t)cv->u.varu_macaddr[2],
         (uint8_t)cv->u.varu_macaddr[3],
         (uint8_t)cv->u.varu_macaddr[4],
         (uint8_t)cv->u.varu_macaddr[5]);

 break;
    case CGV_URL:
 len = snprintf(str, size, "%s://%s%s%s%s%s/%s",
         cv->u.varu_url.varurl_proto,
         cv->u.varu_url.varurl_user,
         strlen(cv->u.varu_url.varurl_passwd)?":":"",
         cv->u.varu_url.varurl_passwd,
         strlen(cv->u.varu_url.varurl_user)||strlen(cv->u.varu_url.varurl_passwd)?"@":"",
         cv->u.varu_url.varurl_addr,
         cv->u.varu_url.varurl_path
     );
 break;
    case CGV_UUID:{
 char uuidstr[37];
 uuid2str(cv->u.varu_uuid, uuidstr, sizeof(uuidstr));
 len = snprintf(str, size, "%s", uuidstr);
 break;
    }
    case CGV_TIME:{
 char timestr[27];
 time2str(cv->u.varu_time, timestr, sizeof(timestr));
 len = snprintf(str, size, "%s", timestr);
 break;
    }
    case CGV_VOID:
 break;
    default:
 break;
    }
    return len;
}






char *
cv2str_dup(cg_var *cv)
{
    int len;
    char *str;

    if ((len = cv2str (cv, ((void *)0), 0)) < 0)
 return ((void *)0);
    if ((str = (char *)malloc (len+1)) == ((void *)0))
 return ((void *)0);
    memset (str, '\0', len+1);
    if ((cv2str(cv, str, len+1)) < 0){
 free(str);
 return ((void *)0);
    }
    return str;
}





int
cv_print(FILE *f, cg_var *cv)
{
    int len = 0;
    char straddr[46];

    switch (cv->var_type){
    case CGV_INT8:
 fprintf(f, "%" "d", cv->u.varu_int8);
 break;
    case CGV_INT16:
 fprintf(f, "%" "d", cv->u.varu_int16);
 break;
    case CGV_INT32:
 fprintf(f, "%" "d", cv->u.varu_int32);
 break;
    case CGV_INT64:
 fprintf(f, "%" "l" "d", cv->u.varu_int64);
 break;
    case CGV_UINT8:
 fprintf(f, "%" "u", cv->u.varu_uint8);
 break;
    case CGV_UINT16:
 fprintf(f, "%" "u", cv->u.varu_uint16);
 break;
    case CGV_UINT32:
 fprintf(f, "%" "u", cv->u.varu_uint32);
 break;
    case CGV_UINT64:
 fprintf(f, "%" "l" "u", cv->u.varu_uint64);
 break;
    case CGV_DEC64:{
 char ss[64];
 int sslen = sizeof(ss);

 cv_dec64_print(cv, ss, &sslen);
 fprintf(f, "%s", ss);
 break;
    }
    case CGV_BOOL:
 if (cv->u.varu_bool)
     fprintf(f, "true");
 else
     fprintf(f, "false");
 break;
    case CGV_REST:
 fprintf(f, "%s", cv->u.varu_string);
 break;
    case CGV_STRING:
 fprintf(f, "\"%s\"", cv->u.varu_string);
 break;
    case CGV_INTERFACE:
 fprintf(f, "\"%s\"", cv->u.varu_string);
 break;
    case CGV_IPV4ADDR:
 fprintf(f, "%s", inet_ntoa(cv->u.varu_ipv4addr.varipv4_ipv4addr));
 break;
    case CGV_IPV4PFX:
 fprintf(f, "%s/%d", inet_ntoa(cv->u.varu_ipv4addr.varipv4_ipv4addr), cv->u.varu_ipv4addr.varipv4_masklen);
 break;
    case CGV_IPV6ADDR:
 if (inet_ntop(10, &cv->u.varu_ipv6addr.varipv6_ipv6addr, straddr, sizeof(straddr)) < 0){
     fprintf(stderr, "inet_ntop: %s\n", strerror((*__errno_location ())));
     return -1;
 }
 fprintf(f, "%s", straddr);
 break;
    case CGV_IPV6PFX:
 if (inet_ntop(10, &cv->u.varu_ipv6addr.varipv6_ipv6addr, straddr, sizeof(straddr)) < 0){
     fprintf(stderr, "inet_ntop: %s\n", strerror((*__errno_location ())));
     return -1;
 }
 fprintf(f, "%s/%d", straddr, cv->u.varu_ipv4addr.varipv4_masklen);
 break;
    case CGV_MACADDR:
 fprintf(f, "%02x:%02x:%02x:%02x:%02x:%02x",
  cv->u.varu_macaddr[0],
  cv->u.varu_macaddr[1],
  cv->u.varu_macaddr[2],
  cv->u.varu_macaddr[3],
  cv->u.varu_macaddr[4],
  cv->u.varu_macaddr[5]);

 break;
    case CGV_URL:
 fprintf(f, "%s://%s%s%s%s%s/%s",
  cv->u.varu_url.varurl_proto,
  cv->u.varu_url.varurl_user,
  strlen(cv->u.varu_url.varurl_passwd)?":":"",
  cv->u.varu_url.varurl_passwd,
  strlen(cv->u.varu_url.varurl_user)||strlen(cv->u.varu_url.varurl_passwd)?"@":"",
  cv->u.varu_url.varurl_addr,
  cv->u.varu_url.varurl_path
     );
 break;
    case CGV_UUID:{
 char uuidstr[37];
 uuid2str(cv->u.varu_uuid, uuidstr, sizeof(uuidstr));
 fprintf(f, "%s", uuidstr);
 break;
    }
    case CGV_TIME:{
 char timestr[27];
 time2str(cv->u.varu_time, timestr, sizeof(timestr));
 fprintf(f, "%s", timestr);
 break;
    }
    case CGV_VOID:
 break;
    default:
 break;
    }
    return len;
}
# 1985 "cligen_var.c"
int
cv_parse1(char *str0, cg_var *cv, char **reason)
{
    int retval = -1;
    char *str;
    char *mask;
    int masklen;

    if ((str = strdup(str0)) == ((void *)0))
 goto done;
    switch (cv->var_type) {
    case CGV_INT8:
 retval = parse_int8(str, &cv->u.varu_int8, reason);
 break;
    case CGV_INT16:
 retval = parse_int16(str, &cv->u.varu_int16, reason);
 break;
    case CGV_INT32:
 retval = parse_int32(str, &cv->u.varu_int32, reason);
 break;
    case CGV_INT64:
 retval = parse_int64(str, &cv->u.varu_int64, reason);
 break;
    case CGV_UINT8:
 retval = parse_uint8(str, &cv->u.varu_uint8, reason);
 break;
    case CGV_UINT16:
 retval = parse_uint16(str, &cv->u.varu_uint16, reason);
 break;
    case CGV_UINT32:
 retval = parse_uint32(str, &cv->u.varu_uint32, reason);
 break;
    case CGV_UINT64:
 retval = parse_uint64(str, &cv->u.varu_uint64, reason);
 break;
    case CGV_DEC64:
 retval = parse_dec64(str, cv_dec64_n_get(cv), &cv->u.varu_dec64.vardec64_i, reason);
 break;
    case CGV_BOOL:
 retval = parse_bool(str, &cv->u.varu_bool, reason);
 break;
    case CGV_REST:
 if ((cv->u.varu_string = strdup(str)) == ((void *)0))
     goto done;
 retval = 1;
 break;
    case CGV_STRING:
 if ((cv->u.varu_string = strdup(str)) == ((void *)0))
     goto done;
 retval = 1;
 break;
    case CGV_INTERFACE:
 if ((cv->u.varu_string = strdup(str)) == ((void *)0))
     goto done;
 retval = 1;
 break;
    case CGV_IPV4ADDR:
 cv->u.varu_ipv4addr.varipv4_masklen = 32;
 retval = parse_ipv4addr(str, &cv->u.varu_ipv4addr.varipv4_ipv4addr, reason);
 break;
    case CGV_IPV6ADDR:
 cv->u.varu_ipv6addr.varipv6_masklen = 128;
 retval = parse_ipv6addr(str, &cv->u.varu_ipv6addr.varipv6_ipv6addr, reason);
 break;
    case CGV_IPV4PFX:
 if ((mask = strchr (str, '/')) == ((void *)0)){
     retval = 0;
     if (reason)
  if ((*reason = cligen_reason("Mask-length missing")) == ((void *)0))
      retval = -1;
     goto done;
 }
 *mask++ = '\0';
 if ((retval = parse_int32(mask, &masklen, reason)) <= 0)
     break;
 if (masklen > 32 || masklen < 0) {
     retval = 0;
     if (reason)
  if ((*reason = cligen_reason("Mask-length out of range: %s", mask)) == ((void *)0))
      retval = -1;
     goto done;
 }
 cv->u.varu_ipv4addr.varipv4_masklen = masklen;
 retval = parse_ipv4addr(str, &cv->u.varu_ipv4addr.varipv4_ipv4addr, reason);
 break;
    case CGV_IPV6PFX:
 if ((mask = strchr (str, '/')) == ((void *)0)){
     retval = 0;
     if (reason)
  if ((*reason = cligen_reason("Mask-length missing")) == ((void *)0))
      retval = -1;
     goto done;
 }
 *mask++ = '\0';
 if ((retval = parse_int32(mask, &masklen, reason)) <= 0)
     break;
 if (masklen > 128 || masklen < 0) {
     retval = 0;
     if (reason &&
  (*reason = cligen_reason("Mask-length out of range: %s", mask))==((void *)0))
  retval = -1;
     goto done;
 }
 cv->u.varu_ipv6addr.varipv6_masklen = masklen;
 retval = parse_ipv6addr(str, &cv->u.varu_ipv6addr.varipv6_ipv6addr, reason);
 break;
    case CGV_MACADDR:
 retval = parse_macaddr(str, cv->u.varu_macaddr, reason);
 break;
    case CGV_URL:
 retval = parse_url(str, cv, reason);
 break;
    case CGV_UUID:
 if (str2uuid(str, cv->u.varu_uuid) < 0){
     retval = 0;
     if (reason &&
  (*reason = cligen_reason("Invalid uuid: %s", str))==((void *)0))
  retval = -1;
     goto done;
 }
 retval = 1;
 break;
    case CGV_TIME:
 if (str2time(str, &cv->u.varu_time) < 0){
     retval = 0;
     if (reason &&
  (*reason = cligen_reason("Invalid time: %s", str))==((void *)0))
  retval = -1;
     goto done;
 }
 retval = 1;
 break;
    case CGV_VOID:
    case CGV_ERR:
 retval = 0;
 if (reason)
     *reason = cligen_reason("Invalid variable");
 break;
    }
  done:
    if (str)
 free (str);
    if (reason && *reason)
 ((retval == 0) ? (void) (0) : __assert_fail ("retval == 0", "cligen_var.c", 2128, __PRETTY_FUNCTION__));
    return retval;
}
# 2155 "cligen_var.c"
int
cv_parse(char *str, cg_var *cv)
{
    int retval;
    char *reason = ((void *)0);

    if ((retval = cv_parse1(str, cv, &reason)) < 0){
 fprintf(stderr, "cv parse error: %s\n", strerror((*__errno_location ())));
 return -1;
    }
    if (retval == 0){
 fprintf(stderr, "cv parse error: %s\n", reason);
 return -1;
    }
    return 0;
}
# 2200 "cligen_var.c"
int
cv_validate(cg_var *cv, cg_varspec *cs, char **reason)
{
    int retval = 1;
    int64_t i = 0;
    uint64_t u = 0;
    char *str;

    switch (cs->cgs_vtype){
    case CGV_INT8:
 if (cs->cgs_range){
     i = cv_int8_get(cv);
     if ((cs->cgs_rangecv_low && ((i) < cv_int8_get(cs->cgs_rangecv_low))) || (cs->cgs_rangecv_high && ((i) > cv_int8_get(cs->cgs_rangecv_high)))){
      if (reason)
   *reason = cligen_reason("Number out of range: %ld", i);
      retval = 0;
  }
 }
 break;
    case CGV_INT16:
 if (cs->cgs_range){
     i = cv_int16_get(cv);
     if ((cs->cgs_rangecv_low && ((i) < cv_int16_get(cs->cgs_rangecv_low))) || (cs->cgs_rangecv_high && ((i) > cv_int16_get(cs->cgs_rangecv_high)))){
      if (reason)
   *reason = cligen_reason("Number out of range: %ld", i);
      retval = 0;
  }
 }
 break;
    case CGV_INT32:
 if (cs->cgs_range){
     i = cv_int32_get(cv);
     if ((cs->cgs_rangecv_low && ((i) < cv_int32_get(cs->cgs_rangecv_low))) || (cs->cgs_rangecv_high && ((i) > cv_int32_get(cs->cgs_rangecv_high)))){
      if (reason)
   *reason = cligen_reason("Number out of range: %ld", i);
      retval = 0;
  }
 }
 break;
    case CGV_INT64:
 if (cs->cgs_range){
     i = cv_int64_get(cv);
     if ((cs->cgs_rangecv_low && ((i) < cv_int64_get(cs->cgs_rangecv_low))) || (cs->cgs_rangecv_high && ((i) > cv_int64_get(cs->cgs_rangecv_high)))){
      if (reason)
   *reason = cligen_reason("Number out of range: %ld", i);
      retval = 0;
  }
 }
 break;
    case CGV_UINT8:
 if (cs->cgs_range){
     u = cv_uint8_get(cv);
     if ((cs->cgs_rangecv_low && ((i) < cv_uint8_get(cs->cgs_rangecv_low))) || (cs->cgs_rangecv_high && ((i) > cv_uint8_get(cs->cgs_rangecv_high)))){
      if (reason)
   *reason = cligen_reason("Number out of range: %lu", u);
      retval = 0;
  }
 }
 break;
    case CGV_UINT16:
 if (cs->cgs_range){
     u = cv_uint16_get(cv);
     if ((cs->cgs_rangecv_low && ((i) < cv_uint16_get(cs->cgs_rangecv_low))) || (cs->cgs_rangecv_high && ((i) > cv_uint16_get(cs->cgs_rangecv_high)))){
      if (reason)
   *reason = cligen_reason("Number out of range: %lu", u);
      retval = 0;
  }
 }
 break;
    case CGV_UINT32:
 if (cs->cgs_range){
     u = cv_uint32_get(cv);
     if ((cs->cgs_rangecv_low && ((i) < cv_uint32_get(cs->cgs_rangecv_low))) || (cs->cgs_rangecv_high && ((i) > cv_uint32_get(cs->cgs_rangecv_high)))){
      if (reason)
   *reason = cligen_reason("Number out of range: %lu", u);
      retval = 0;
  }
 }
 break;
    case CGV_UINT64:
 if (cs->cgs_range){
     u = cv_uint64_get(cv);
     if ((cs->cgs_rangecv_low && ((i) < cv_uint64_get(cs->cgs_rangecv_low))) || (cs->cgs_rangecv_high && ((i) > cv_uint64_get(cs->cgs_rangecv_high)))){
      if (reason)
   *reason = cligen_reason("Number out of range: %lu", u);
      retval = 0;
  }
 }
 break;
    case CGV_DEC64:
 if (cv_dec64_n_get(cv) != cs->cgs_dec64_n){
     if (reason)
  *reason = cligen_reason("Decimal 64 fraction-bits mismatch %d(cv) != %d(spec)",
     cv->u.varu_dec64.vardec64_n, cs->cgs_dec64_n);
     retval = 0;
 }




 break;
    case CGV_STRING:
 str = cv_string_get(cv);
 if (cs->cgs_range){
     u = strlen(str);
     if ((cs->cgs_rangecv_low && ((u) < cv_uint64_get(cs->cgs_rangecv_low))) || (cs->cgs_rangecv_high && ((u) > cv_uint64_get(cs->cgs_rangecv_high)))){
      if (reason)
   *reason = cligen_reason("Number out of range: %lu", u);
      retval = 0;
  }
 }
# 2323 "cligen_var.c"
 if (cs->cgs_regex != ((void *)0)){
     if ((retval = match_regexp(cv_string_get(cv), cs->cgs_regex)) < 0)
  break;
     if (retval == 0){
  if (reason)
      *reason = cligen_reason("regexp match fail: %s does not match %s",
         cv_string_get(cv), cs->cgs_regex);
  retval = 0;
  break;
     }
 }
 break;
    case CGV_ERR:
    case CGV_VOID:
 retval = 0;
 if (reason)
     *reason = cligen_reason("Invalid cv");
 retval = 0;
 break;
    case CGV_BOOL:
    case CGV_INTERFACE:
    case CGV_REST:
    case CGV_IPV4ADDR:
    case CGV_IPV6ADDR:
    case CGV_IPV4PFX:
    case CGV_IPV6PFX:
    case CGV_MACADDR:
    case CGV_URL:
    case CGV_UUID:
    case CGV_TIME:
 break;
    }
    if (reason && *reason)
 ((retval == 0) ? (void) (0) : __assert_fail ("retval == 0", "cligen_var.c", 2356, __PRETTY_FUNCTION__));
    return retval;
}






int
cv_cmp(cg_var *cgv1, cg_var *cgv2)
{
    int n;


    if(cgv1->var_type != cgv2->var_type)
 return cgv1->var_type - cgv2->var_type;

    switch (cgv1->var_type) {
    case CGV_ERR:
 return 0;
    case CGV_INT8:
 return (cgv1->u.varu_int8 - cgv2->u.varu_int8);
    case CGV_INT16:
 return (cgv1->u.varu_int16 - cgv2->u.varu_int16);
    case CGV_INT32:
 return (cgv1->u.varu_int32 - cgv2->u.varu_int32);
    case CGV_INT64:
 return (cgv1->u.varu_int64 - cgv2->u.varu_int64);
    case CGV_UINT8:
 return (cgv1->u.varu_uint8 - cgv2->u.varu_uint8);
    case CGV_UINT16:
 return (cgv1->u.varu_uint16 - cgv2->u.varu_uint16);
    case CGV_UINT32:
 return (cgv1->u.varu_uint32 - cgv2->u.varu_uint32);
    case CGV_UINT64:
 return (cgv1->u.varu_uint64 - cgv2->u.varu_uint64);
    case CGV_DEC64:
 return (cv_dec64_i_get(cgv1) - cv_dec64_i_get(cgv2) &&
  cv_dec64_n_get(cgv1) - cv_dec64_n_get(cgv2));
    case CGV_BOOL:
 return (cgv1->u.varu_bool - cgv2->u.varu_bool);
    case CGV_REST:
    case CGV_STRING:
    case CGV_INTERFACE:
 return strcmp(cgv1->u.varu_string, cgv2->u.varu_string);
    case CGV_IPV4ADDR:
 return memcmp(&cgv1->u.varu_ipv4addr.varipv4_ipv4addr, &cgv2->u.varu_ipv4addr.varipv4_ipv4addr,
        sizeof(cgv1->u.varu_ipv4addr.varipv4_ipv4addr));
    case CGV_IPV4PFX:
 if ((n = memcmp(&cgv1->u.varu_ipv4addr.varipv4_ipv4addr, &cgv2->u.varu_ipv4addr.varipv4_ipv4addr,
    sizeof(cgv1->u.varu_ipv4addr.varipv4_ipv4addr))))
     return n;
 return cgv1->u.varu_ipv4addr.varipv4_masklen - cgv2->u.varu_ipv4addr.varipv4_masklen;
    case CGV_IPV6ADDR:
 return memcmp(&cgv1->u.varu_ipv6addr.varipv6_ipv6addr, &cgv2->u.varu_ipv6addr.varipv6_ipv6addr,
        sizeof (cgv1->u.varu_ipv6addr.varipv6_ipv6addr));
    case CGV_IPV6PFX:
 if ((n = memcmp(&cgv1->u.varu_ipv6addr.varipv6_ipv6addr, &cgv2->u.varu_ipv6addr.varipv6_ipv6addr,
    sizeof(cgv1->u.varu_ipv6addr.varipv6_ipv6addr))))
     return n;
 return cgv1->u.varu_ipv6addr.varipv6_masklen - cgv2->u.varu_ipv6addr.varipv6_masklen;
    case CGV_MACADDR:
 return memcmp(&cgv1->u.varu_macaddr, &cgv2->u.varu_macaddr,
        sizeof (cgv1->u.varu_macaddr));

    case CGV_URL:
 if ((n = strcmp(cgv1->u.varu_url.varurl_proto, cgv2->u.varu_url.varurl_proto)))
     return n;
 if ((n = strcmp(cgv1->u.varu_url.varurl_addr, cgv2->u.varu_url.varurl_addr)))
     return n;
 if ((n = strcmp(cgv1->u.varu_url.varurl_path, cgv2->u.varu_url.varurl_path)))
     return n;
 if ((n = strcmp(cgv1->u.varu_url.varurl_user, cgv2->u.varu_url.varurl_user)))
     return n;
 return strcmp(cgv1->u.varu_url.varurl_passwd, cgv2->u.varu_url.varurl_passwd);
    case CGV_UUID:
 return memcmp(cgv1->u.varu_uuid, cgv2->u.varu_uuid, 16);
    case CGV_TIME:
 return memcmp(&cgv1->u.varu_time, &cgv2->u.varu_time, sizeof(struct timeval));
    case CGV_VOID:
 return (cgv1->u.varu_string == cgv2->u.varu_string);
    }

    return -1;
}
# 2452 "cligen_var.c"
int
cv_cp(cg_var *new, cg_var *old)
{
    int retval = -1;

    memcpy(new, old, sizeof(*old));
    if (old->var_name)
 if ((new->var_name = strdup(old->var_name)) == ((void *)0))
     goto done;
    switch (new->var_type) {
    case CGV_ERR:
 break;
    case CGV_INT8:
    case CGV_INT16:
    case CGV_INT32:
    case CGV_INT64:
    case CGV_UINT8:
    case CGV_UINT16:
    case CGV_UINT32:
    case CGV_UINT64:
    case CGV_DEC64:
    case CGV_BOOL:
 break;
    case CGV_REST:
    case CGV_STRING:
    case CGV_INTERFACE:
 if (old->u.varu_string)
     if ((new->u.varu_string = strdup(old->u.varu_string)) == ((void *)0))
  goto done;
 break;
    case CGV_IPV4ADDR:
    case CGV_IPV4PFX:
    case CGV_IPV6ADDR:
    case CGV_IPV6PFX:
    case CGV_MACADDR:
 break;
    case CGV_URL:
 if (old->u.varu_url.varurl_proto)
     if ((new->u.varu_url.varurl_proto = strdup(old->u.varu_url.varurl_proto)) == ((void *)0))
  goto done;
 if (old->u.varu_url.varurl_addr)
     if ((new->u.varu_url.varurl_addr = strdup(old->u.varu_url.varurl_addr)) == ((void *)0))
  goto done;
 if (old->u.varu_url.varurl_path)
     if ((new->u.varu_url.varurl_path = strdup(old->u.varu_url.varurl_path)) == ((void *)0))
  goto done;
 if (old->u.varu_url.varurl_user)
     if ((new->u.varu_url.varurl_user = strdup(old->u.varu_url.varurl_user)) == ((void *)0))
  goto done;
 if (old->u.varu_url.varurl_passwd)
     if ((new->u.varu_url.varurl_passwd = strdup(old->u.varu_url.varurl_passwd)) == ((void *)0))
  goto done;
 break;
    case CGV_UUID:
 break;
    case CGV_TIME:
 break;
    case CGV_VOID:
 new->u.varu_string = old->u.varu_string;
    }
    retval = 0;
  done:
    return retval;
}






cg_var *
cv_dup(cg_var *old)
{
    cg_var *new;

    if ((new = cv_new(old->var_type)) == ((void *)0))
 return ((void *)0);
    if (cv_cp(new, old) < 0){
 cv_free(new);
 return ((void *)0);
    }
    return new;
}
# 2544 "cligen_var.c"
cg_var *
cv_new(enum cv_type type)
{
    cg_var *cv;

    if ((cv = malloc(sizeof(cg_var))) == ((void *)0))
 goto done;
    memset(cv, 0, sizeof(*cv));
    cv->var_type = type;
  done:
    return cv;

}






int
cv_reset(cg_var *cgv)
{
    enum cv_type type = cgv->var_type;

    if (cgv->var_name)
 free(cgv->var_name);
    switch (cgv->var_type) {
    case CGV_REST:
    case CGV_STRING:
    case CGV_INTERFACE:
 free(cgv->u.varu_string);
 break;
    case CGV_URL:
 free(cgv->u.varu_url.varurl_proto);
 free(cgv->u.varu_url.varurl_addr);
 free(cgv->u.varu_url.varurl_path);
 free(cgv->u.varu_url.varurl_user);
 free(cgv->u.varu_url.varurl_passwd);
 break;
    case CGV_VOID:
 break;
    default:
 break;
    }
    memset(cgv, 0, sizeof(*cgv));
    cgv->var_type = type;
    return 0;
}





int
cv_free(cg_var *cv)
{
    cv_reset(cv);
    free(cv);
    return 0;
}
# 2616 "cligen_var.c"
int
match_regexp(char *string, char *pattern0)
{

    char pattern[1024];
    int status;
    regex_t re;
    char errbuf[1024];
    int len0;

    len0 = strlen(pattern0);
    if (len0 > sizeof(pattern)-3){
 fprintf(stderr, "pattern too long\n");
 return -1;
    }
    pattern[0] = '^';
    strncpy(pattern+1, pattern0, sizeof(pattern)-1);
    strncat(pattern, "$", sizeof(pattern)-len0-1);
    if (regcomp(&re, pattern, (((1 << 1) << 1) << 1)|1) != 0)
 return(0);
    status = regexec(&re, string, (size_t) 0, ((void *)0), 0);
    regfree(&re);
    if (status != 0) {
 regerror(status, &re, errbuf, sizeof(errbuf));
 return(0);
    }

    return(1);
}
