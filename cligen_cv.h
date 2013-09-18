/*
  CVS Version: $Id: cligen_cv.h,v 1.3 2013/04/12 10:59:52 olof Exp $ 

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

  Private cg_var type definition. Dont include this to external apps, only 
  internal cligen
*/

#ifndef _CLIGEN_CV_H_
#define _CLIGEN_CV_H_

/*
 * Cligen Variable structure
 * cg_var / cv
 */
struct cg_var {
    int	var_type;   /* Type according to enum cv_type */
    char *var_name; /* Name of variable as appears in <name ...> in the syntax */
    char var_const; /* Keyword */
    char var_flag ; /* Application-specific flags, no semantics by cligen */
    union {
	char	varu_bool;
	int32_t	varu_int;
	int64_t	varu_long;
	char   *varu_string;
	char   *varu_interface;
	struct {
	    struct in_addr	varipv4_ipv4addr;
	    uint8_t		varipv4_masklen;
	} varu_ipv4addr;
	struct {
	    struct in6_addr	varipv6_ipv6addr;
	    uint8_t		varipv6_masklen;
	} varu_ipv6addr;

	char	               varu_macaddr[6];
	struct {
	    char               *varurl_proto;
	    char 	       *varurl_addr; 
	    char	       *varurl_path;
	    char	       *varurl_user;
	    char	       *varurl_passwd;
	} varu_url;
	uuid_t	               varu_uuid;
	struct timeval         varu_time;
    } u;
};

/*
 * Access macros (moved from .h)
 */
#define var_bool	u.varu_bool
#define var_int		u.varu_int
#define var_long	u.varu_long
#define var_string	u.varu_string
#define var_void	u.varu_string
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

#endif /* _CLIGEN_CV_H_ */
