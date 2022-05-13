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
#include "cligen_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <inttypes.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <time.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <string.h>
#include <errno.h>

#include "cligen_buf.h"
#include "cligen_cv.h"
#include "cligen_cvec.h"
#include "cligen_parsetree.h"
#include "cligen_callback.h"
#include "cligen_object.h"
#include "cligen_io.h"
#include "cligen_regex.h"
#include "cligen_getline.h"

#include "cligen_cv_internal.h"
/*
 * URL protocol strings
 */
static char *cg_urlprotostr[] = {
    NULL,
    "file",
    "flash",
    "tftp",
    "ftp",
    "telnet",
    "http",
    "ssh",
    NULL
};

/*! Get name of cligen variable cv
 * @param[in] cv     CLIgen variable
 * @retval    name   Name of cv
 */
char *
cv_name_get(cg_var *cv)
{
    if (cv == NULL) 
	return 0;
    return cv->var_name;
}

/*! Set new CLIgen varable name. 
 * Free previous string if existing.
 * @param[in] cv     CLIgen variable
 * @param[in] s0     New name
 * @retval    s0     Return the new name
 * @retval    NULL   Error
 */
char *
cv_name_set(cg_var *cv, 
	    char   *s0)
{
    char *s1 = NULL;

    if (cv == NULL) 
	return 0;

    /* Duplicate s0. Must be done before a free, in case s0 is part of the original */
    if (s0){
	if ((s1 = strdup(s0)) == NULL)
	    return NULL; /* error in errno */
    }
    if (cv->var_name != NULL)
	free(cv->var_name);
    cv->var_name = s1;
    return s1; 
}

/*! Get cv type
 * @param[in] cv     CLIgen variable
 * @retval    type   Type of cv
 */
enum cv_type
cv_type_get(cg_var *cv)
{
    if (cv == NULL) 
	return 0;
    return cv->var_type;
}

/*! Set new CLIgen type. 
 * @param[in] cv     CLIgen variable
 * @param[in] t      New type
 * @retval    t      New type
 */
enum cv_type
cv_type_set(cg_var      *cv, 
	    enum cv_type x)
{
    if (cv == NULL) 
	return 0;
    return (cv->var_type = x);
}

/*! Get cv const flag
 * @param[in] cv     CLIgen variable
 * @retval    type   Type of cv
 */
char
cv_const_get(cg_var *cv)
{
    if (cv == NULL) 
	return 0;
    return cv->var_const;
}

/*! Set CLIgen const flag. 
 * @param[in] cv     CLIgen variable
 * @param[in] t      New type
 * @retval    t      New type
 */
char
cv_const_set(cg_var *cv, 
	     int     c)
{
    if (cv == NULL)
	return 0;
    return (cv->var_const = c);
}

/*! Get application-specific cv flag
 * @param[in] cv     CLIgen variable
 * @see cv_flag_set
 */
char
cv_flag(cg_var *cv, 
	char    mask)
{
    if (cv == NULL)
	return 0;
    return cv->var_flag & mask;
}

/*! Clear application-specific cv flag 
 * @param[in] cv     CLIgen variable
 */
char
cv_flag_clr(cg_var *cv, 
	    char    mask)
{
    if (cv == NULL)
	return 0;
    return cv->var_flag ^= mask;
}

/*! Set application-specific cv flag 
 * @param[in] cv     CLIgen variable
 * @see cv_flag
 */
char
cv_flag_set(cg_var *cv, 
	    char    mask)
{
    if (cv == NULL) 
	return 0;
    return cv->var_flag |= mask;
}

/*! Get boolean value of cv
 * @param[in] cv     CLIgen variable
 */
char
cv_bool_get(cg_var *cv)
{
    if (cv == NULL) 
	return 0;
    return ((cv)->u.varu_bool);
}

/*! Set boolean value of cv
 * @param[in] cv     CLIgen variable
 */
char
cv_bool_set(cg_var *cv, 
	    char    x)
{
    if (cv == NULL) 
	return 0;
    return (cv->u.varu_bool = x);
}

/*! Get 8-bit integer value of cv
 * @param[in] cv     CLIgen variable
 */
int8_t
cv_int8_get(cg_var *cv)
{
    if (cv == NULL) 
	return 0;
    return ((cv)->u.varu_int8);
}

/*! Set 8-bit integer value of cv
 * @param[in] cv     CLIgen variable
 */
int8_t
cv_int8_set(cg_var *cv, 
	    int8_t  x)
{
    if (cv == NULL) 
	return 0;
    return (cv->u.varu_int8 = x);
}

/*! Get 16-bit integer value of cv
 * @param[in] cv     CLIgen variable
 */
int16_t
cv_int16_get(cg_var *cv)
{
    if (cv == NULL) 
	return 0;
    return ((cv)->u.varu_int16);
}

/*! Set 16-bit integer value of cv
 * @param[in] cv     CLIgen variable
 */
int16_t
cv_int16_set(cg_var *cv, 
	     int16_t x)
{
    if (cv == NULL) 
	return 0;
    return (cv->u.varu_int16 = x);
}

/*! Get 32-bit integer value of cv
 * @param[in] cv     CLIgen variable
 */
int32_t
cv_int32_get(cg_var *cv)
{
    if (cv == NULL) 
	return 0;
    return ((cv)->u.varu_int32);
}

/*! Set 32-bit integer value of cv
 * @param[in] cv     CLIgen variable
 */
int32_t
cv_int32_set(cg_var *cv, 
	     int32_t x)
{
    if (cv == NULL) 
	return 0;
    return (cv->u.varu_int32 = x);
}

/*! Get 64-bit integer value of cv
 * @param[in] cv     CLIgen variable
 */
int64_t
cv_int64_get(cg_var *cv)
{
    if (cv == NULL) 
	return 0;
    return ((cv)->u.varu_int64);
}

/*! Set 64-bit integer value of cv
 * @param[in] cv     CLIgen variable
 */
int64_t
cv_int64_set(cg_var *cv, 
             int64_t x)
{
    if (cv == NULL) 
	return 0;
    return (cv->u.varu_int64 = x);
}

/*! Get 8-bit unsigned integer value of cv
 * @param[in] cv     CLIgen variable
 */
uint8_t
cv_uint8_get(cg_var *cv)
{
    if (cv == NULL) 
	return 0;
    return ((cv)->u.varu_uint8);
}

/*! Set 8-bit unsigned integer value of cv
 * @param[in] cv     CLIgen variable
 */
uint8_t
cv_uint8_set(cg_var *cv,
	     uint8_t x)
{
    if (cv == NULL) 
	return 0;
    return (cv->u.varu_uint8 = x);
}

/*! Get 16-bit unsigned integer value of cv
 * @param[in] cv     CLIgen variable
 */
uint16_t
cv_uint16_get(cg_var *cv)
{
    if (cv == NULL) 
	return 0;
    return ((cv)->u.varu_uint16);
}

/*! Set 16-bit unsigned integer value of cv
 * @param[in] cv     CLIgen variable
 */
uint16_t
cv_uint16_set(cg_var  *cv, 
	      uint16_t x)
{
    return (cv->u.varu_uint16 = x);
}

/*! Get 32-bit unsigned integer value of cv
 * @param[in] cv     CLIgen variable
 */
uint32_t
cv_uint32_get(cg_var *cv)
{
    if (cv == NULL) 
	return 0;
    return ((cv)->u.varu_uint32);
}

/*! Set 32-bit unsigned integer value of cv
 * @param[in] cv     CLIgen variable
 */
uint32_t
cv_uint32_set(cg_var  *cv, 
	      uint32_t x)
{
    if (cv == NULL) 
	return 0;
    return (cv->u.varu_uint32 = x);
}

/*! Get 64-bit unsigned integer value of cv
 * @param[in] cv     CLIgen variable
 */
uint64_t
cv_uint64_get(cg_var *cv)
{
    if (cv == NULL) 
	return 0;
    return ((cv)->u.varu_uint64);
}

/*! Set 64-bit unsigned integer value of cv
 * @param[in] cv     CLIgen variable
 */
uint64_t
cv_uint64_set(cg_var  *cv, 
	      uint64_t x)
{
    if (cv == NULL) 
	return 0;
    return (cv->u.varu_uint64 = x);
}

/*! Get n-value of decimal-64 of cv (eg exponent)
 * @param[in] cv     CLIgen variable
 */
uint8_t 
cv_dec64_n_get(cg_var *cv)
{
    if (cv == NULL) 
	return 0;
    return ((cv)->var_dec64_n);
}

/*! Set n-value of decimal-64 of cv (eg exponent)
 * @param[in] cv     CLIgen variable
 * XXX range check? 1..18 
 */
uint8_t 
cv_dec64_n_set(cg_var *cv, 
	       uint8_t x)
{
    if (cv == NULL) 
	return 0;
    return (cv->var_dec64_n = x);
}

/*! Get i-value of decimal-64 of cv (eg base)
 * @param[in] cv     CLIgen variable
 */
int64_t 
cv_dec64_i_get(cg_var *cv)
{
    if (cv == NULL) 
	return 0;
    return ((cv)->var_dec64_i);
}

/*! Set i-value of decimal-64 of cv (eg base)
 * @param[in] cv     CLIgen variable
 */
int64_t 
cv_dec64_i_set(cg_var *cv, 
	       int64_t x)
{
    if (cv == NULL) 
	return 0;
    return (cv->var_dec64_i = x);
}

/*! Get pointer to cv string. 
 *
 * @param[in] cv     CLIgen variable
 * String can be modified in-line but must call _set function to reallocate.
 */
char *
cv_string_get(cg_var *cv)
{
    if (cv == NULL) 
	return 0;
    return ((cv)->u.varu_string);
}

/*! Allocate new string from original NULL-terminated string. Malloc new string and free previous 
 * @param[in] cv     CLIgen variable
 * @param[in] s0     String to copy from
 * @retval    NULL   Error
 * @retval    str    the new (malloced) string
 */
char *
cv_string_set(cg_var *cv, 
	      char   *s0)
{
    char *s1 = NULL;

    if (cv == NULL){
	errno = EINVAL;
	return NULL;
    }
    if (s0 == NULL){
	errno = EINVAL;
	return NULL;
    }

    /* Duplicate s0. Must be done before a free, in case s0 is part of the original */
    if ((s1 = strdup(s0)) == NULL)
	return NULL; /* error in errno */
    if (cv->u.varu_string != NULL)
	free(cv->u.varu_string);
    cv->u.varu_string = s1;
    return s1; 
}

/*! Set new string without malloc 
 * @param[in] cv     CLIgen variable
 * @param[in] s      String to directly assign (must have been alloced)
 * @retval    0      OK
 * @retval    -1     Error
 */
int
cv_string_set_direct(cg_var *cv, 
		     char   *s)
{
    if (cv == NULL){
	errno = EINVAL;
	return -1;
    }
    if (s == NULL){
	errno = EINVAL;
	return -1;
    }
    if (cv->u.varu_string != NULL)
	free(cv->u.varu_string);
    cv->u.varu_string = s;
    return 0; 
}

/*! Allocate new string from original by copying using strncpy
 * @param[in] cv     CLIgen variable
 * @param[in] s0     String to copy
 * @param[in] n      Number of characters to copy (excluding NULL).
 * @retval    NULL   Error
 * @retval    str    the new (malloced) string
 */
char *
cv_strncpy(cg_var *cv, 
	   char   *s0,
	   size_t  n)
{
    char *s1 = NULL;

    if (cv == NULL){
	errno = EINVAL;
	return NULL;
    }
    if (s0 == NULL){
	errno = EINVAL;
	return NULL;
    }
    /* Allocate s1. and then copy */
    if ((s1 = malloc(n+1)) == NULL)
	return NULL; /* error in errno */
    strncpy(s1, s0, n);
    s1[n] = '\0';
    if (cv->u.varu_string != NULL)
	free(cv->u.varu_string);
    cv->u.varu_string = s1;
    return s1; 
}

/*! Get ipv4addr, pointer returned, can be used to set value.
 * @param[in] cv     CLIgen variable
 */
struct in_addr *
cv_ipv4addr_get(cg_var *cv)
{
    if (cv == NULL) 
	return 0;
    return &cv->u.varu_ipv4addr.varipv4_ipv4addr;
}

/*! Set ipv4addr, pointer returned
 * @param[in] cv     CLIgen variable
 * @param[in] addr   storage space for address
 */
struct in_addr *
cv_ipv4addr_set(cg_var *cv,
		struct in_addr *addr)
{
    if (cv && addr) 
	cv->var_ipv4addr = *addr;
    return addr;
}

/*! Get ipv4addr length of cv
 * @param[in] cv     CLIgen variable
 */
uint8_t
cv_ipv4masklen_get(cg_var *cv)
{
    if (cv == NULL) 
	return 0;
    return cv->u.varu_ipv4addr.varipv4_masklen;
}

