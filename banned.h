/*
 * banned.h - Prohibit use of unsafe C functions at compile time.
 *
 * Include this header LAST in each .c file to catch unsafe function usage.
 * Generated files (cligen_parse.tab.c, lex.cligen_parse.c) are excluded.
 *
 * Use safe alternatives:
 *   strcpy  -> memcpy (when length is known) or strlcpy
 *   strcat  -> strlcat or manual append with explicit sizes
 *   sprintf -> snprintf
 *   vsprintf -> vsnprintf
 *   gets    -> fgets
 *   system  -> fork + execv
 */

#ifdef __GNUC__

#undef strcpy
#define strcpy(d, s)   _Pragma("GCC error \"strcpy is banned: use memcpy or strlcpy\"")

#undef strcat
#define strcat(d, s)   _Pragma("GCC error \"strcat is banned: use strlcat or manual append with snprintf\"")

#undef sprintf
#define sprintf(...)   _Pragma("GCC error \"sprintf is banned: use snprintf\"")

#undef vsprintf
#define vsprintf(...)  _Pragma("GCC error \"vsprintf is banned: use vsnprintf\"")

#undef gets
#define gets(s)        _Pragma("GCC error \"gets is banned: use fgets\"")

#undef system
#define system(s)      _Pragma("GCC error \"system is banned: use fork + execv\"")

#endif /* __GNUC__ */
