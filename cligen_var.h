/*
  CVS Version: $Id: cligen_var.h,v 1.24 2013/04/16 17:11:58 olof Exp $ 

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

  CLIgen variables - cgv
  cgv:s are created when parsing an input string as instances of cg_obj variable 
  when matching.
  Note that a cg_obj is a syntax object and contains a part that specifies cgv:s called cov
*/

#ifndef _CLIGEN_VAR_H_
#define _CLIGEN_VAR_H_

#include <stdint.h> /* int64 requires stdint */

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
  CGV_INT,       /* 32-bit signed integer */
  CGV_LONG,      /* 64-bit signed integer */
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
  CGV_VOID,      /* Pointer to external data */
};

/* No pointers to value */
#define cv_inline(t)((t)==CGV_ERR||(t)==CGV_INT|| \
                      (t)==CGV_LONG|| (t)==CGV_BOOL|| \
                      (t)==CGV_IPV4ADDR||(t)==CGV_IPV4PFX|| \
                      (t)==CGV_IPV6ADDR||(t)==CGV_IPV6PFX|| \
		      (t)==CGV_MACADDR ||(t)==CGV_UUID||(t)==CGV_TIME )

/* var_string is set to something meaningful */
#define cv_isstring(t)((t)==CGV_STRING||(t)==CGV_REST|| \
			(t)==CGV_INTERFACE)

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

struct cg_obj;     /* forward declaration. Original in cligen_gen.h */

struct cg_varspec; /* forward declaration. Original in cligen_gen.h */

/*
 * Prototypes
 */
char *cv_name_get(cg_var *cv);
char *cv_name_set(cg_var *cv, char *s0);
enum cv_type cv_type_get(cg_var *cv);
enum cv_type cv_type_set(cg_var *cv, enum cv_type x);
char cv_const_get(cg_var *cv);
char cv_const_set(cg_var *cv, int c);
char cv_index_get(cg_var *cv);
char cv_index_set(cg_var *cv, char x);
void *cv_value_get(cg_var *cv);

int32_t cv_int_get(cg_var *cv);
int32_t cv_int_set(cg_var *cv, int32_t x);
int64_t cv_long_get(cg_var *cv);
int64_t cv_long_set(cg_var *cv, int64_t x);
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

//int check_int64(char *str, int64_t *val);
int parse_int64(char *str, int64_t *val, char **reason);
int str2urlproto(char *str);
int str2uuid(char *in, uuid_t u);
int uuid2str(uuid_t u, char *in, int len);
int str2time(char *in, struct timeval *tv);
int time2str(struct timeval tv, char *fmt, int len);

enum cv_type cv_str2type(char *str);
char   *cv_type2str(enum cv_type type);
int     cv_len(cg_var *cgv);
int     cv2str(cg_var *cv, char *str, size_t size);
char   *cv2str_dup(cg_var *cv);

int     cv_print(FILE *f, cg_var *cgv);

int     cv_cmp(cg_var *cgv1, cg_var *cgv2);
int     cv_cp(cg_var *new, cg_var *old);
cg_var *cv_dup(cg_var *old);
int     cv_parse(char *str, cg_var *cgv); 
int     cv_parse1(char *str, cg_var *cgv, char **reason); /* better err-handling */

int     cv_validate(cg_var *cv, struct cg_varspec *cs, char **reason);
int     cv_reset(cg_var *cgv); /* not free cgv itself */ /* XXX: free_only */
int     cv_free(cg_var *cv);   /* free cgv itself */
cg_var *cv_new(enum cv_type type);
int     match_regexp(char *string, char *pattern0);


#endif /* _CLIGEN_VAR_H_ */