/*! Set ipv4addr length of cv
 * @param[in] cv     CLIgen variable
 * @param[in] masklen storage spave for masklen
 */
uint8_t
cv_ipv4masklen_set(cg_var *cv, uint8_t masklen)
{
    if (cv == NULL) 
	return 0;
    cv->u.varu_ipv4addr.varipv4_masklen = masklen;
    return masklen;
}

/*! Get ipv6addr, pointer returned, can be used to set value.
 * @param[in] cv     CLIgen variable
 */
struct in6_addr *
cv_ipv6addr_get(cg_var *cv)
{
    if (cv == NULL) 
	return 0;
    return &cv->u.varu_ipv6addr.varipv6_ipv6addr;
}

/*! Get ipv6addr length of cv
 * @param[in] cv     CLIgen variable
 */
uint8_t
cv_ipv6masklen_get(cg_var *cv)
{
    if (cv == NULL) 
	return 0;
    return cv->u.varu_ipv6addr.varipv6_masklen;
}

/*! Returns a pointer to 6-byte mac-address array. 
 *
 * @param[in] cv     CLIgen variable
 * This can be used to set the address too
 */
char *
cv_mac_get(cg_var *cv)
{
    if (cv == NULL) 
	return 0;
    return cv->u.varu_macaddr;
}

/*! Returns a pointer to uuid byte array. 
 * 
 * @param[in] cv     CLIgen variable
 * This can be used to set the uuid too.
 */
unsigned char *
cv_uuid_get(cg_var *cv)
{
    if (cv == NULL) 
	return 0;
    return cv->u.varu_uuid;
}

/*! Set uuid value
 * @param[in] cv     CLIgen variable
 */
unsigned char *
cv_uuid_set(cg_var        *cv, 
	    unsigned char *u)
{
    if (cv == NULL) 
	return 0;
    memcpy((char*)&cv->u.varu_uuid, u, 16);
    return cv->u.varu_uuid;
}

/*! Returns a struct timeval by value.
 * @param[in] cv     CLIgen variable
 */
struct timeval
cv_time_get(cg_var *cv)
{
    if (cv == NULL) {
	struct timeval t = { 0 };
	return t;
    }
    return cv->u.varu_time;
}

/*! Set timeval struct
 * @param[in] cv     CLIgen variable
 * Returns a struct timeval by value.
 */
struct timeval
cv_time_set(cg_var        *cv, 
	    struct timeval t)
{
    if (cv == NULL) {
	struct timeval t = { 0 };
	return t;
    }
    cv->u.varu_time = t;
    return t;
}

/*! Returns a void pointer
 * @param[in] cv     CLIgen variable
 */
void *
cv_void_get(cg_var *cv)
{
    if (cv == NULL)
	return NULL;
    return cv->var_void;
}

/*! Set void pointer
 * @param[in] cv     CLIgen variable
 */
int
cv_void_set(cg_var   *cv, 
	    void     *p)
{
    if (cv)
	cv->var_void = p;
    return 0;
}

/*! Get pointer to URL proto string. 
 *
 * @param[in] cv     CLIgen variable
 * String can be modified in-line but must call _set function to reallocate.
 */
char *
cv_urlproto_get(cg_var *cv)
{
    if (cv == NULL) 
	return 0;
    return (cv)->u.varu_url.varurl_proto;
}

/*! Set URL protocol
 * malloc new string from original. Free previous string if existing.
 * @param[in] cv     CLIgen variable
 */
char *
cv_urlproto_set(cg_var *cv, 
		char   *s0)
{
    char *s1 = NULL;

    if (!cv) {
	return 0;
    }

    /* Duplicate s0. Must be done before a free, in case s0 is part of the original */
    if (s0){
	if ((s1 = strdup(s0)) == NULL)
	    return NULL; /* error in errno */
    }
    if (cv->u.varu_url.varurl_proto != NULL)
	free(cv->u.varu_url.varurl_proto);
    cv->u.varu_url.varurl_proto = s1;
    return s1; 
}

/*! Get pointer to URL address string. 
 *
 * @param[in] cv     CLIgen variable
 * String can be modified in-line but must call _set function to reallocate.
 */
char *
cv_urladdr_get(cg_var *cv)
{
    if (!cv) {
	return 0;
    }

    return (cv)->u.varu_url.varurl_addr;
}

/*! Set URL address
 * @param[in] cv     CLIgen variable
 * malloc new string from original. Free previous string if existing.
 */
char *
cv_urladdr_set(cg_var *cv, 
	       char   *s0)
{
    char *s1 = NULL;

    if (!cv) {
	return 0;
    }

    /* Duplicate s0. Must be done before a free, in case s0 is part of the original */
    if (s0){
	if ((s1 = strdup(s0)) == NULL)
	    return NULL; /* error in errno */
    }
    if (cv->u.varu_url.varurl_addr != NULL)
	free(cv->u.varu_url.varurl_addr);
    cv->u.varu_url.varurl_addr = s1;
    return s1; 
}

/*! Get pointer to URL path string. 
 *
 * @param[in] cv     CLIgen variable
 * String can be modified in-line but must call _set function to reallocate.
 */
char *
cv_urlpath_get(cg_var *cv)
{
    if (!cv) {
	return 0;
    }

    return (cv)->u.varu_url.varurl_path;
}

/*! Set URL path
 * @param[in] cv     CLIgen variable
 * malloc new string from original. Free previous string if existing.
 */
char *
cv_urlpath_set(cg_var *cv, 
	       char   *s0)
{
    char *s1 = NULL;

    if (!cv) {
	return 0;
    }

    /* Duplicate s0. Must be done before a free, in case s0 is part of the original */
    if (s0){
	if ((s1 = strdup(s0)) == NULL)
	    return NULL; /* error in errno */
    }
    if (cv->u.varu_url.varurl_path != NULL)
	free(cv->u.varu_url.varurl_path);
    cv->u.varu_url.varurl_path = s1;
    return s1; 
}

/*! Get pointer to URL user string. 
 *
 * @param[in] cv     CLIgen variable
 * String can be modified in-line but must call _set function to reallocate.
 */
char *
cv_urluser_get(cg_var *cv)
{
    if (!cv) {
	return 0;
    }

    return (cv)->u.varu_url.varurl_user;
}

/*! Set URL user
 * @param[in] cv     CLIgen variable
 * malloc new string from original. Free previous string if existing.
 */
char *
cv_urluser_set(cg_var *cv, 
	       char   *s0)
{
    char *s1 = NULL;

    if (!cv) {
	return 0;
    }

    /* Duplicate s0. Must be done before a free, in case s0 is part of the original */
    if (s0){
	if ((s1 = strdup(s0)) == NULL)
	    return NULL; /* error in errno */
    }
    if (cv->u.varu_url.varurl_user != NULL)
	free(cv->u.varu_url.varurl_user);
    cv->u.varu_url.varurl_user = s1;
    return s1; 
}

/*! Get pointer to URL passwd string. 
 *
 * @param[in] cv     CLIgen variable
 * String can be modified in-line but must call _set function to reallocate.
 */
char *
cv_urlpasswd_get(cg_var *cv)
{
    if (!cv) {
	return 0;
    }

    return (cv)->u.varu_url.varurl_passwd;
}

/*! Set URL password
 * @param[in] cv     CLIgen variable
 * malloc new string from original. Free previous string if existing.
 */
char *
cv_urlpasswd_set(cg_var *cv, 
		 char   *s0)
{
    char *s1 = NULL;

    if (!cv) {
	return 0;
    }

    /* Duplicate s0. Must be done before a free, in case s0 is part of the original */
    if (s0){
	if ((s1 = strdup(s0)) == NULL)
	    return NULL; /* error in errno */
    }
    if (cv->u.varu_url.varurl_passwd != NULL)
	free(cv->u.varu_url.varurl_passwd);
    cv->u.varu_url.varurl_passwd = s1;
    return s1; 
}

/*! Parse an int64 number with explicit base and check for errors
 * @param[in]  str     String containing number to parse
 * @parame[in] base    If base is 0 or 16, the string may include a "0x" prefix,
 *                     the number will be read in base 16; otherwise, a zero base
 *                     is taken  as 10 (decimal) unless the next character is '0',
 *                     in which case it is taken as 8 (octal).
 * @param[in]  imin    Min range (set INT64_MIN for default)    
 * @param[in]  imax    Max range (set INT64_MAX for default)
 * @param[out] val     Value on success
 * @param[out] reason  Error string on failure
 * @retval -1 : Error (fatal), with errno set to indicate error
 * @retval  0 : Validation not OK, malloced reason is returned
 * @retval  1 : Validation OK, value returned in val parameter
 */
static int
parse_int64_base(char    *str, 
		 int      base,
		 int64_t  imin,
		 int64_t  imax, 
		 int64_t *val,
		 char   **reason)
{
    int64_t i;
    char    *ep;
    int      retval = -1;

    errno = 0;
    i = strtoll(str, &ep, base);
    if (str[0] == '\0' || *ep != '\0'){
	if (reason != NULL)
	    if ((*reason = cligen_reason("'%s' is not a number", str)) == NULL){
		retval = -1;
		goto done;
	    }
	retval = 0;
	goto done;
    }
    if (errno != 0){
	if ((i == INT64_MIN || i == INT64_MAX) && errno == ERANGE){ 
	    errno = 0;
	    if (reason != NULL)
		if ((*reason = cligen_reason("Number %s out of range: %" PRId64 " - %" PRId64, str, imin, imax)) == NULL){
		    retval = -1; /* malloc */
		    goto done;
		}
	    retval = 0;
	    goto done;
	}
	else{
	    if ((*reason = cligen_reason("%s: %s", str, strerror(errno))) == NULL){
		errno = 0;
		retval = -1;
		goto done;
	    }
	}
	errno = 0;
    } /* if errno */
    else if (i < imin || i > imax){
	if (reason != NULL)
	    if ((*reason = cligen_reason("Number %s out of range: %" PRId64 " - %" PRId64, str, imin, imax)) == NULL){
		retval = -1;
		goto done;
	    }
	retval = 0;
	goto done;
    }
    *val = i;
    retval = 1; /* OK */
  done:
    return retval;
}

/*! Parse an int8 number and check for errors
 * @param[in]  str     String containing number to parse
 * @param[out] val     Value on success
 * @param[out] reason  Error string on failure
 * @retval -1 : Error (fatal), with errno set to indicate error
 * @retval  0 : Validation not OK, malloced reason is returned
 * @retval  1 : Validation OK, value returned in val parameter
 */
int
parse_int8(char   *str, 
	   int8_t *val,
	   char  **reason)
{
    int64_t  i;
    int      retval;
    
    if ((retval = parse_int64_base(str, 0, -128, 127, &i, reason)) != 1)
	goto done;
    *val = (int8_t)i;
  done:
    return retval;
}

/*! Parse an int16 number and check for errors
 * @param[in]  str     String containing number to parse
 * @param[out] val     Value on success
 * @param[out] reason  Error string on failure
 * @retval -1 : Error (fatal), with errno set to indicate error
 * @retval  0 : Validation not OK, malloced reason is returned
 * @retval  1 : Validation OK, value returned in val parameter
 */
int
parse_int16(char    *str, 
	    int16_t *val, 
	    char   **reason)
{
    int64_t i;
    int      retval;
    
    if ((retval = parse_int64_base(str, 0, -32768, 32767, &i, reason)) != 1)
	goto done;
    *val = (int16_t)i;
  done:
    return retval;
}

/*! Parse an int32 number and check for errors
 * @param[in]  str     String containing number to parse
 * @param[out] val     Value on success
 * @param[out] reason  Error string on failure
 * @retval -1 : Error (fatal), with errno set to indicate error
 * @retval  0 : Validation not OK, malloced reason is returned
 * @retval  1 : Validation OK, value returned in val parameter
 */
int
parse_int32(char    *str, 
	    int32_t *val, 
	    char   **reason)
{
    int64_t  i;
    int      retval;
    
    if ((retval = parse_int64_base(str, 0, INT_MIN, INT_MAX, &i, reason)) != 1)
	goto done;
    *val = (int32_t)i;
  done:
    return retval;
}

/*! Parse an int64 number and check for errors
 * @param[in]  str     String containing number to parse
 * @param[out] val     Value on success
 * @param[out] reason  Error string on failure
 * @retval -1 : Error (fatal), with errno set to indicate error
 * @retval  0 : Validation not OK, malloced reason is returned
 * @retval  1 : Validation OK, value returned in val parameter
 */
int
parse_int64(char    *str, 
	    int64_t *val,
	    char   **reason)
{
    return parse_int64_base(str, 0, INT64_MIN, INT64_MAX, val, reason);
}

