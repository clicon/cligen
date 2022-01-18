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

 * This is an internal CLIgen header file 
 * Do not use these struct for external use, the internal structure may change. 
 * @see cligen_cv.h for external API use
*/

#ifndef _CLIGEN_CV_INTERNAL_H_
#define _CLIGEN_CV_INTERNAL_H_

/*
 * Constants
 */
/* Allow use of on/off as alternative truth values to true/false */
#define BOOL_TRUTH_ON_OFF

/* Allow use of enable/disable as alternative truth values to true/false */
#define BOOL_TRUTH_ENABLE_DISABLE

/*
 * Types
 */
/*
 * Cligen Variable structure
 * cg_var / cv
 * Note that a cv holds a value. The specification of a cv is a cg_varspec
 */
struct cg_var {
    enum cv_type var_type;  /* Type of variable appears in <name:type ...> */
    char        *var_name;  /* Name of variable appears in <name:type ...> */
    char        *var_show;  /* Show help-text, same as name or <name..show:<show>> */
    char         var_const; /* Set if the variable is a keyword */
    char         var_flag ; /* Application-specific flags, no semantics by cligen */
    union {
	uint8_t	 varu_bool;
	int8_t	 varu_int8;
	int16_t	 varu_int16;
	int32_t	 varu_int32;
	int64_t	 varu_int64;
	uint8_t	 varu_uint8;
	uint16_t varu_uint16;
	uint32_t varu_uint32;
	uint64_t varu_uint64;
	char    *varu_string;
	char    *varu_interface;
	struct {
	    int64_t  vardec64_i;    /* base number i in i x 10^-n */
	    uint8_t  vardec64_n;    /* fraction n in i x 10^-n */
	} varu_dec64;
	struct {
	    struct in_addr  varipv4_ipv4addr;
	    uint8_t	    varipv4_masklen;
	} varu_ipv4addr;
	struct {
	    struct in6_addr varipv6_ipv6addr;
	    uint8_t	    varipv6_masklen;
	} varu_ipv6addr;
	char	            varu_macaddr[6];
	struct {
	    char           *varurl_proto;
	    char 	   *varurl_addr; 
	    char	   *varurl_path;
	    char	   *varurl_user;
	    char	   *varurl_passwd;
	} varu_url;
	uuid_t	            varu_uuid;
	struct timeval      varu_time;
	void               *varu_void;
    } u;
};

/*
 * Access macros
 */
#define var_bool	u.varu_bool
#define var_int8	u.varu_int8
#define var_int16	u.varu_int16
#define var_int32	u.varu_int32
#define var_int64	u.varu_int64
#define var_uint8	u.varu_uint8
#define var_uint16	u.varu_uint16
#define var_uint32	u.varu_uint32
#define var_uint64	u.varu_uint64
#define var_dec64_i	u.varu_dec64.vardec64_i
#define var_dec64_n	u.varu_dec64.vardec64_n
#define var_string	u.varu_string
#define var_void	u.varu_void
#define var_rest	u.varu_string
#define var_interface	u.varu_string
#define var_macaddr	u.varu_macaddr
#define var_uuid	u.varu_uuid
#define var_time	u.varu_time
#define var_ipv4addr	u.varu_ipv4addr.varipv4_ipv4addr
#define var_ipv4masklen	u.varu_ipv4addr.varipv4_masklen
#define var_ipv6addr	u.varu_ipv6addr.varipv6_ipv6addr
#define var_ipv6masklen	u.varu_ipv6addr.varipv6_masklen
#define var_urlproto	u.varu_url.varurl_proto
#define var_urladdr	u.varu_url.varurl_addr
#define var_urlpath	u.varu_url.varurl_path
#define var_urluser	u.varu_url.varurl_user
#define var_urlpasswd	u.varu_url.varurl_passwd

#endif /* _CLIGEN_CV_INTERNAL_H_ */
