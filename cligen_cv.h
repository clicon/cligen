/*
  ***** BEGIN LICENSE BLOCK *****
 
  Copyright (C) 2001-2022 Olof Hagsand

  This file is part of CLIgen.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  Alternatively, the contents of this file may be used under the terms of
  the GNU General Public License Version 2 or later (the "GPL"),
  in which case the provisions of the GPL are applicable instead
  of those above. If you wish to allow use of your version of this file only
  under the terms of the GPL, and not to allow others to
  use your version of this file under the terms of Apache License version 2, indicate
  your decision by deleting the provisions above and replace them with the 
  notice and other provisions required by the GPL. If you do not delete
  the provisions above, a recipient may use your version of this file under
  the terms of any one of the Apache License version 2 or the GPL.

  ***** END LICENSE BLOCK *****


  CLIgen variables - cgv
  cgv:s are created when parsing an input string as instances of cg_obj variable 
  when matching.
  Note that a cg_obj is a syntax object and contains a part that specifies cgv:s called cov
*/

#ifndef _CLIGEN_CV_H_
#define _CLIGEN_CV_H_

#include <stdint.h> /* int64 requires stdint */

/*
 * Macros and constants
 * CLIgen flags defined are in the range 0x01 -0x0f
 * An application can use any flags above that
 */
#define V_INVERT  0x01  /* Used by regexp code as inverted regexps */

typedef unsigned char uuid_t[16];

/*
 * First, built in types,
 * Some types have their values in-line (eg in the cgv struct), others
 * have pointers to the value.
 * Cgvs with pointers are: string, interface, rest, choice, 
 * expand.
 */
enum cv_type{
  CGV_ERR=0,     /* Invalid */
  CGV_INT8,      /* 8-bit signed integer / char */
  CGV_INT16,     /* 16-bit signed integer */
  CGV_INT32,     /* 32-bit signed integer */
  CGV_INT64,     /* 32-bit signed integer */
  CGV_UINT8,     /* 8-bit unsigned integer / char */
  CGV_UINT16,    /* 16-bit unsigned integer */
  CGV_UINT32,    /* 32-bit unsigned integer */
  CGV_UINT64,    /* 32-bit unsigned integer */
  CGV_DEC64,     /* 64-bit signed decimal number */
  CGV_BOOL,      /* 1-bit boolean value */
  CGV_REST,      /* Rest of line, not parsed */
  CGV_STRING,    /* Null-terminated character string */
  CGV_INTERFACE, /* name of interface CISCO style: eg eth0, eth0/24 */
  CGV_IPV4ADDR,  /* Address: 1.2.3.4 */
  CGV_IPV4PFX,   /* Prefix: 1.2.3.4/34 */
  CGV_IPV6ADDR,  /* Address: 2001:0db8:85a3:0042:0000:8a2e:0370:7334 */
  CGV_IPV6PFX,   /* Prefix: 2001:0db8:85a3:0042:0000:8a2e:0370:7334/48 */
  CGV_MACADDR,   /* f0:de:f1:1b:10:47 */
  CGV_URL,       /* <proto>://[<user>[:<passwd>]@]<addr>[/<path>] */
  CGV_UUID,      /* Universally Unique Identifier: 550e8400-e29b-41d4-a716-446655440000 */
  CGV_TIME,      /* ISO 8601 date+timestamp: 2008-09-21T18:57:21.003 (extended format) */
  CGV_VOID,      /* Pointer to external data. Notes: not freed on cv_free; 
                    not null-terminated string, cv_cp/dup will retain pointer */
  CGV_EMPTY,     /* A type without a value */
};

/* cv is one of the int-types */
#define cv_isint(t)((t)==CGV_INT8   || (t)==CGV_INT16|| \
                    (t)==CGV_INT32  || (t)==CGV_INT64|| \
                    (t)==CGV_UINT8  || (t)==CGV_UINT16|| \
                    (t)==CGV_UINT32 || (t)==CGV_UINT64)

/* No pointers to value */
#define cv_inline(t)((t)==CGV_ERR      || cv_isint(t)|| \
                     (t)==CGV_DEC64    || (t)==CGV_BOOL|| \
                     (t)==CGV_IPV4ADDR || (t)==CGV_IPV4PFX|| \
                     (t)==CGV_IPV6ADDR || (t)==CGV_IPV6PFX|| \
                     (t)==CGV_MACADDR  || (t)==CGV_UUID||    \
                     (t)==CGV_TIME     || (t)==CGV_EMPTY)

/* var_string is set to something meaningful */
#define cv_isstring(t)((t)==CGV_STRING||(t)==CGV_REST|| \
                        (t)==CGV_INTERFACE)

#define cv_typemax(t) 

/*
 * Cligen Variable structure
 * cg_var / cv
 * A cg_var is a variable instantiation, ie it has a name and a value and is typed.
 * The type determines how the values are stored. Some values are stored in-line
 * using a union structure (such as int) while others uses pointers (eg string).
 * It is different from cg_obj of type CO_VARIABLE in that the cg_obj is a 
 * specification. 
 */
typedef struct cg_var cg_var;

struct cg_obj;     /* forward declaration. Original in cligen_object.h */

struct cg_varspec; /* forward declaration. Original in cligen_object.h */

/*
 * Prototypes
 */