/*! Parse an uint64 number and check for errors
 * @param[in]  str     String containing number to parse
 * @parame[in] base    If base is 0 or 16, the string may include a "0x" prefix,
 *                     the number will be read in base 16; otherwise, a zero base
 *                     is taken  as 10 (decimal) unless the next character is '0',
 *                     in which case it is taken as 8 (octal).
 * @param[in]  umin    Min range (set UINT64_MIN for default)    
 * @param[in]  umax    Max range (set UINT64_MAX for default)
 * @param[out] val     Value on success
 * @param[out] reason  Error string on failure (if given)
 * @retval -1 : Error (fatal), with errno set to indicate error
 * @retval  0 : Validation not OK, malloced reason is returned
 * @retval  1 : Validation OK, value returned in val parameter
 * @note: we have to detect a minus sign ourselves,....
 */
static int
parse_uint64_base(char     *str, 
		  int       base,
		  uint64_t  umin,
		  uint64_t  umax, 
		  uint64_t *val, 
		  char    **reason)
{
    uint64_t i;
    char    *ep;
    int      retval = -1;

    errno = 0;
    i = strtoull(str, &ep, base);
    if (str[0] == '\0' || *ep != '\0'){
	if (reason != NULL)
	    if ((*reason = cligen_reason("'%s' is not a number", str)) == NULL){
		retval = -1; /* malloc */
		goto done;
	    }
	retval = 0;
	goto done;
    }
    if (errno != 0){
	if (i == UINT64_MAX && errno == ERANGE){ 
	    if (reason != NULL)
		if ((*reason = cligen_reason("Number %s out of range: 0 - %" PRIu64, str, umax)) == NULL){
		    retval = -1; /* malloc */
		    goto done;
		}
	    retval = 0;
	    goto done;
	}
	else{
	    if ((*reason = cligen_reason("%s: %s", str, strerror(errno))) == NULL){
		retval = -1; /* malloc */
		goto done;
	    }
	    retval = 0;
	    goto done;
	}
    }
    else if (i > umax){
	if (reason != NULL)
	    if ((*reason = cligen_reason("Number %s out of range: %" PRIu64 " - %" PRIu64, str, umin, umax)) == NULL){
		retval = -1;
		goto done;
	    }
	retval = 0;
	goto done;
    }
    /* strtoull does _not_ detect negative numbers,... */
    if (strchr(str, '-') != NULL){
	if (reason != NULL)
	    if ((*reason = cligen_reason("Number %s out of range: 0 - %" PRIu64, str, umax)) == NULL){
		retval = -1; /* malloc */
		goto done;
	    }
	retval = 0;
	goto done;
    }
    *val = i;
    retval = 1; /* OK */
  done:
    return retval;
}

/*! Parse an uint8 number and check for errors
 * @param[in]  str     String containing number to parse
 * @param[out] val     Value on success
 * @param[out] reason  Error string on failure
 * @retval -1 : Error (fatal), with errno set to indicate error
 * @retval  0 : Validation not OK, malloced reason is returned
 * @retval  1 : Validation OK, value returned in val parameter
 */
int
parse_uint8(char    *str, 
	    uint8_t *val, 
	    char   **reason)
{
    uint64_t i;
    int      retval;
    
    if ((retval = parse_uint64_base(str, 0, 0, 255, &i, reason)) != 1)
	goto done;
    *val = (uint8_t)i;
  done:
    return retval;
}

/*! Parse an uint16 number and check for errors
 * @param[in]  str     String containing number to parse
 * @param[out] val     Value on success
 * @param[out] reason  Error string on failure
 * @retval -1 : Error (fatal), with errno set to indicate error
 * @retval  0 : Validation not OK, malloced reason is returned
 * @retval  1 : Validation OK, value returned in val parameter
 */
int
parse_uint16(char     *str, 
	     uint16_t *val, 
	     char    **reason)
{
    uint64_t i;
    int      retval;
    
    if ((retval = parse_uint64_base(str, 0, 0, 65535, &i, reason)) != 1)
	goto done;
    *val = (uint16_t)i;
  done:
    return retval;
}

/*! Parse an uint32 number and check for errors
 * @param[in]  str     String containing number to parse
 * @param[out] val     Value on success
 * @param[out] reason  Error string on failure
 * @retval -1 : Error (fatal), with errno set to indicate error
 * @retval  0 : Validation not OK, malloced reason is returned
 * @retval  1 : Validation OK, value returned in val parameter
 */
int
parse_uint32(char     *str, 
	     uint32_t *val, 
	     char    **reason)
{
    uint64_t i;
    int      retval;
    
    if ((retval = parse_uint64_base(str, 0, 0, UINT_MAX, &i, reason)) != 1)
	goto done;
    *val = (uint32_t)i;
  done:
    return retval;
}

/*! Parse an uint64 number and check for errors
 * @param[in]  str     String containing number to parse
 * @param[out] val     Value on success
 * @param[out] reason  Error string on failure (if given)
 * @retval -1 : Error (fatal), with errno set to indicate error
 * @retval  0 : Validation not OK, malloced reason is returned
 * @retval  1 : Validation OK, value returned in val parameter
 * @note: we have to detect a minus sign ourselves,....
 */
int
parse_uint64(char     *str, 
	     uint64_t *val,
	     char   **reason)
{
    return parse_uint64_base(str, 0, 0, UINT64_MAX, val, reason);
}

/*! Parse a decimal64 value
 * @param[in]  str        String to parse
 * @param[in]  n          number of decimals
 * @param[out] dec64_i    64-bit number
 * @param[out] reason     if given, malloced err string (retval=0), needs freeing
 * @retval -1             fatal error
 * @retval 0              parse error, reason in reason
 * @retval 1              OK
 */
int
parse_dec64(char    *str, 
	    uint8_t  n, 
	    int64_t *dec64_i, 
	    char   **reason)
{
    int      retval = 1;
    char    *s0 = NULL; /* the whole string, eg aaa.bbb*/
    char    *s1;        /* the first part (eg aaa)  */
    char    *s2;        /* the second part (eg bbb)  */
    char    *ss = NULL; /* Help string */
    int      len1;
    int      len2 = 0;
    int      i;

/*
     +----------------+-----------------------+----------------------+
     | fraction-digit | min                   | max                  |
     +----------------+-----------------------+----------------------+
     | 1              | -922337203685477580.8 | 922337203685477580.7 |
     | 18             | -9.223372036854775808 | 9.223372036854775807 |
     +----------------+-----------------------+----------------------+
*/

    if (n<=0 || n>18){
	if (reason != NULL)
	    if ((*reason = cligen_reason("%s: fraction-digit=%d given but should be in interval [1:18]", __FUNCTION__, n)) == NULL){
		goto done;
	    }
	retval = 0;
	goto done;
    }
    if ((s0 = strdup(str)) == NULL){
	retval = -1; /* malloc */
	goto done;
    }
    s2 = s0;
    s1 = strsep(&s2, ".");
    len1 = strlen(s1);
    if ((ss = malloc(strlen(str)+n+2)) == NULL){
	retval = -1; /* malloc */
	goto done;
    }
    memcpy(ss, s1, len1);

    /*
     *     | s1 |.| s2 |
     *     | s1 | s2 |000|
     *           <---n-->
     */
    if (s2){
	len2 = strlen(s2);
	if (len2 > n){
	    if (reason != NULL)
		if ((*reason = cligen_reason("%s has %d fraction-digits but may only have %d", str, len2, n)) == NULL){
		    retval = -1; /* malloc */
		    goto done;
		}
	    retval = 0;
	    goto done;
	}
	memcpy(ss+len1, s2, len2); 
    }
    /* Fill out with trailing zeroes if any 
       | s1 | s2 |
     */
    for (i=len1+len2; i<len1+n; i++)
	ss[i] = '0';
    ss[len1+n] = '\0'; /* trailing zero */
    /* XXX: remove any beginning zeros */
    if ((retval = parse_int64_base(ss, 10, INT64_MIN, INT64_MAX, dec64_i, reason)) != 1)
	goto done;
  done:
    if (s0)
	free(s0);
    if (ss)
	free(ss);
    return retval;
}

/*! Parse boolean values, eg true or false, on or off
 * @param[in]  str        String to parse
 * @param[in]  val        integer 0 or 1
 * @param[out] reason     if given, malloced err string (retval=0), needs freeing
 * @retval -1             fatal error
 * @retval 0              parse error, reason in reason
 * @retval 1              OK
 * @code
 *    int     ret;
 *    char   *reason = NULL;
 *    uint8_t val;
 *
 *    if ((ret = parse_bool("true", &val, &reason)) < 0){
 *       err;
 *    if (ret == 0)
 *       fprintf(stderr, "parse error: %s\n", reason); 
 *    else
 *        fprintf(stdout, "val: %hu\n", val); 
 *    if (reason)
 *	 free(reason);
 * @endcode
 */