char *cv_name_get(cg_var *cv);
char *cv_name_set(cg_var *cv, const char *s0);
enum cv_type cv_type_get(cg_var *cv);
enum cv_type cv_type_set(cg_var *cv, enum cv_type x);
char cv_const_get(cg_var *cv);
char cv_const_set(cg_var *cv, int c);

char cv_flag(cg_var *cv, char mask);
char cv_flag_clr(cg_var *cv, char mask);
char cv_flag_set(cg_var *cv, char mask);

char cv_bool_get(cg_var *cv);
char cv_bool_set(cg_var *cv, char x);
int8_t  cv_int8_get(cg_var *cv);
int8_t  cv_int8_set(cg_var *cv, int8_t x);
int16_t cv_int16_get(cg_var *cv);
int16_t cv_int16_set(cg_var *cv, int16_t x);
int32_t cv_int32_get(cg_var *cv);
int32_t cv_int32_set(cg_var *cv, int32_t x);
int64_t cv_int64_get(cg_var *cv);
int64_t cv_int64_set(cg_var *cv, int64_t x);

uint8_t  cv_uint8_get(cg_var *cv);
uint8_t  cv_uint8_set(cg_var *cv, uint8_t x);
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

char   *cv_string_get(cg_var *cv);
char   *cv_string_set(cg_var *cv, const char *s0);
int     cv_string_set_direct(cg_var *cv, char *s);
char   *cv_strncpy(cg_var *cv, char *s0, size_t n);
struct in_addr *cv_ipv4addr_get(cg_var *cv);
struct in_addr *cv_ipv4addr_set(cg_var *cv, struct in_addr *addr);
uint8_t cv_ipv4masklen_get(cg_var *cv);
uint8_t cv_ipv4masklen_set(cg_var *cv, uint8_t masklen);
struct in6_addr *cv_ipv6addr_get(cg_var *cv);
uint8_t cv_ipv6masklen_get(cg_var *cv);
char *cv_mac_get(cg_var *cv);
unsigned char *cv_uuid_get(cg_var *cv);
unsigned char *cv_uuid_set(cg_var *cv, unsigned char *u);
struct timeval cv_time_get(cg_var *cv);
struct timeval cv_time_set(cg_var *cv, struct timeval t);
void *cv_void_get(cg_var *cv);
int   cv_void_set(cg_var *cv, void *p);
char *cv_urlproto_get(cg_var *cv);
char *cv_urlproto_set(cg_var *cv, const char *s0);
char *cv_urladdr_get(cg_var *cv);
char *cv_urladdr_set(cg_var *cv, const char *s0);
char *cv_urlpath_get(cg_var *cv);
char *cv_urlpath_set(cg_var *cv, const char *s0);
char *cv_urluser_get(cg_var *cv);
char *cv_urluser_set(cg_var *cv, const char *s0);
char *cv_urlpasswd_get(cg_var *cv);
char *cv_urlpasswd_set(cg_var *cv, const char *s0);

int parse_int8(char *str, int8_t *val, char **reason);
int parse_int16(char *str, int16_t *val, char **reason);
int parse_int32(char *str, int32_t *val, char **reason);
int parse_int64(char *str, int64_t *val, char **reason);
int parse_uint8(char *str, uint8_t *val, char **reason);
int parse_uint16(char *str, uint16_t *val, char **reason);
int parse_uint32(char *str, uint32_t *val, char **reason);
int parse_uint64(char *str, uint64_t *val, char **reason);
int parse_dec64(char *str, uint8_t n, int64_t *dec64_i, char **reason);
int parse_bool(char *str, uint8_t *val, char **reason);

int str2urlproto(const char *str);
int str2uuid(const char *in, uuid_t u);
int uuid2str(uuid_t u, char *in, int len);
int cligen_tonum(int n, const char *s);
int str2time(const char *in, struct timeval *tv);
int time2str(struct timeval tv, char *fmt, int len);

enum cv_type cv_str2type(const char *str);
const char   *cv_type2str(enum cv_type type);
size_t  cv_len(cg_var *cgv);
int     cv2cbuf(cg_var *cv, cbuf *cb);
int     cv2str(cg_var *cv, char *str, size_t size);
char   *cv2str_dup(cg_var *cv);

int     cv_print(FILE *f, cg_var *cgv);
int     cvtype_max2str(enum cv_type type, char *str, size_t size);
char   *cvtype_max2str_dup(enum cv_type type);

int     cv_max_set(cg_var *cv);
int     cv_min_set(cg_var *cv);

int     cv_cmp(cg_var *cgv1, cg_var *cgv2);
int     cv_cp(cg_var *n, cg_var *old);
cg_var *cv_dup(cg_var *old);
int     cv_parse(const char *str, cg_var *cgv); 
int     cv_parse1(const char *str, cg_var *cgv, char **reason); /* better err-handling */

int     cv_validate(cligen_handle h, cg_var *cv, struct cg_varspec *cs, char *cmd, char **reason);
int     cv_reset(cg_var *cgv); /* not free cgv itself */ /* XXX: free_only */
int     cv_free(cg_var *cv);   /* free cgv itself */
cg_var *cv_new(enum cv_type type);

size_t  cv_size(cg_var *cv);

#endif /* _CLIGEN_CV_H_ */