int
parse_bool(char    *str, 
	   uint8_t *val, 
	   char   **reason)
{
    int i;
    int retval = 1;

    if (strcmp(str, "true") == 0)
	i = 1;
    else if (strcmp(str, "false") == 0)
	i = 0;
#ifdef BOOL_TRUTH_ON_OFF
    else if (strcmp(str, "on") == 0)
	i = 1;
    else if (strcmp(str, "off") == 0)
	i = 0;
#endif
#ifdef BOOL_TRUTH_ENABLE_DISABLE
    else if (strcmp(str, "enable") == 0)
	i = 1;
    else if (strcmp(str, "disable") == 0)
	i = 0;
#endif    
    else{
	if (reason)
	    if ((*reason = cligen_reason("'%s' is not a boolean value", str)) == NULL){
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

/*! Parse an IPv4 address struct
 * @param[in]  str        String to parse
 * @param[out]  val        IPv4 binary address
 * @param[out] reason     if given, malloced err string (retval=0), needs freeing
 * @retval -1             fatal error
 * @retval 0              parse error, reason in reason
 * @retval 1              OK
 */
int
parse_ipv4addr(char           *str, 
	       struct in_addr *val, 
	       char          **reason)
{
    int retval = -1;

    if ((retval = inet_pton(AF_INET, str, val)) < 0)
	goto done;
    if (retval == 0 && reason) 
	if ((*reason = cligen_reason("Invalid IPv4 address")) == NULL)
	    retval = -1;
  done:
    return retval;
}

/*! Parse an IPv6 address struct
 * @param[in]  str        String to parse
 * @param[in]  val        IPv6 binary address
 * @param[out] reason     if given, malloced err string (retval=0), needs freeing
 * @retval -1             fatal error
 * @retval 0              parse error, reason in reason
 * @retval 1              OK
 */
int
parse_ipv6addr(char            *str, 
	       struct in6_addr *val, 
	       char           **reason)
{
    int retval = -1;

    if ((retval = inet_pton(AF_INET6, str, val)) < 0)
	goto done;
    if (retval == 0 && reason) 
	if ((*reason = cligen_reason("Invalid IPv6 address")) == NULL)
	    retval = -1;
  done:
    return retval;
}


/*! Own version of ether_aton(): 
 * parse string in colon hex notation and return a vector of chars.
 * @param[out] reason     if given, malloced err string (retval=0), needs freeing
 */

#define MACADDR_OCTETS	6
#define MACADDR_STRLEN	(MACADDR_OCTETS * 3) - 1    /* 6*sizeof("xx:")-1 */

static int
parse_macaddr(char  *str, 
	      char   addr[MACADDR_OCTETS], 
	      char **reason)
{
    char *s1;
    int n_colons;
    unsigned int octets[MACADDR_OCTETS];
    int i;

    /*
     * MAC addresses are exactly MACADDR_STRLEN (17) bytes long.
     */
    if ((str == NULL) || strlen(str) != MACADDR_STRLEN) {
	if (reason && (*reason = cligen_reason("%s: Invalid MAC address (bad length)", str)) == NULL) {
	    return -1;
	}
	return 0;
    }

    /*
     * Allow only valid charcters 0-9, a-f, A-F and ':'.
     */
    n_colons = 0;
    for (s1 = str; s1 && *s1; ++s1) {
	if (isxdigit(*s1)) {
	    continue;
	}
	if (*s1 == ':') {
	    ++n_colons;
	    continue;
	}

	if (reason) {
	    *reason = cligen_reason("%s: Invalid MAC address (illegal character '%c')", str, *s1);
	    if (*reason == NULL) {
		return -1;
	    }
	}
	return 0;
    }

    /*
     * Ensure exactly 6 octets.
     */
    if (n_colons != MACADDR_OCTETS - 1) {
	if (reason) {
	    *reason = cligen_reason("%s: Invalid MAC address (should have 6 octets, not %d)", str, n_colons + 1);
	    if (*reason == NULL) {
		return -1;
	    }
	}
	return 0;
    }

    /*
     * Ensure octets are proper two-character widths.
     */
    if (str[2] != ':' || str[5] != ':' || str[8] != ':'
	|| str[11] != ':' || str[14] != ':' || str[17] != 0) {
	if (reason) {
	    *reason = cligen_reason("%s: Invalid MAC address (poorly formed octets)", str);
	    if (*reason == NULL) {
		return -1;
	    }
	}
	return 0;
    }

    sscanf(str,
	   "%x:%02x:%02x:%02x:%02x:%02x",
	   octets + 0, octets + 1, octets + 2,
	   octets + 3, octets + 4, octets + 5);

    for (i = 0; i < MACADDR_OCTETS; ++i) {
	addr[i] = octets[i];
    }

    return 1;	/* OK */
}

/*! Parse URL
 * @param[in]  url    Syntax:  <proto>://[<user>[:<passwd>]@]<addr>[/<path>]. May 
 *                    be changed destructively
 * @param[in]  cv     CLIgen variable
 * @param[out] reason if given, malloced err string (retval=0), needs freeing
 * URL syntax: Syntax:  <proto>://[<user>[:<passwd>]@]<addr>[/<path>]. 
 * @note The syntax is not complete: it is recommended to use regexps
 */
static int
parse_url(char   *url, 
	  cg_var *cv, 
	  char  **reason)
{
    char    *str0 = NULL;
    char    *str;
    char    *tmp;
    char    *tmp2;
    int      retval = -1;

    if ((str0 = strdup(url)) == NULL)
	goto done;
    str = str0;
    if ((tmp = strchr(str, ':')) == NULL)
	goto warn;
    if (strncmp(tmp, "://", 3)) /* :// */
	goto warn;
    *tmp = '\0';
    cv->var_urlproto = strdup(str);
    str = tmp+3; 
    if ((tmp = strchr(str, '@')) != NULL){
	/* user field */
	*tmp = '\0';
	if ((tmp2 = strchr(str, ':')) != NULL){
	    /* user:passwd field */
	    *tmp2 = '\0';
	    cv->var_urlpasswd = strdup(tmp2+1);
	}
	else
	    cv->var_urlpasswd = strdup("");
	cv->var_urluser = strdup(str);
	str = tmp+1;
    }
    else{
	cv->var_urluser = strdup("");
	cv->var_urlpasswd = strdup("");
    }
    if ((tmp = strchr(str, '/')) != NULL)
	*tmp = '\0';
    cv->var_urladdr = strdup(str);
    if (tmp){
	str = tmp+1;
	cv->var_urlpath = strdup(str);
    }
    else
	cv->var_urlpath = strdup("");
    retval = 1; /* OK */
  done:
    if (str0)
	free(str0);
    return retval;
  warn:
    if (str0)
	free(str0);
    if (reason && (*reason = cligen_reason("%s: Invalid URL", url)) == NULL)
	return -1;
    return 0;
}

/*! Translate url protocol string to enum
 * @param[in]  str  
 */
int
str2urlproto(char *str)
{
    int proto;

    for (proto = 1;; proto++){
	if (cg_urlprotostr[proto] == NULL)
	    break;
	if (strcmp(str, cg_urlprotostr[proto]) == 0)
	    return proto;
    }
    return 0;
}

/*! Translate uuid binary data structure to uuid ascii string
 * @param[in]  u     UUID as binary data structure
 * @param[out] fmt   Format string. 
 * @param[in]  len   Length of fmt needs to be at 37.
 * @retval    0  OK
 * @retval   -1  Error
 */
int
uuid2str(uuid_t u, 
	 char  *fmt, 
	 int    len)
{
    snprintf(fmt, len, 
	    "%02x%02x%02x%02x-"	"%02x%02x-"	"%02x%02x-"	"%02x%02x-"  
	    "%02x%02x%02x%02x%02x%02x", 
	    u[0]&0xff,	    u[1]&0xff,	    u[2]&0xff,	    u[3]&0xff,
	    u[4]&0xff,	    u[5]&0xff,	    u[6]&0xff,	    u[7]&0xff,	    
	    u[8]&0xff,	    u[9]&0xff,	    u[10]&0xff,	    u[11]&0xff,	    
	    u[12]&0xff,	    u[13]&0xff,	    u[14]&0xff,	    u[15]&0xff);
    return 0;
}

/*! Given a single hex character, return its number as int
 */
static int 
toint(char c)
{
  if (c >= '0' && c <= '9') 
      return      c - '0';
  if (c >= 'A' && c <= 'F') 
      return 10 + c - 'A';
  if (c >= 'a' && c <= 'f') 
      return 10 + c - 'a';
  return -1;
}

/*! Translate uuid ascii string to uuid binary data structure.
 * uuid string on form f47ac10b-58cc-4372-a567-0e02b2c3d479 to uuid data structure.
 * @param[in]  in    in-string is 36 bytes + null termination (37 bytes in total).
 * @param[out] u     uuid is a 16 byte unsigned character array.
 * @retval     0     OK
 * @retval    -1     Error
 */
int
str2uuid(char  *in, 
	 uuid_t u)
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
	u[i++]    = (a << 4) | b; 
    }
    if (in[j++] != '-')
	goto done;
    for (k=0; k<2; k++){
	a = toint(in[j++]);  b = toint(in[j++]);
	if (a < 0 || b < 0)
	    goto done;
	u[i++]    = (a << 4) | b; 
    }
    if (in[j++] != '-')
	goto done;
    for (k=0; k<2; k++){
	a = toint(in[j++]);  b = toint(in[j++]);
	if (a < 0 || b < 0)
	    goto done;
	u[i++]    = (a << 4) | b; 
    }
    if (in[j++] != '-')
	goto done;
    for (k=0; k<2; k++){
	a = toint(in[j++]);  b = toint(in[j++]);
	if (a < 0 || b < 0)
	    goto done;
	u[i++]    = (a << 4) | b; 
    }
    if (in[j++] != '-')
	goto done;
    for (k=0; k<6; k++){
	a = toint(in[j++]);  b = toint(in[j++]);
	if (a < 0 || b < 0)
	    goto done;
	u[i++]    = (a << 4) | b; 
    }
    if (in[j] != '\0')
	goto done;
    retval = 0;
done:
    return retval;
}

/*! Given a single digit character, return its number as int
 */
static int 
todig(char c)
{
  if (c >= '0' && c <= '9') 
      return      c - '0';
  return -1;
}

/*! Given a string with digits, return a number
 * @param[in] n  Number of digits to read from s.
 * @param[in] s  String containing digits
 * @retval    0  OK
 * @retval   -1  Error
 * eg if s is "23456" and nr is 3, then return 234.
 * @note works for positive numbers and up to 2^31
 */
int
cligen_tonum(int   n, 
	     char *s)
{
    int      i;
    int      a;
    int      sum = 0;
    int      retval = -1;

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

/*! Translate (ascii) ISO 8601 time string to (binary) timeval
 * Translate ISO 8601 date+time on the form 2008-09-21T18:57:21.003456Z to a timeval structure.
 * @param[in]  in   Input ISO 8601 string
 * @param[out] tv   Timeval
 * @retval     0    OK
 * @retval    -1    Error
 * T can be space.
 * in-string is upto 26 bytes + null termination (27 bytes in total).
 * out timestamp is a 4+4 integer struct timeval  
 * returns 0 if OK parses. -1 if not correct.
 * usec can be 1-6 digits. That is, .3 means 300000 usec.
 * @note : string coming in is expected to be UTC and we parse that into a tm struct and
 * then call mktime(). Problem is mktime() assumes tm is localtime. Therefore we must
 * adjust the timeval with the timezone to get it right. (Should be a mktime() that is
 * UTC).
 * @note ISO 8601 accepts years 0000-9999, but struct timeval is more restricted. In this code between 1970-2104
 * @note Zone designator support have been added (2019-01) but are not stored.
 * @note You should be able to leave out less significant fields. That is, 2003 is
 * a time. But now you can only leave out usec.
 * @see time2str
 */
int
str2time(char           *in, 
	 struct timeval *tv)
{
    int        retval = -1;
    int        i = 0;
    int        j;
    int        len;
    int        year;
    int        month;
    int        day;
    int        hour;
    int        min;
    int        sec;
    int        usec = 0;
    struct tm *tm = NULL;
    time_t     t;
    char       frac[7];

    if ((year = cligen_tonum(4, &in[i])) < 0)
	goto done;
    if (year < 1970 || year > 2104)
	goto done;
    i += 4;
    if (in[i++] != '-')
	goto done;
    if ((month = cligen_tonum(2, &in[i])) < 0)
	goto done;
    if (month < 1 || month > 12)
	goto done;
    i += 2;
    if (in[i++] != '-')
	goto done;
    if ((day = cligen_tonum(2, &in[i])) < 0)
	goto done;
    if (day < 1 || day > 31)
	goto done;
    i += 2;
    if (in[i] != 'T' && in[i] != ' ')
	goto done;
    i++;
    if ((hour = cligen_tonum(2, &in[i])) < 0)
	goto done;
    if (hour > 23)
	goto done;
    i += 2;
    if (in[i++] != ':')
	goto done;
    if ((min = cligen_tonum(2, &in[i])) < 0)
	goto done;
    if (min > 59)
	goto done;
    i += 2;
    if (in[i++] != ':')
	goto done;
    if ((sec = cligen_tonum(2, &in[i])) < 0)
	goto done;
    if (sec > 59)
	goto done;
    i += 2;
    switch (in[i]){
    case '\0':
	goto mkdate;
	break;
    case '.':
	i++;
	break;
    case 'Z':
    case '+':
    case '-':
	goto zone;
    default:
	goto done;
	break;
    }
    for (len=0; len<6; len++){
	if (isdigit(in[i+len]))
	    frac[len] = in[i+len];
	else
	    break;
    }
    if (len < 1)
	goto done;
    frac[len] = 0;
    if ((usec = cligen_tonum(len, frac)) < 0)
	goto done;
    for (j=0; j<6-len; j++)
	usec *= 10;
    if (usec > 999999)
	goto done;
    i += len;
 zone:
    switch (in[i]){
    case 'Z': /* does not parse zone*/
	i++;
	break;
    case '+': 
    case '-': /* not parsed */
	goto mkdate;
    case '\0':
	break;
    default:
	goto done;
    }
    if (in[i] != '\0')
	goto done;
  mkdate:
    if ((tm = calloc(1, sizeof(struct tm))) == NULL){
	fprintf(stderr, "calloc: %s\n", strerror(errno));
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
#if 0 /* If I enable this, I get 1h extra when loading from file in summer.
         When parsing a timestamp such as 2013-04-03T10:50:36 from file to cv
	 (and db). So that when it is printed again it is 2013-04-03T11:50:36 
	 I dont know why I enabled it in the first place. That is why I make this
	 note.
      */
    if (tm->tm_isdst) /* Daylight savings time */
	t += 3600; /* add hour */
#endif
    /* Must adjust with timezone, since mktime() assumes tm i local but it is UTC! */
#ifdef linux
    t = t - timezone;  
#else
    {
	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv, &tz);
	t = t - tz.tz_minuteswest*60; 
    }
#endif
    tv->tv_sec = t; 
    tv->tv_usec = usec;
    retval = 0;
done:
    if (tm)
	free(tm);
    return retval;
}

/*! Translate (binary) timeval to (ascii) ISO 8601 time string
 * from timeval to  ISO 8601 date+time on the form 2008-09-21T18:57:21.003456Z
 * @param[in]  tv   Timeval
 * @param[out] fmt  Format string. 
 * @param[in]  len  Length of format string. Must be at least 28 bytes.
 * @retval     0    OK
 * @retval    -1    Error
 * @code
 *   char timestr[28];
 *   struct timeval tv;
 *   gettimeofday(&tv);
 *   if (time2str(tv, timestr, sizeof(timestr)) < 0)
 *     err;
 *   printf("%s", timestr);
 * @endcode
 * @see str2time
 */
int
time2str(struct timeval tv, 
	 char          *fmt,
	 int            len)
{
    int        retval = -1;
    struct tm *tm;

    tm = gmtime((time_t*)&tv.tv_sec);
    if (snprintf(fmt, len, "%04d-%02d-%02dT%02d:%02d:%02d.%06ldZ",
	     tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, 
		 tm->tm_min, tm->tm_sec, tv.tv_usec) < 0)
	goto done;
    retval = 0;
done:
    return retval;
}

/*! Translate (parse) a string to a CV type.
 * @param[in] str    Name of type
 * @retval    type   CLIgen variable type
 */
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
      return CGV_VOID; /* N/A */
  if (strcmp(str,"empty") == 0)
      return CGV_EMPTY;
  return CGV_ERR;
}

/*! Translate (print) a cv type to a static string.
 * @param[in] tpe    CLIgen variable type
 * @retval    NULL   Error
 * @retval    str    Static string containing name of type as ASCII string
 */
char *
cv_type2str(enum cv_type type)
{
    char *str = NULL;
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
    case CGV_EMPTY:
	str="empty";
	break;
    default:
	fprintf(stderr, "%s: invalid type: %d\n", __FUNCTION__, type);
	break;
    }
    return str;
}

/*! Return length of cligen variable value (as stored in binary)
 *
 * @param[in] cv     CLIgen variable
 * @retval    len    Length of variable
 * Special with strings that are only pointed to by the variable.
 * In that case the lengths of the strings pointed to are added, trailing
 * null included.
 */
size_t
cv_len(cg_var *cv)
{
    int len = 0;

    switch (cv->var_type){
    case CGV_INT8:
	len = sizeof(cv->var_int8);
	break;
    case CGV_INT16:
	len = sizeof(cv->var_int16);
	break;
    case CGV_INT32:
	len = sizeof(cv->var_int32);
	break;
    case CGV_INT64:
	len = sizeof(cv->var_int64);
	break;
    case CGV_UINT8:
	len = sizeof(cv->var_uint8);
	break;
    case CGV_UINT16:
	len = sizeof(cv->var_uint16);
	break;
    case CGV_UINT32:
	len = sizeof(cv->var_uint32);
	break;
    case CGV_UINT64:
	len = sizeof(cv->var_uint64);
	break;
    case CGV_DEC64:
	len = sizeof(cv->var_dec64_i) + sizeof(cv->var_dec64_n);
	break;
    case CGV_BOOL:
	len = sizeof(cv->var_bool);
	break;
    case CGV_REST:
	len = (cv->var_rest ? strlen(cv->var_rest) : 0) + 1;
	break;
    case CGV_STRING: 
	len = (cv->var_string ? strlen(cv->var_string) : 0) + 1;
	break;
    case CGV_INTERFACE: 
	len = (cv->var_interface ? strlen(cv->var_interface) : 0) + 1;
	break;
    case CGV_IPV4ADDR:
	len = sizeof(cv->var_ipv4addr);
	break;
    case CGV_IPV4PFX:
	len = sizeof(cv->u.varu_ipv4addr);
	break;
    case CGV_IPV6ADDR:
	len = sizeof(cv->var_ipv6addr);
	break;
    case CGV_IPV6PFX:
	len = sizeof(cv->u.varu_ipv6addr);
	break;
    case CGV_MACADDR:
	len = sizeof(cv->var_macaddr);
	break;
    case CGV_URL:
	len = (cv->var_urlproto ? strlen(cv->var_urlproto):0) + 1 +
	    (cv->var_urladdr ? strlen(cv->var_urladdr):0) + 1 +
	    (cv->var_urlpath ? strlen(cv->var_urlpath):0) + 1 +
	    (cv->var_urluser ? strlen(cv->var_urluser):0) + 1 +
	    (cv->var_urlpasswd ? strlen(cv->var_urlpasswd):0) + 1;
	break;
    case CGV_UUID:
	len = sizeof(cv->var_uuid);
	break;
    case CGV_TIME:
	len = sizeof(cv->var_time);
	break;
    case CGV_VOID:
	len = sizeof(void*); /* N/A */
	break;
    case CGV_EMPTY:
	len = 0;
	break;
    default:
	break;
    }
    return len;
}

/*! Print a dec64 cv to a string 
 *
 * @param[in]     cv   A cligen variable of type CGV_DEC64 to print
 * @param[out]    s0   A string that will hold the dec64
 * @param[inout]  len  A string that holds available free space in s0
 * @retval        0    OK
 * @retval        -1   Error with error msg on stderr. 
 */
static int
cv_dec64_print(cg_var *cv, 
	       char   *s0, 
	       int    *s0len)
{
    int      i;
    uint8_t  n = cv->var_dec64_n;;
    int      len;
    int64_t di;

    assert(0<n && n<19);
    /* Convert negative numbers to positive and prepend a '-' at the end */
    di = cv_dec64_i_get(cv);
    if (di<0)
	di = -di;
    len = snprintf(s0, *s0len, "%0*" PRId64, n+1, di);
    assert(len>=n);
    *s0len -= len;
    /* Shift fraction digits right, including null character. 
     * eg:  xyz --> x.yz (if n==2) 
     */
    for (i=len; i>=len-n; i--) 
	s0[i+1] = s0[i];
    (*s0len)--;
    s0[len-n] = '.';
    /* prepend a '-' if the number is negative*/    
    if (cv_dec64_i_get(cv)<0){ 
	for (i=len+1; i>=0; i--)
	    s0[i+1] = s0[i];
	s0[0] = '-';
	(*s0len)--;
    }
    return 0;
}

/*! Print value of CLIgen variable to CLIgen buf using printf style formats.
 * @param[in]   cv   CLIgen variable
 * @param[out]  cb   Value printed 
 * The params shuld be switched cb<->cv
*/
int
cv2cbuf(cg_var *cv,
	cbuf   *cb)
{
    char straddr[INET6_ADDRSTRLEN];
    char ss[64];
    int  sslen = sizeof(ss);
    char uuidstr[37];
    char timestr[28];

    switch (cv->var_type){
    case CGV_INT8:
	cprintf(cb, "%" PRId8, cv->var_int8);
	break;
    case CGV_INT16:
	cprintf(cb, "%" PRId16, cv->var_int16);
	break;
    case CGV_INT32:
	cprintf(cb, "%" PRId32, cv->var_int32);
	break;
    case CGV_INT64:
	cprintf(cb, "%" PRId64, cv->var_int64);
	break;
    case CGV_UINT8:
	cprintf(cb, "%" PRIu8, cv->var_uint8);
	break;
    case CGV_UINT16:
	cprintf(cb, "%" PRIu16, cv->var_uint16);
	break;
    case CGV_UINT32:
	cprintf(cb, "%" PRIu32, cv->var_uint32);
	break;
    case CGV_UINT64:
	cprintf(cb, "%" PRIu64, cv->var_uint64);
	break;
    case CGV_DEC64:
	cv_dec64_print(cv, ss, &sslen);
	cprintf(cb, "%s", ss);
	break;
    case CGV_BOOL:
	if (cv->var_bool)
	    cprintf(cb, "true");
	else
	    cprintf(cb, "false");
	break;
    case CGV_REST:
	cprintf(cb, "%s", cv->var_rest);
	break;
    case CGV_STRING: 
	cprintf(cb, "%s", cv->var_string);
	break;
    case CGV_INTERFACE: 
	cprintf(cb, "%s", cv->var_interface);
	break;
    case CGV_IPV4ADDR:
	cprintf(cb, "%s", inet_ntoa(cv->var_ipv4addr));
	break;
    case CGV_IPV4PFX:
	cprintf(cb, "%s/%u", 
		inet_ntoa(cv->var_ipv4addr),
		cv->var_ipv4masklen);
	break;
    case CGV_IPV6ADDR:
	if (inet_ntop(AF_INET6, &cv->var_ipv6addr, straddr, sizeof(straddr)) < 0){
	    fprintf(stderr, "inet_ntop: %s\n", strerror(errno));
	    return -1;
	}
	cprintf(cb, "%s", straddr);
	break;
    case CGV_IPV6PFX:
	if (inet_ntop(AF_INET6, &cv->var_ipv6addr, straddr, sizeof(straddr)) < 0){
	    fprintf(stderr, "inet_ntop: %s\n", strerror(errno));
	    return -1;
	}
	cprintf(cb, "%s/%u", straddr, cv->var_ipv6masklen);
	break;
    case CGV_MACADDR:
	cprintf(cb, "%02x:%02x:%02x:%02x:%02x:%02x", 
		(uint8_t)cv->var_macaddr[0],
		(uint8_t)cv->var_macaddr[1],
		(uint8_t)cv->var_macaddr[2],
		(uint8_t)cv->var_macaddr[3],
		(uint8_t)cv->var_macaddr[4],
		(uint8_t)cv->var_macaddr[5]);
	break;
    case CGV_URL: /* <proto>://[<user>[:<passwd>]@]<addr>[/<path>] */
	cprintf(cb, "%s://%s%s%s%s%s/%s", 	
		cv->var_urlproto,
		cv->var_urluser,
		strlen(cv->var_urlpasswd)?":":"",
		cv->var_urlpasswd,
		strlen(cv->var_urluser)||strlen(cv->var_urlpasswd)?"@":"",
		cv->var_urladdr,
		cv->var_urlpath
	    );
	break;
    case CGV_UUID:
	uuid2str(cv->var_uuid, uuidstr, sizeof(uuidstr));
	cprintf(cb, "%s", uuidstr);
	break;
    case CGV_TIME:
	time2str(cv->var_time, timestr, sizeof(timestr));
	cprintf(cb, "%s", timestr);
	break;
    case CGV_VOID: /* N/A */
    case CGV_EMPTY: 
	break;
    default:
	break;
    }
    return 0;
}

/*! Print value of CLIgen variable using printf style formats.
 *
 * You can use str=NULL to get the expected length.
 * The number of (potentially if str=NULL) written bytes is returned.
 * The value is printed as it would have been input, ie the reverse of
 * parsing.
 * Typically used by external code when transforming cgv:s.
 * Note, for strings, the length returned is _excluding_ the null byte, but the length
 * in supplied in the argument list is _including_ the null byte.
 * @param[in]   cv   CLIgen variable
 * @param[out]  str  Value printed in this string
 * @param[in]   size Length of 'str'
 * @retval len  How many bytes printed
 * @see  cv2cbuf   which also prints a CV but to cbuf
 * @see  cv_print  which also prints a CV but to a file
 */
int
cv2str(cg_var *cv, 
       char   *str, 
       size_t  size)
{
    int  len = 0;
    char straddr[INET6_ADDRSTRLEN];
    char ss[64];
    int  sslen = sizeof(ss);
    char uuidstr[37];
    char timestr[28];

    if (cv == NULL) 
	return 0;
    switch (cv->var_type){
    case CGV_INT8:
	len = snprintf(str, size, "%" PRId8, cv->var_int8);
	break;
    case CGV_INT16:
	len = snprintf(str, size, "%" PRId16, cv->var_int16);
	break;
    case CGV_INT32:
	len = snprintf(str, size, "%" PRId32, cv->var_int32);
	break;
    case CGV_INT64:
	len = snprintf(str, size, "%" PRId64, cv->var_int64);
	break;
    case CGV_UINT8:
	len = snprintf(str, size, "%" PRIu8, cv->var_uint8);
	break;
    case CGV_UINT16:
	len = snprintf(str, size, "%" PRIu16, cv->var_uint16);
	break;
    case CGV_UINT32:
	len = snprintf(str, size, "%" PRIu32, cv->var_uint32);
	break;
    case CGV_UINT64:
	len = snprintf(str, size, "%" PRIu64, cv->var_uint64);
	break;
    case CGV_DEC64:
	cv_dec64_print(cv, ss, &sslen);
	len = snprintf(str, size, "%s", ss);
	break;
    case CGV_BOOL:
	if (cv->var_bool)
	    len = snprintf(str, size, "true");
	else
	    len = snprintf(str, size, "false");
	break;
    case CGV_REST:
	len = snprintf(str, size, "%s", cv->var_rest);
	break;
    case CGV_STRING: 
	len = snprintf(str, size, "%s", cv->var_string);
	break;
    case CGV_INTERFACE: 
	len = snprintf(str, size, "%s", cv->var_interface);
	break;
    case CGV_IPV4ADDR:
	len = snprintf(str, size, "%s", inet_ntoa(cv->var_ipv4addr));
	break;
    case CGV_IPV4PFX:
	len = snprintf(str, size, "%s/%u", 
		       inet_ntoa (cv->var_ipv4addr),
		       cv->var_ipv4masklen);
	break;
    case CGV_IPV6ADDR:
	if (inet_ntop(AF_INET6, &cv->var_ipv6addr, straddr, sizeof(straddr)) < 0){
	    fprintf(stderr, "inet_ntop: %s\n", strerror(errno));
	    return -1;
	}
	len = snprintf(str, size, "%s", straddr);
	break;
    case CGV_IPV6PFX:
	if (inet_ntop(AF_INET6, &cv->var_ipv6addr, straddr, sizeof(straddr)) < 0){
	    fprintf(stderr, "inet_ntop: %s\n", strerror(errno));
	    return -1;
	}
	len = snprintf(str, size, "%s/%u", straddr, cv->var_ipv6masklen);
	break;
    case CGV_MACADDR:
	len = snprintf(str, size, "%02x:%02x:%02x:%02x:%02x:%02x", 
		       (uint8_t)cv->var_macaddr[0],
		       (uint8_t)cv->var_macaddr[1],
		       (uint8_t)cv->var_macaddr[2],
		       (uint8_t)cv->var_macaddr[3],
		       (uint8_t)cv->var_macaddr[4],
		       (uint8_t)cv->var_macaddr[5]);

	break;
    case CGV_URL: /* <proto>://[<user>[:<passwd>]@]<addr>[/<path>] */
	len = snprintf(str, size, "%s://%s%s%s%s%s/%s", 	
		       cv->var_urlproto,
		       cv->var_urluser,
		       strlen(cv->var_urlpasswd)?":":"",
		       cv->var_urlpasswd,
		       strlen(cv->var_urluser)||strlen(cv->var_urlpasswd)?"@":"",
		       cv->var_urladdr,
		       cv->var_urlpath
	    );
	break;
    case CGV_UUID:
	uuid2str(cv->var_uuid, uuidstr, sizeof(uuidstr));
	len = snprintf(str, size, "%s", uuidstr);
	break;
    case CGV_TIME:
	time2str(cv->var_time, timestr, sizeof(timestr));
	len = snprintf(str, size, "%s", timestr);
	break;
    case CGV_VOID: /* N/A */
    case CGV_EMPTY:
	break;
    default:
	break;
    }
    return len;
}

/*! Print value of CLIgen variable using printf style formats into a new string
 *
 * @param[in]   cv  CLIgen variable
 * @retval      str Malloced string containing value. Should be freed after use.
 * @see cv2str
 */
char *
cv2str_dup(cg_var *cv)
{
    int   len;
    char *str;

    if (cv == NULL) 
	return NULL;
    if ((len = cv2str(cv, NULL, 0)) < 0)
	return NULL;
    if ((str = (char *)malloc(len+1)) == NULL)
	return NULL;
    memset(str, '\0', len+1);
    if ((cv2str(cv, str, len+1)) < 0){
	free(str);
	return NULL;
    }
    return str;
}

/*! Pretty print cligen variable value to a file
 *
 * @see cv2str which also prints a CV but to a string
 */
int
cv_print(FILE   *f, 
	 cg_var *cv)
{
    int  len = 0;
    char straddr[INET6_ADDRSTRLEN];
    char ss[64];
    int  sslen = sizeof(ss);
    char uuidstr[37];
    char timestr[28];

    switch (cv->var_type){
    case CGV_INT8:
	fprintf(f, "%" PRId8, cv->var_int8);
	break;
    case CGV_INT16:
	fprintf(f, "%" PRId16, cv->var_int16);
	break;
    case CGV_INT32:
	fprintf(f, "%" PRId32, cv->var_int32);
	break;
    case CGV_INT64:
	fprintf(f, "%" PRId64, cv->var_int64);
	break;
    case CGV_UINT8:
	fprintf(f, "%" PRIu8, cv->var_uint8);
	break;
    case CGV_UINT16:
	fprintf(f, "%" PRIu16, cv->var_uint16);
	break;
    case CGV_UINT32:
	fprintf(f, "%" PRIu32, cv->var_uint32);
	break;
    case CGV_UINT64:
	fprintf(f, "%" PRIu64, cv->var_uint64);
	break;
    case CGV_DEC64:
	cv_dec64_print(cv, ss, &sslen);
	fprintf(f, "%s", ss);
	break;
    case CGV_BOOL:
	if (cv->var_bool)
	    fprintf(f, "true");
	else
	    fprintf(f, "false");
	break;
    case CGV_REST:
	fprintf(f, "%s", cv->var_rest);
	break;
    case CGV_STRING: 
	if (cv->var_string)
	    fprintf(f, "\"%s\"", cv->var_string);
	break;
    case CGV_INTERFACE: 
	fprintf(f, "\"%s\"", cv->var_interface);
	break;
    case CGV_IPV4ADDR:
	fprintf(f, "%s", inet_ntoa(cv->var_ipv4addr));
	break;
    case CGV_IPV4PFX:
	fprintf(f, "%s/%d", inet_ntoa(cv->var_ipv4addr), cv->var_ipv4masklen);
	break;
    case CGV_IPV6ADDR:
	if (inet_ntop(AF_INET6, &cv->var_ipv6addr, straddr, sizeof(straddr)) < 0){
	    fprintf(stderr, "inet_ntop: %s\n", strerror(errno));
	    return -1;
	}
	fprintf(f, "%s", straddr);
	break;
    case CGV_IPV6PFX:
	if (inet_ntop(AF_INET6, &cv->var_ipv6addr, straddr, sizeof(straddr)) < 0){
	    fprintf(stderr, "inet_ntop: %s\n", strerror(errno));
	    return -1;
	}
	fprintf(f, "%s/%d", straddr, cv->var_ipv4masklen);
	break;
    case CGV_MACADDR:
	fprintf(f, "%02x:%02x:%02x:%02x:%02x:%02x", 
		cv->var_macaddr[0],
		cv->var_macaddr[1],
		cv->var_macaddr[2],
		cv->var_macaddr[3],
		cv->var_macaddr[4],
		cv->var_macaddr[5]);

	break;
    case CGV_URL: /* <proto>://[<user>[:<passwd>]@]<addr>[/<path>] */
	fprintf(f, "%s://%s%s%s%s%s/%s", 	
		cv->var_urlproto,
		cv->var_urluser,
		strlen(cv->var_urlpasswd)?":":"",
		cv->var_urlpasswd,
		strlen(cv->var_urluser)||strlen(cv->var_urlpasswd)?"@":"",
		cv->var_urladdr,
		cv->var_urlpath
	    );
	break;
    case CGV_UUID:
	uuid2str(cv->var_uuid, uuidstr, sizeof(uuidstr));
	fprintf(f, "%s", uuidstr);
	break;
    case CGV_TIME:
	time2str(cv->var_time, timestr, sizeof(timestr));
	fprintf(f, "%s", timestr);
	break;
    case CGV_VOID: /* N/A */
    case CGV_EMPTY: /* N/A */
	break;
    default:
	break;
    }
    return len;
}

/*! Print max value of a CLIgen variable type as string
 * @param[in]   type  CLIgen variable type
 * @param[out]  str   Max value printed in this string
 * @param[in]   size  Length of 'str'
 * @retval len  How many bytes printed
 * @see cvtype_max2str_dup
 * You can use str=NULL to get the expected length.
 * The number of (potentially if str=NULL) written bytes is returned.
 * @note DEC64 type does not work, need fraction-digits,... use cv_max_set
 * @see cv_max_set
 */
int
cvtype_max2str(enum cv_type type, 
	       char        *str, 
	       size_t       size)
{
    int  len = 0;

    switch (type){
    case CGV_INT8:
	len = snprintf(str, size, "%" PRId8, INT8_MAX);
	break;
    case CGV_INT16:
	len = snprintf(str, size, "%" PRId16, INT16_MAX);
	break;
    case CGV_INT32:
	len = snprintf(str, size, "%" PRId32, INT32_MAX);
	break;
    case CGV_INT64:
	len = snprintf(str, size, "%" PRId64, INT64_MAX);
	break;
    case CGV_UINT8:
	len = snprintf(str, size, "%" PRIu8, UINT8_MAX);
	break;
    case CGV_UINT16:
	len = snprintf(str, size, "%" PRIu16, UINT16_MAX);
	break;
    case CGV_UINT32:
	len = snprintf(str, size, "%" PRIu32, UINT32_MAX);
	break;
    case CGV_UINT64:
        len = snprintf(str, size, "%" PRIu64, UINT64_MAX);
        break;
    case CGV_DEC64:
	len = snprintf(str, size, "%" PRId64 ".0", INT64_MAX);
	break;
    case CGV_BOOL:
	len = snprintf(str, size, "true");
	break;
    default:
	break;
    }
    return len;
}

/*! Print max value of a CLIgen variable type as string
 *
 * The string should be freed after use.
 * @param[in]   type  CLIgen variable type
 * @retval      str   Malloced string containing value. Should be freed after use.
 * @see cvtype_max2str
 * @note DEC64 type does not work, need fraction-digits,... use cv_max_set
 * @see cv_max_set
 */
char *
cvtype_max2str_dup(enum cv_type type)
{
    int   len;
    char *str;

    if ((len = cvtype_max2str(type, NULL, 0)) < 0)
	return NULL;
    if ((str = (char *)malloc(len+1)) == NULL)
	return NULL;
    memset(str, '\0', len+1);
    if ((cvtype_max2str(type, str, len+1)) < 0){
	free(str);
	return NULL;
    }
    return str;
}

int
cv_max_set(cg_var *cv)
{
    switch (cv->var_type){
    case CGV_INT8:
	cv->var_int8 = INT8_MAX;
	break;
    case CGV_INT16:
	cv->var_int16 = INT16_MAX;
	break;
    case CGV_INT32:
	cv->var_int32 = INT32_MAX;
	break;
    case CGV_INT64:
	cv->var_int64 = INT64_MAX;
	break;
    case CGV_UINT8:
	cv->var_uint8 = UINT8_MAX;
	break;
    case CGV_UINT16:
	cv->var_uint16 = UINT16_MAX;
	break;
    case CGV_UINT32:
	cv->var_uint32 = UINT32_MAX;
	break;
    case CGV_UINT64:
	cv->var_uint64 = UINT64_MAX;
        break;
    case CGV_DEC64: /* assume fraction-digits is set to something */
	cv->var_uint64 = INT64_MAX;
	break;
    case CGV_BOOL:
	cv->var_bool = 1;
	break;
    default:
	break;
    }
    return 0;
}

int
cv_min_set(cg_var *cv)
{
    switch (cv->var_type){
    case CGV_INT8:
	cv->var_int8 = INT8_MIN;
	break;
    case CGV_INT16:
	cv->var_int16 = INT16_MIN;
	break;
    case CGV_INT32:
	cv->var_int32 = INT32_MIN;
	break;
    case CGV_INT64:
	cv->var_int64 = INT64_MIN;
	break;
    case CGV_UINT8:
	cv->var_uint8 = 0;
	break;
    case CGV_UINT16:
	cv->var_uint16 = 0;
	break;
    case CGV_UINT32:
	cv->var_uint32 = 0;
	break;
    case CGV_UINT64:
	cv->var_uint64 = 0;
        break;
    case CGV_DEC64: /* assume fraction-digits is set to something */
	cv->var_uint64 = INT64_MIN;
	break;
    case CGV_BOOL:
	cv->var_bool = 0;
	break;
    default:
	break;
    }
    return 0;
}

/*! Parse cv from string. 
 *
 * This function expects an initialized cv as created by cv_new() or
 * prepared by cv_reset(). 
 * The following is required of a cv before calling this function:
 *  - A type field. So that the parser knows how to parse the string
 *  - For decimal64 the fraction_digits (n) must be known.
 *
 * See also cv_parse() which has simpler error handling.
 * and cv_validate() where the cv is validated against a cligen object specification.
 *
 * @param[in]  str0    Input string. Example, number variable, str can be "7834" or "0x7634"
 * @param[in]  cv      cligen variable, as prepared by cv_reset()/cv_new()
 * @param[out] reason  If given, and if return value is 0, contains a malloced string
 *                 describing the reason why the validation failed. If given must be NULL.
 *
 * @retval -1  Error (fatal), with errno set to indicate error
 * @retval 0   Validation not OK, malloced reason is returned
 * @retval 1   Validation OK
 *
 * @code
 *  cg_var *cv = cv_new(CGV_STRING);
 *  char   *reason=NULL;
 *  if (cv_parse1("mystring", cv, &reason) < 0)
 *    cv_free(cv);
 *  free(reason);
 * @endcode
 */
int
cv_parse1(char   *str0,
	  cg_var *cv, 
	  char  **reason)
{
    int    retval = -1;
    char  *str;
    char  *mask;
    int    masklen = 0;
    int    i, j;
    size_t len;
    
    if (reason && (*reason != NULL)){
	fprintf(stderr, "reason must be NULL on calling\n");
	return -1;
    }
    if (str0 == NULL){
	if ((str = strdup("")) == NULL)
	    goto done;
    }
    else
	if ((str = strdup(str0)) == NULL)
	goto done;
    switch (cv->var_type) {
    case CGV_INT8:
	retval = parse_int8(str, &cv->var_int8, reason);
	break;
    case CGV_INT16:
	retval = parse_int16(str, &cv->var_int16, reason);
	break;
    case CGV_INT32:
	retval = parse_int32(str, &cv->var_int32, reason);
	break;
    case CGV_INT64:
	retval = parse_int64(str, &cv->var_int64, reason);
	break;
    case CGV_UINT8:
	retval = parse_uint8(str, &cv->var_uint8, reason);
	break;
    case CGV_UINT16:
	retval = parse_uint16(str, &cv->var_uint16, reason);
	break;
    case CGV_UINT32:
	retval = parse_uint32(str, &cv->var_uint32, reason);
	break;
    case CGV_UINT64:
	retval = parse_uint64(str, &cv->var_uint64, reason);
	break;
    case CGV_DEC64:
	retval = parse_dec64(str, cv_dec64_n_get(cv), &cv->var_dec64_i, reason);
	break;
    case CGV_BOOL:
	retval = parse_bool(str, &cv->var_bool, reason);
	break;
    case CGV_REST:
	j = 0; /* decode string, remove \<delimiters */
	/* XXX: unsure of the sanity of the use of strlen below, is it static or does the
	 * length change due to the loop logic? */
	len = strlen(str);
	for (i=0; i<len; i++)
	    if (str[i] != '\\')
		str[j++] = str[i];
	for (;j<len;j++)
	    str[j] = '\0';
	if (cv->var_rest)
	    free(cv->var_rest);
	if ((cv->var_rest = strdup(str)) == NULL)
	    goto done;
	retval = 1;
	break;
    case CGV_STRING:
	j = 0; /* decode string, remove \<delimiters */
	len = strlen(str);
	for (i=0;i<len;i++)
	    if (str[i] != '\\')
		str[j++] = str[i];
	for (;j<len;j++)
	    str[j] = '\0';
	if (cv->var_string){
	    free(cv->var_string);
	    cv->var_string = NULL;
	}
	if ((cv->var_string = strdup(str)) == NULL)
	    goto done;
	retval = 1;
	break;
    case CGV_INTERFACE:
	if (cv->var_interface)
	    free(cv->var_interface);
	if ((cv->var_interface = strdup(str)) == NULL)
	    goto done;
	retval = 1;
	break;
    case CGV_IPV4ADDR: 
	cv->var_ipv4masklen = 32;
	retval = parse_ipv4addr(str, &cv->var_ipv4addr, reason);
	break;
    case CGV_IPV6ADDR:
	cv->var_ipv6masklen = 128;
	retval = parse_ipv6addr(str, &cv->var_ipv6addr, reason);
	break;
    case CGV_IPV4PFX:
	if ((mask = strchr (str, '/')) == NULL){
	    retval = 0;
	    if (reason) 
		if ((*reason = cligen_reason("Mask-length missing")) == NULL)
		    retval = -1;
	    goto done;
	}
	*mask++ = '\0';
	if ((retval = parse_int32(mask, &masklen, reason)) <= 0)
	    break;
	if (masklen > 32 || masklen < 0) {
	    retval = 0;
	    if (reason) 
		if ((*reason = cligen_reason("Mask-length %s out of range:0 - 32", mask)) == NULL)
		    retval = -1;
	    goto done;
	}
	cv->var_ipv4masklen = masklen;
	retval = parse_ipv4addr(str, &cv->var_ipv4addr, reason);
	break;
    case CGV_IPV6PFX:
	if ((mask = strchr (str, '/')) == NULL){
	    retval = 0;
	    if (reason) 
		if ((*reason = cligen_reason("Mask-length missing")) == NULL)
		    retval = -1;
	    goto done;
	}
	*mask++ = '\0';
	if ((retval = parse_int32(mask, &masklen, reason)) <= 0)
	    break;
	if (masklen > 128 || masklen < 0) {
	    retval = 0;
	    if (reason &&
		(*reason = cligen_reason("Mask-length %s out of range: 0 - 128", mask))==NULL)
		retval = -1;
	    goto done;
	}
	cv->var_ipv6masklen = masklen;
	retval = parse_ipv6addr(str, &cv->var_ipv6addr, reason);
	break;
    case CGV_MACADDR:
	retval = parse_macaddr(str, cv->var_macaddr, reason);
	break;
    case CGV_URL: 
	retval = parse_url(str, cv, reason);
	break;
    case CGV_UUID:
	if (str2uuid(str, cv->var_uuid) < 0){
	    retval = 0;
	    if (reason &&
		(*reason = cligen_reason("Invalid uuid: %s", str))==NULL)
		retval = -1;
	    goto done;
	}
	retval = 1;
	break;
    case CGV_TIME:
	if (str2time(str, &cv->var_time) < 0){ /* XXX calloc gives true error */
	    retval = 0;
	    if (reason &&
		(*reason = cligen_reason("Invalid time: %s", str))==NULL)
		retval = -1;
	    goto done;
	}
	retval = 1;
	break;
    case CGV_VOID: /* N/A */
    case CGV_EMPTY:
    case CGV_ERR:
	retval = 0;
	if (reason) 
	    *reason = cligen_reason("Invalid variable");
	break;
    } /* switch */
  done:
    if (str)
	free (str);
    if (reason && *reason)
	assert(retval == 0); /* validation error only on reason */
    return retval;
}

/*! Parse cv from string. 
 *
 * This function expects an initialized cv as created by cv_new() or
 * prepared by cv_reset(). 
 * @note The following is required of a cv before calling this function:
 *  - A type field. So that the parser knows how to parse the string
 *  - For decimal64 the fraction_digits (n) must be known (cv_dec64_n_set())
 *
 * @see cv_parse1() with better error handling.
 * @see cv_validate() where cv is validated against a cligen object specification.
 *
 * Arguments:
 * @param[in]  str  Input string. Example, number variable, str can be "7834" or "0x7634"
 * @param[in]  cv   cligen variable, as prepared by cv_reset()/cv_new()
 * @retval -1 Error (fatal), or parsing error, printf error in stderr.
 * @retval  0 Parse OK
 *
 * @code
 *  cg_var *cv = cv_new(CGV_STRING);
 *  if (cv_parse("mystring", cv) < 0)
 *    cv_free(cv);
 * @endcode
 */
int
cv_parse(char   *str, 
	 cg_var *cv)
{
    int retval;
    char *reason = NULL;

    if ((retval = cv_parse1(str, cv, &reason)) < 0){
	fprintf(stderr, "cv parse error: %s\n", strerror(errno));
	return -1;
    }
    if (retval == 0){
	fprintf(stderr, "cv parse error: %s\n", reason);
	return -1;
    }
    return 0;
}

/*! Range interval check if a number is in an interval
 * @param[in] i      The number to check if it is in an interval
 * @param[in] cvlow  cv containing lower bound
 * @param[in] cvupp  cv containing upper bound
 * @param[in] type   Numeric type string, eg "int32"
 * @retval    0      i is outside [low,upper]
 * @retval    1      i is in [low,upper]
 * @note need to use macro trick ## to fix different types
*/
#define range_check(i, cvlow, cvupp, type)       \
    (((i) >= cv_##type##_get(cvlow)) && \
     ((i) <= cv_##type##_get(cvupp)))

/*! Error messsage for int violating ranges */
static int
outofrange(cg_var     *cv0,
	   cg_varspec *cs,
	   char      **reason)
{
    int     retval = -1;
    cbuf   *cb = NULL;
    cg_var *cv1;
    cg_var *cv2;
    int     i;

    if ((cb = cbuf_new()) == NULL)
	goto done;
    cprintf(cb, "Number ");
    cv2cbuf(cv0, cb);
    cprintf(cb, " out of range: ");
    for (i=0; i<cs->cgs_rangelen; i++){
	cv1 = cvec_i(cs->cgs_rangecvv_low, i);
	cv2 = cvec_i(cs->cgs_rangecvv_upp, i);
	if (i)
	    cprintf(cb, ", ");
	cv2cbuf(cv1, cb);
	cprintf(cb, " - ");
	cv2cbuf(cv2, cb);
    }
    if (reason && (*reason = strdup(cbuf_get(cb))) == NULL)
	goto done;
    if (cb)
	cbuf_free(cb);
    retval = 0;
 done:
    return retval;
}

/*! Error messsage for string violating string limits */
static int
outoflength(uint64_t    u64,
	    cg_varspec *cs,
	    char      **reason)
{
    int     retval = -1;
    cbuf   *cb = NULL;
    cg_var *cv1;
    cg_var *cv2;
    int     i;

    if ((cb = cbuf_new()) == NULL)
	goto done;
    cprintf(cb, "String length %" PRIu64 " out of range: ", u64);
    for (i=0; i<cs->cgs_rangelen; i++){
	cv1 = cvec_i(cs->cgs_rangecvv_low, i);
	cv2 = cvec_i(cs->cgs_rangecvv_upp, i);
	if (i)
	    cprintf(cb, ", ");
	cv2cbuf(cv1, cb);
	cprintf(cb, " - ");
	cv2cbuf(cv2, cb);
    }
    if (reason && (*reason = strdup(cbuf_get(cb))) == NULL)
	goto done;
    if (cb)
	cbuf_free(cb);
    retval = 0;
 done:
    return retval;
}

/*! Validate cligen variable cv using the spec in cs.
 *
 * @param[in]  h       CLIgen handle
 * @param[in]  cv      A cligen variable to validate. This is a correctly parsed cv.
 * @param[in]  cs      A cligen variable specification object that defines the cv.
 * @param[in]  cmd     Command for help and debug
 * @param[out] reason  If given, and if return value is 0, contains a malloced string
 *                      describing the reason why the validation failed.
 * @retval -1  Error (fatal), with errno set to indicate error
 * @retval 0   Validation not OK, malloced reason is returned. returned reason must be freed
 * @retval 1   Validation OK
 */
int
cv_validate(cligen_handle h,
	    cg_var       *cv, 
	    cg_varspec   *cs, 
	    char         *cmd,
	    char        **reason)
{
    int      retval = 1; /* OK */
    int32_t  i = 0;
    uint32_t u = 0;
    int64_t  i64;
    uint64_t u64;
    char    *str;
    int      ok;
    int      j;
    cg_var  *cv1;
    cg_var  *cv2;
    
    switch (cs->cgs_vtype){
    case CGV_INT8:
	if (!cs->cgs_rangelen)
	    break;
	i = cv_int8_get(cv);
	ok = 0; 	/* At least one should pass */
	for (j=0; j<cs->cgs_rangelen; j++){
	    cv1 = cvec_i(cs->cgs_rangecvv_low, j);
	    cv2 = cvec_i(cs->cgs_rangecvv_upp, j);
	    if ((ok = range_check(i, cv1, cv2, int8)) != 0)
		break;
	}
	if (!ok){
	    if (outofrange(cv, cs, reason) < 0)
		goto done;
	    retval = 0; /* No match */
	}
	break;
    case CGV_INT16:
	if (!cs->cgs_rangelen)
	    break;
	i = cv_int16_get(cv);
	ok = 0; 	/* At least one should pass */
	for (j=0; j<cs->cgs_rangelen; j++){
	    cv1 = cvec_i(cs->cgs_rangecvv_low, j);
	    cv2 = cvec_i(cs->cgs_rangecvv_upp, j);
	    if ((ok = range_check(i, cv1, cv2, int16)) != 0)
		break;
	}
	if (!ok){
	    if (outofrange(cv, cs, reason) < 0)
		goto done;
	    retval = 0; /* No match */
	}
	break;
    case CGV_INT32:
	if (!cs->cgs_rangelen)
	    break;
	i = cv_int32_get(cv);
	ok = 0; 	/* At least one should pass */
	for (j=0; j<cs->cgs_rangelen; j++){
	    cv1 = cvec_i(cs->cgs_rangecvv_low, j);
	    cv2 = cvec_i(cs->cgs_rangecvv_upp, j);
	    if ((ok = range_check(i, cv1, cv2, int32)) != 0)
		break;
	}
	if (!ok){
	    if (outofrange(cv, cs, reason) < 0)
		goto done;
	    retval = 0; /* No match */
	}
	break;
    case CGV_INT64:
	if (!cs->cgs_rangelen)
	    break;
	i64 = cv_int64_get(cv);
	ok = 0; 	/* At least one should pass */
	for (j=0; j<cs->cgs_rangelen; j++){
	    cv1 = cvec_i(cs->cgs_rangecvv_low, j);
	    cv2 = cvec_i(cs->cgs_rangecvv_upp, j);
	    if ((ok = range_check(i64, cv1, cv2, int64)) != 0)
		break;
	}
	if (!ok){
	    if (outofrange(cv, cs, reason) < 0)
		goto done;
	    retval = 0; /* No match */
	}
	break;
    case CGV_UINT8:
	if (!cs->cgs_rangelen)
	    break;
	u = cv_uint8_get(cv);
	ok = 0; 	/* At least one should pass */
	for (j=0; j<cs->cgs_rangelen; j++){
	    cv1 = cvec_i(cs->cgs_rangecvv_low, j);
	    cv2 = cvec_i(cs->cgs_rangecvv_upp, j);
	    if ((ok = range_check(u, cv1, cv2, uint8)) != 0)
		break;
	}
	if (!ok){
	    if (outofrange(cv, cs, reason) < 0)
		goto done;
	    retval = 0; /* No match */
	}
	break;
    case CGV_UINT16:
	if (!cs->cgs_rangelen)
	    break;
	u = cv_uint16_get(cv);
	ok = 0; 	/* At least one should pass */
	for (j=0; j<cs->cgs_rangelen; j++){
	    cv1 = cvec_i(cs->cgs_rangecvv_low, j);
	    cv2 = cvec_i(cs->cgs_rangecvv_upp, j);
	    if ((ok = range_check(u, cv1, cv2, uint16)) != 0)
		break;
	}
	if (!ok){
	    if (outofrange(cv, cs, reason) < 0)
		goto done;
	    retval = 0; /* No match */
	}
	break;
    case CGV_UINT32:
	if (!cs->cgs_rangelen)
	    break;
	u = cv_uint32_get(cv);
	ok = 0; 	/* At least one should pass */
	for (j=0; j<cs->cgs_rangelen; j++){
	    cv1 = cvec_i(cs->cgs_rangecvv_low, j);
	    cv2 = cvec_i(cs->cgs_rangecvv_upp, j);
	    if ((ok = range_check(u, cv1, cv2, uint32)) != 0)
		break;
	}
	if (!ok){
	    if (outofrange(cv, cs, reason) < 0)
		goto done;
	    retval = 0; /* No match */
	}
	break;
    case CGV_UINT64:
	if (!cs->cgs_rangelen)
	    break;
	u64 = cv_uint64_get(cv);
	ok = 0; 	/* At least one should pass */
	for (j=0; j<cs->cgs_rangelen; j++){
	    cv1 = cvec_i(cs->cgs_rangecvv_low, j);
	    cv2 = cvec_i(cs->cgs_rangecvv_upp, j);
	    if ((ok = range_check(u64, cv1, cv2, uint64)) != 0)
		break;
	}
	if (!ok){
	    if (outofrange(cv, cs, reason) < 0)
		goto done;
	    retval = 0; /* No match */
	}
	break;
    case CGV_DEC64:
	if (cv_dec64_n_get(cv) != cs->cgs_dec64_n){
	    if (reason)
		*reason = cligen_reason("Decimal 64 fraction-bits mismatch %d(cv) != %d(spec)",
					cv->var_dec64_n, cs->cgs_dec64_n);
	    retval = 0;
	}
	if (!cs->cgs_rangelen)
	    break;
	i64 = cv_int64_get(cv);
	ok = 0; 	/* At least one should pass */
	for (j=0; j<cs->cgs_rangelen; j++){
	    cv1 = cvec_i(cs->cgs_rangecvv_low, j);
	    cv2 = cvec_i(cs->cgs_rangecvv_upp, j);
	    if ((ok = range_check(i64, cv1, cv2, int64)) != 0)
		break;
	}
	if (!ok){
	    if (reason){
		if (outofrange(cv, cs, reason) < 0)
		    goto done;
	    }
	    retval = 0; /* No match */
	}
	break;
    case CGV_REST:
    case CGV_STRING:
	str = cv_string_get(cv);
	if (cs->cgs_regex != NULL){
	    char *regexp;
	    cv1 = NULL;
	    while ((cv1 = cvec_each(cs->cgs_regex, cv1)) != NULL){
		regexp = cv_string_get(cv1);
		if ((retval = match_regexp(h, str, regexp, cv_flag(cv1, V_INVERT))) < 0)
		    break;
		if (retval == 0){
		    if (reason)
			*reason = cligen_reason("\"%s\" is invalid input for cli command: %s",
						str, cmd);
		    retval = 0;
		    break; /* from the while */
		}
	    }
	    if (retval == 0)
		break; /* from the switch */
	}

	if (!cs->cgs_rangelen)	/* Skip range check */
	    break;
	u64 = strlen(str); /* size_t */
	ok = 0; 	/* At least one should pass */
	for (j=0; j<cs->cgs_rangelen; j++){
	    cv1 = cvec_i(cs->cgs_rangecvv_low, j);
	    cv2 = cvec_i(cs->cgs_rangecvv_upp, j);
	    if ((ok = range_check(u64, cv1, cv2, uint64)) != 0)
		break;
	}
	if (!ok){
	    if (outoflength(u64, cs, reason) < 0)
		goto done;
	    retval = 0; /* No match */
	}
	break;
    case CGV_ERR:
    case CGV_VOID:
	if (reason)
	    *reason = cligen_reason("Invalid cv");
	retval = 0;
	break;
    case CGV_BOOL:
    case CGV_INTERFACE:
    case CGV_IPV4ADDR: 
    case CGV_IPV6ADDR: 
    case CGV_IPV4PFX: 
    case CGV_IPV6PFX: 
    case CGV_MACADDR:
    case CGV_URL: 
    case CGV_UUID: 
    case CGV_TIME: 
    case CGV_EMPTY: 
	break;
    }
    if (reason && *reason)
	assert(retval == 0);
 done:
    return retval;
}

/*! Compare two CLIgen variables as strcmp(3)
 * @param[in]  cv1   CLIgen variable #1
 * @param[in]  cv2   CLIgen variable #2
 * @retval     0     equal
 * @retval    <0     cv1 is less than cv2
 * @retval    >0     cv1 is greater than cv2
 * XXX assume dec64_n are equal 
 */
int 
cv_cmp(cg_var *cv1, 
       cg_var *cv2)
{
    int n;

    /* Same type? */
    if(cv1->var_type != cv2->var_type)
	return cv1->var_type - cv2->var_type;

    switch (cv1->var_type) {
    case CGV_ERR:
	return 0;
    case CGV_INT8:
	return (cv1->var_int8 - cv2->var_int8);
    case CGV_INT16:
	return (cv1->var_int16 - cv2->var_int16);
    case CGV_INT32:
	return (cv1->var_int32 - cv2->var_int32);
    case CGV_INT64:
	return (cv1->var_int64 - cv2->var_int64);
    case CGV_UINT8:
	return (cv1->var_uint8 - cv2->var_uint8);
    case CGV_UINT16:
	return (cv1->var_uint16 - cv2->var_uint16);
    case CGV_UINT32:
	return (cv1->var_uint32 - cv2->var_uint32);
    case CGV_UINT64:
	return (cv1->var_uint64 - cv2->var_uint64);
    case CGV_DEC64:
	/* XXX assume dec64_n are equal */
	return (cv_dec64_i_get(cv1) - cv_dec64_i_get(cv2));
    case CGV_BOOL:
	return (cv1->var_bool - cv2->var_bool);
    case CGV_REST:
    case CGV_STRING:	
    case CGV_INTERFACE:  /* All strings have the same address */
	return strcmp(cv1->var_string, cv2->var_string);
    case CGV_IPV4ADDR:
	return memcmp(&cv1->var_ipv4addr, &cv2->var_ipv4addr,
		      sizeof(cv1->var_ipv4addr));
    case CGV_IPV4PFX:
	if ((n =  memcmp(&cv1->var_ipv4addr, &cv2->var_ipv4addr,
			 sizeof(cv1->var_ipv4addr))))
	    return n;
	return cv1->var_ipv4masklen - cv2->var_ipv4masklen;
    case CGV_IPV6ADDR:
	return memcmp(&cv1->var_ipv6addr, &cv2->var_ipv6addr,
		      sizeof(cv1->var_ipv6addr));
    case CGV_IPV6PFX:
	if ((n =  memcmp(&cv1->var_ipv6addr, &cv2->var_ipv6addr,
			 sizeof(cv1->var_ipv6addr))))
	    return n;
	return cv1->var_ipv6masklen - cv2->var_ipv6masklen;
    case CGV_MACADDR:
	return memcmp(&cv1->var_macaddr, &cv2->var_macaddr,
		      sizeof(cv1->var_macaddr));
	
    case CGV_URL:
	if ((n = strcmp(cv1->var_urlproto, cv2->var_urlproto)))
	    return n;
	if ((n = strcmp(cv1->var_urladdr, cv2->var_urladdr)))
	    return n;
	if ((n = strcmp(cv1->var_urlpath, cv2->var_urlpath)))
	    return n;
	if ((n = strcmp(cv1->var_urluser, cv2->var_urluser)))
	    return n;
	return  strcmp(cv1->var_urlpasswd, cv2->var_urlpasswd);
    case CGV_UUID:
	return memcmp(cv1->var_uuid, cv2->var_uuid, 16);
    case CGV_TIME:
	return memcmp(&cv1->var_time, &cv2->var_time, sizeof(struct timeval));
    case CGV_VOID: /* compare pointers */
	return (cv1->var_void == cv2->var_void);
    case CGV_EMPTY: /* Always equal */
	return 0;
    }

    return -1;
}

/*! Copy from one cv to a new cv.
 *
 * The new cv should have been be initialized, such as after cv_new() or
 * after cv_reset().
 * The new cv may involve duplicating strings, etc.

 * @retval 0   0n success, 
 * @retval -1  On error with errno set (strdup errors)
 */
int
cv_cp(cg_var *new, 
      cg_var *old)
{
    int retval = -1;

    memcpy(new, old, sizeof(*old)); 
    if (old->var_name)
	if ((new->var_name = strdup(old->var_name)) == NULL)
	    goto done;
    if (old->var_show)
	if ((new->var_show = strdup(old->var_show)) == NULL)
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
    case CGV_INTERFACE:  /* All strings have the same address */
	if (old->var_string)
	    if ((new->var_string = strdup(old->var_string)) == NULL) 
		goto done;
	break;
    case CGV_IPV4ADDR:
    case CGV_IPV4PFX:
    case CGV_IPV6ADDR:
    case CGV_IPV6PFX:
    case CGV_MACADDR:
	break;
    case CGV_URL:
	if (old->var_urlproto)
	    if ((new->var_urlproto = strdup(old->var_urlproto)) == NULL)
		goto done;
	if (old->var_urladdr)
	    if ((new->var_urladdr = strdup(old->var_urladdr)) == NULL)
		goto done;
	if (old->var_urlpath)
	    if ((new->var_urlpath = strdup(old->var_urlpath)) == NULL)
		goto done;
	if (old->var_urluser)
	    if ((new->var_urluser = strdup(old->var_urluser)) == NULL)
		goto done;
	if (old->var_urlpasswd)
	    if ((new->var_urlpasswd = strdup(old->var_urlpasswd)) == NULL)
		goto done;
	break;
    case CGV_UUID:
	break;
    case CGV_TIME:
	break;
    case CGV_VOID: /* cp pointer */
	new->var_void = old->var_void;
	break;
    case CGV_EMPTY:
	break;
    }
    retval = 0;
  done:
    return retval;
}

/*! Create a new cgv and copy the contents from the original. 
 *
 * This may involve duplicating strings, etc.
 * The new cv needs to be freed by cv_free().
 */
cg_var *
cv_dup(cg_var *old)
{
    cg_var *new;

    if ((new = cv_new(old->var_type)) == NULL)
	return NULL;
    if (cv_cp(new, old) < 0){
	cv_free(new);
	return NULL;
    }
    return new;
}

/*! Create new cligen variable. 
 *
 * @retval NULL  on error, error printed on stder
 * @retval cv    on success the malloc:ed cligen variable. Needs to be freed w cv_free()
 * @note returned cv needs to be freed with cv_free()
 * @note if type is CGV_DEC64, cv_dec64_n_set needs also be called
 * @see cvec_add  which adds a cv to an existing cvec
 * @see cv_free
 */
cg_var *
cv_new(enum cv_type type)
{
    cg_var             *cv;

    if ((cv = malloc(sizeof(cg_var))) == NULL)
	goto done;
    memset(cv, 0, sizeof(*cv));
    cv->var_type = type;
  done:
    return cv;

}

/*! Free pointers and resets a single CLIgen variable cv
 *
 * But does not free the cgv itself! 
 * the type is maintained after reset.
 */
int
cv_reset(cg_var *cv)
{
    enum cv_type type = cv->var_type;

    if (cv->var_name)
	free(cv->var_name);
    if (cv->var_show)
	free(cv->var_show);
    switch (cv->var_type) {
    case CGV_REST:
    case CGV_STRING:
    case CGV_INTERFACE:
	if (cv->var_string)
	    free(cv->var_string);	/* All strings have the same address */
	break;
    case CGV_URL:
	if (cv->var_urlproto)
	    free(cv->var_urlproto);
	if (cv->var_urladdr)
	    free(cv->var_urladdr);
	if (cv->var_urlpath)
	    free(cv->var_urlpath);
	if (cv->var_urluser)
	    free(cv->var_urluser);
	if (cv->var_urlpasswd)
	    free(cv->var_urlpasswd);
	break;
    case CGV_VOID: /* XXX: freeit ? */
	break;
    case CGV_EMPTY: 
	break;
    default:
	break;
    }
    memset(cv, 0, sizeof(*cv));
    cv->var_type = type;
    return 0;
}

/*! Free a single CLIgen variable (cv) AND frees the cv itself
 *
 * @param[in] cv     CLIgen variable
 * @see cv_new
 */
int
cv_free(cg_var *cv)
{
    if (cv) {
	cv_reset(cv);
	free(cv);
    }
    return 0;
}

/*! Return the alloced memory of a CLIgen variable
 */
size_t
cv_size(cg_var *cv)
{
    size_t sz = 0;

    sz += sizeof(struct cg_var);
    if (cv->var_name)
	sz += strlen(cv->var_name)+1;
    if (cv->var_show)
	sz += strlen(cv->var_show)+1;
    if (cv_isstring(cv->var_type) && cv->var_string)
	sz += strlen(cv->var_string)+1;
    return sz;
}
