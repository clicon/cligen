/*
  CVS Version: $Id: cligen_var.c,v 1.37 2013/04/16 18:20:15 olof Exp $ 

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
/*! \file */ 
#include "cligen_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#ifdef HAVE_REGEX_H
#include <regex.h>
#endif

#include "cligen_var.h"
#include "cligen_cvec.h"
#include "cligen_gen.h"
#include "cligen_match.h"
#include "cligen_cv.h"
#include "getline.h"

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

/*! 
 * \brief Get name of cligen variable cv
 */
char *
cv_name_get(cg_var *cv)
{
    return cv->var_name;
}

/*! 
 * \brief allocate new string from original. Free previous string if existing.
 */
char *
cv_name_set(cg_var *cv, char *s0)
{
    char *s1 = NULL;

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

/*! 
 * \brief Get cv type
 */
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

/*!
 * \brief Get application-specific cv flag
 */
char
cv_flag(cg_var *cv, char mask)
{
    return cv->var_flag & mask;
}

/*!
 * \brief Clear application-specific cv flag 
 */
char
cv_flag_clr(cg_var *cv, char mask)
{
    return cv->var_flag ^= mask;
}

/*!
 * \brief Set application-specific cv flag 
 */
char
cv_flag_set(cg_var *cv, char mask)
{
    return cv->var_flag |= mask;
}

/*! 
 * \brief Get value of cv without specific type set
 */
void *
cv_value_get(cg_var *cv)
{
    return &cv->u;
}

/*! 
 * \brief Get boolean value of cv
 */
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

/*! 
 * \brief Get integer value of cv
 */
int32_t
cv_int_get(cg_var *cv)
{
    return ((cv)->u.varu_int);
}

int32_t
cv_int_set(cg_var *cv, int32_t x)
{
    return (cv->u.varu_int = x);
}

/*! 
 * \brief Get 64-bit integer value of cv
 */
int64_t
cv_long_get(cg_var *cv)
{
    return ((cv)->u.varu_long);
}

int64_t
cv_long_set(cg_var *cv, int64_t x)
{
    return (cv->u.varu_long = x);
}

/*! 
 * \brief Get pointer to cv string. 
 *
 * String can be modified in-line but must call _set function to reallocate.
 */
char *
cv_string_get(cg_var *cv)
{
    return ((cv)->u.varu_string);
}

/*! 
 * \brief allocate new string from original. Free previous string if existing.
 */
char *
cv_string_set(cg_var *cv, char *s0)
{
    char *s1 = NULL;

    /* Duplicate s0. Must be done before a free, in case s0 is part of the original */
    if (s0){
	if ((s1 = strdup(s0)) == NULL)
	    return NULL; /* error in errno */
    }
    if (cv->u.varu_string != NULL)
	free(cv->u.varu_string);
    cv->u.varu_string = s1;
    return s1; 
}

/*! 
 * \brief Get ipv4addr, pointer returned, can be used to set value.
 */
struct in_addr *
cv_ipv4addr_get(cg_var *cv)
{
    return &cv->u.varu_ipv4addr.varipv4_ipv4addr;
}

/*! 
 * \brief Get ipv4addr length of cv
 */
uint8_t
cv_ipv4masklen_get(cg_var *cv)
{
    return cv->u.varu_ipv4addr.varipv4_masklen;
}

/*! 
 * \brief Get ipv6addr, pointer returned, can be used to set value.
 */
struct in6_addr *
cv_ipv6addr_get(cg_var *cv)
{
    return &cv->u.varu_ipv6addr.varipv6_ipv6addr;
}

/*! 
 * \brief Get ipv6addr length of cv
 */
uint8_t
cv_ipv6masklen_get(cg_var *cv)
{
    return cv->u.varu_ipv6addr.varipv6_masklen;
}

/*! 
 * \brief  Returns a pointer to 6-byte mac-address array. 
 *
 * This can be used to set the address too
 */
char *
cv_mac_get(cg_var *cv)
{
    return cv->u.varu_macaddr;
}

/*! 
 * \brief Returns a pointer to uuid byte array. 
 * 
 * This can be used to set the uuid too.
 */
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

/*! 
 * \brief Returns a struct timeval by value.
 */
struct timeval
cv_time_get(cg_var *cv)
{
    return cv->u.varu_time;
}

/*
 * cv_time_set
 * Returns a struct timeval by value.
 */
struct timeval
cv_time_set(cg_var *cv, struct timeval t)
{
    cv->u.varu_time = t;
    return t;
}

/*! 
 * \brief  Get pointer to URL proto string. 
 *
 * String can be modified in-line but must call _set function to reallocate.
 */
char *
cv_urlproto_get(cg_var *cv)
{
    return (cv)->u.varu_url.varurl_proto;
}

char *
cv_urlproto_set(cg_var *cv, char *s0)
{
    char *s1 = NULL;

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

/*! 
 * \brief  Get pointer to URL address string. 
 *
 * String can be modified in-line but must call _set function to reallocate.
 */
char *
cv_urladdr_get(cg_var *cv)
{
    return (cv)->u.varu_url.varurl_addr;
}

/*
 * cv_urladdr_set  
 * malloc new string from original. Free previous string if existing.
 */
char *
cv_urladdr_set(cg_var *cv, char *s0)
{
    char *s1 = NULL;

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

/*! 
 * \brief  Get pointer to URL path string. 
 *
 * String can be modified in-line but must call _set function to reallocate.
 */
char *
cv_urlpath_get(cg_var *cv)
{
    return (cv)->u.varu_url.varurl_path;
}

/*
 * cv_urlpath_set  
 * malloc new string from original. Free previous string if existing.
 */
char *
cv_urlpath_set(cg_var *cv, char *s0)
{
    char *s1 = NULL;

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

/*! 
 * \brief  Get pointer to URL user string. 
 *
 * String can be modified in-line but must call _set function to reallocate.
 */
char *
cv_urluser_get(cg_var *cv)
{
    return (cv)->u.varu_url.varurl_user;
}

/*
 * cv_urluser_set  
 * malloc new string from original. Free previous string if existing.
 */
char *
cv_urluser_set(cg_var *cv, char *s0)
{
    char *s1 = NULL;

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

/*! 
 * \brief  Get pointer to URL passwd string. 
 *
 * String can be modified in-line but must call _set function to reallocate.
 */
char *
cv_urlpasswd_get(cg_var *cv)
{
    return (cv)->u.varu_url.varurl_passwd;
}

/*
 * cv_urlpasswd_set  
 * malloc new string from original. Free previous string if existing.
 */
char *
cv_urlpasswd_set(cg_var *cv, char *s0)
{
    char *s1 = NULL;

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

/*
 * cv Access methods end
 */


/*
 * -1 : Error (fatal), with errno set to indicate error
 *  0 : Validation not OK, malloced reason is returned
 *  1 : Validation OK
 */
static int
parse_int32(char *str, int32_t *val, char **reason)
{
    uint64_t i;
    char    *ep;
    int      retval = -1;

    errno = 0;
    i = strtoul(str, &ep, 0);
    if (str[0] == '\0' || *ep != '\0'){
	if (reason == NULL){
	    retval = 0;
	    goto done;
	}
	if ((*reason = cligen_reason("%s is not a number", str)) == NULL){
	    retval = -1;
	    goto done;
	}
	retval = 0;
	goto done;
    }
    if (errno == ERANGE){
	if (reason == NULL){
	    retval = 0;
	    goto done;
	}
	if ((*reason = cligen_reason("%s is out of range (type is int32_t)", str)) == NULL){
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

/*
 * parse_int64
 * return value:
 * -1 : fatal error 
 *  0 : parse error
 *  1 : ok
 */
int
parse_int64(char *str, int64_t *val, char **reason)
{
    uint64_t i;
    char    *ep;
    int      retval = -1;

    errno = 0;
    i = strtoull(str, &ep, 0);
    if (str[0] == '\0' || *ep != '\0'){
	if (reason == NULL){
	    retval = 0;
	    goto done;
	}
	if ((*reason = cligen_reason("%s is not a number", str)) == NULL){
	    retval = -1;
	    goto done;
	}
	retval = 0;
	goto done;
    }
    if (i == 18446744073709551615ULL && errno == ERANGE){ /* ULONG_MAX */
	if (reason == NULL){
	    retval = 0;
	    goto done;
	}
	if ((*reason = cligen_reason("%s is out of range (type is int64_t)", str)) == NULL){
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

int
parse_bool(char *str, char *val, char **reason)
{
    int i;
    int retval = 1;

    if (strcmp(str, "true") == 0 || strcmp(str, "on") == 0){
	i = 1;
    }
    else
	if (strcmp(str, "false") == 0 || strcmp(str, "off") == 0){
	    i = 0;
	}
	else{
	    if (reason)
		if ((*reason = cligen_reason("%s is not a boolean value", str)) == NULL){
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

    if ((retval = inet_pton(AF_INET, str, val)) < 0)
	goto done;
    if (retval == 0 && reason) 
	if ((*reason = cligen_reason("Invalid IPv4 address")) == NULL)
	    retval = -1;
  done:
    return retval;
}

int
parse_ipv6addr(char *str, struct in6_addr *val, char **reason)
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


/*
 * Own version of ether_aton(): 
 * parse string in colon hex notation and return a vector of chars.
 */
#define MACADDR_STRLEN 17 /* 6*sizeof("xx:")-1 */
static int
parse_macaddr(char *str, char addr[6], char **reason)
{
    char s[MACADDR_STRLEN+1], *s1, *s2;
    int i=0;
    int tmp; 
    int retval = -1;

    if ((str == NULL) || strlen(str) > MACADDR_STRLEN){
	retval = 0;
	if (reason && (*reason = cligen_reason("%s: Invalid macaddress", str)) == NULL)
	    retval = -1;
	goto done;
    }
    strncpy(s, str, MACADDR_STRLEN+1);
    s2 = s;
#ifdef HAVE_STRSEP
    while ((s1 = strsep(&s2, ":")) != NULL)
#else
	while ((s1 = strtok((i==0?s2:NULL),":")) != NULL)
#endif
	    {
		if (sscanf(s1, "%x", &tmp) < 1) 
		    return -1;
		if (tmp < 0 || 255 < tmp){
		    retval = 0;
		    if (reason && (*reason = cligen_reason("%s: Invalid macaddress", str)) == NULL)
			retval = -1;
		    goto done;
		}
		addr[i++] = (uint8_t)tmp;
	    }
    if (i!=6){
	retval = 0;
	if (reason && 
	    (*reason = cligen_reason("%s: Invalid macaddr", str)) == NULL){
	    retval = -1;
	    goto done;
	}
	goto done;
    }
    retval = 1; /* OK */
  done:
    return retval;
}

/* 
 * url may be changed destructively
 * URL syntax:
 * <proto>://[<user>[:<passwd>]@]<addr>[/<path>]
 */
static int
parse_url(char *url, cg_var *cv, char **reason)
{
    char    *str0;
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
    if (reason && (*reason = cligen_reason("%s: Invalid URL", url)) == NULL)
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
	if (cg_urlprotostr[proto] == NULL)
	    break;
	if (strcmp (str, cg_urlprotostr[proto]) == 0)
	    return proto;
    }
    return 0;
}

/* fmt needs to be char[37] */
int
uuid2str(uuid_t u, char *fmt, int len)
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

/* 
 * str2uuid
 * Translate uuid string on form f47ac10b-58cc-4372-a567-0e02b2c3d479 to uuid data structure.
 * in-string is 36 bytes + null termination (37 bytes in total).
 * out uuid is a 16 byte unsigned character array.
 * returns 0 if OK parses. -1 if not correct.
 */
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

static int 
todig(char c)
{
  if (c >= '0' && c <= '9') 
      return      c - '0';
  return -1;
}

/* 
 * tonum
 * n is nr of digits to read from s.
 * eg if s is "23456" and nr is 3, then return 234.
 * note: only positive 32-bit integers 
 * return -1 on error.
 */
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

/* 
 * str2time
 * Translate ISO 8601 date+time on the form 2008-09-21T18:57:21.003456 to a timeval structure.
 * T can be space.
 * in-string is upto 26 bytes + null termination (27 bytes in total).
 * out timestamp is a 4+4 integer struct timeval  
 * returns 0 if OK parses. -1 if not correct.
 * usec can be 1-6 digits. That is, .3 means 300000 usec.
 * NOTE : string coming in is expected to be UTC and we parse that into a tm struct and
 * then call mktime(). Problem is mktime() assumes tm is localtime. Therefore we must
 * adjust the timeval with the timezone to get it right. (Should be a mktime() that is
 * UTC).
 * XXX: You should be able to leave out less significant fields. That is, 2003 is
 * a time. But now you can only leave out usec.
 */
int
str2time(char *in, struct timeval *tv)
{
    int        retval = -1;
    int        i = 0;
    int        j;
    int        len;
    int        year, 
	month, 
	day, 
	hour, 
	min, 
	sec, 
	usec = 0;
    struct tm *tm; 
    time_t     t;

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
    free(tm);
    tv->tv_sec = t; 
    tv->tv_usec = usec;
    retval = 0;
done:
    return retval;
}

/* 
 * time2str
 * fmt needs to be 27 bytes.
 */
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


/*! 
 * \brief Translate (parse) a string to a CV type.
 */

enum cv_type
cv_str2type(char *str)
{
   if (strcmp(str, "number") == 0 || strcmp(str, "int") == 0)
    return CGV_INT;
  if (strcmp(str, "long") == 0)
     return CGV_LONG;
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
  return CGV_ERR;
}

/*! 
 * \brief Translate (print) a cv type to a static string.
 */
char *
cv_type2str(enum cv_type type)
{
    char *str = NULL;
    switch (type){
    case CGV_ERR:
	str="err";
	break;
    case CGV_INT:
	str="number";
	break;
    case CGV_LONG:
	str="long";
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

/*! 
 * \brief  Return length of cligen variable value (as encoded in binary)
 *
 * Special with strings that are only pointed to by the variable.
 * In that case the lengths of the strings pointed to are added, trailing
 * null included.
 */
int
cv_len(cg_var *cv)
{
    int len = 0;

    switch (cv->var_type){
    case CGV_INT:
	len = sizeof(cv->var_int);
	break;
    case CGV_LONG:
	len = sizeof(cv->var_long);
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
    default:
	break;
    }
    return len;
}


/*! 
 * \brief Print value of CLIgen variable using printf style formats.
 *
 * The value is printed in the string 'str' which has length 'size'.
 * You can use str=NULL to get the expected length.
 * The number of (potentially if str=NULL) written bytes is returned.
 * The value is printed as it would have been input, ie the reverse of
 * parsing.
 * Typically used by external code when transforming cgv:s.
 * Note, for strings, the length returned is _excluding_ the null byte, but the length
 * in supplied in the argument list is _including_ the null byte.
 */
int
cv2str(cg_var *cv, char *str, size_t size)
{
    int len = 0;
    char straddr[INET6_ADDRSTRLEN];

    switch (cv->var_type){
    case CGV_INT:
	len = snprintf(str, size, "%" PRId32, cv->var_int);
	break;
    case CGV_LONG:
	len = snprintf(str, size, "%" PRId64, cv->var_long);
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
		       cv->var_macaddr[0],
		       cv->var_macaddr[1],
		       cv->var_macaddr[2],
		       cv->var_macaddr[3],
		       cv->var_macaddr[4],
		       cv->var_macaddr[5]);

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
    case CGV_UUID:{  /* 37 */
	char uuidstr[37];
	uuid2str(cv->var_uuid, uuidstr, sizeof(uuidstr));
	len = snprintf(str, size, "%s", uuidstr);
	break;
    }
    case CGV_TIME:{  /* 27 */
	char timestr[27];
	time2str(cv->var_time, timestr, sizeof(timestr));
	len = snprintf(str, size, "%s", timestr);
	break;
    }
    case CGV_VOID: /* N/A */
	break;
    default:
	break;
    }
    return len;
}

/*! 
 * \brief Like cv2str, but allocate a string with right length.
 *
 * The string should be freed after use.
 */
char *
cv2str_dup(cg_var *cv)
{
    int   len;
    char *str;

    if ((len = cv2str (cv, NULL, 0)) < 0)
	return NULL;
    if ((str = (char *)malloc (len+1)) == NULL)
	return NULL;
    memset (str, '\0', len+1);
    if ((cv2str(cv, str, len+1)) < 0){
	free(str);
	return NULL;
    }
    return str;
}

/*! 
 * \brief Pretty print cligen variable value to a file
 *
 * Same as cv2str but on file
 */
int
cv_print(FILE *f, cg_var *cv)
{
    int len = 0;
    char straddr[INET6_ADDRSTRLEN];

    switch (cv->var_type){
    case CGV_INT:
	fprintf(f, "%" PRId32, cv->var_int);
	break;
    case CGV_LONG:
	fprintf(f, "%" PRId64, cv->var_long);
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
    case CGV_UUID:{
	char uuidstr[37];
	uuid2str(cv->var_uuid, uuidstr, sizeof(uuidstr));
	fprintf(f, "%s", uuidstr);
	break;
    }
    case CGV_TIME:{
	char timestr[27];
	time2str(cv->var_time, timestr, sizeof(timestr));
	fprintf(f, "%s", timestr);
	break;
    }
    case CGV_VOID: /* N/A */
	break;
    default:
	break;
    }
    return len;
}

/*! 
 * \brief parse cv from string. 
 *
 * An initialized cv is expected with a type field as created
 * by cv_new() or prepared by cv_reset().
 * Validate cligen variable cv using the spec in cs.
 *
 * Arguments:
 * IN    str   Input string. Example, number variable, str can be "7834" or "0x7634"
 * INOUT cgv   cligen variable, as prepared by cv_reset()/cv_new()
 * OUT reason: If given, and if return value is 0, contains a malloced string
 *              describing the reason why the validation failed.
 *
 * Return values:
 * -1 : Error (fatal), with errno set to indicate error
 *  0 : Validation not OK, malloced reason is returned
 *  1 : Validation OK
 *
 * Example:
 *  cg_var *cv = cv_new(CGV_STRING);
 *  char   *reason=NULL;
 *  cv_parse1("mystring", cv, &reason):
 *  free(reason);
 */
int
cv_parse1(char *str0, cg_var *cv, char **reason)
{
    int    retval = -1;
    char  *str;
    char  *mask;
    int    masklen;

    if ((str = strdup(str0)) == NULL)
	goto done;
    switch (cv->var_type) {
    case CGV_INT:
	retval = parse_int32(str, &cv->var_int, reason);
	break;
    case CGV_LONG:
	retval = parse_int64(str, &cv->var_long, reason);
	break;
    case CGV_BOOL:
	retval = parse_bool(str, &cv->var_bool, reason);
	break;
    case CGV_REST:
	if ((cv->var_rest = strdup(str)) == NULL)
	    goto done;
	retval = 1;
	break;
    case CGV_STRING:
	if ((cv->var_string = strdup(str)) == NULL)
	    goto done;
	retval = 1;
	break;
    case CGV_INTERFACE:
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
		if ((*reason = cligen_reason("Mask-length out of range: %s", mask)) == NULL)
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
		(*reason = cligen_reason("Mask-length out of range: %s", mask))==NULL)
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
	assert(retval == 0);
    return retval;
}

/*
 * cv_parse
 * parse cv from string. An initialized cv is expected with a type field as created
 * by cv_new() or prepared by cv_reset().
 * Validate cligen variable cv using the spec in cs.
 *
 * Arguments:
 * IN    str   Input string. Example, number variable, str can be "7834" or "0x7634"
 * INOUT cgv   cligen variable, as prepared by cv_reset()/cv_new()
 *
 * Return values:
 * -1 : Error (fatal), or parsing error, printf error in stderr.
 *  0 : Parse OK
 *
 * Example:
 *  cg_var *cv = cv_new(CGV_STRING);
 *  if (cv_parse("mystring", cv) < 0)
         cv_free(cv);

 */
int
cv_parse(char *str, cg_var *cv)
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

/*! 
 * \brief  Validate cligen variable cv using the spec in cs.
 *
 * Arguments:
 * IN cv:      A cligen variable to validate. This is a correctly parsed cv.
 * IN cs:      A cligen variable specification object that defines the cv.
 * OUT reason: If given, and if return value is 0, contains a malloced string
 *             describing the reason why the validation failed.
 *
 * Return values:
 * -1 : Error (fatal), with errno set to indicate error
 *  0 : Validation not OK, malloced reason is returned
 *  1 : Validation OK
 */
int
cv_validate(cg_var *cv, cg_varspec *cs, char **reason)
{
    int retval = 1; /* OK */
    long long i = 0;

    switch (cs->cgs_vtype){
    case CGV_INT:
	i = cv_int_get(cv);
    case CGV_LONG: /* fallthru */
	 /* Check range if specified */
	if (cs->cgs_vtype == CGV_LONG)
	    i = cv_long_get(cv);
	if (cs->cgs_range){
	    if (i < cs->cgs_range_low || i > cs->cgs_range_high) {
		if (reason)
		    *reason = cligen_reason("Number out of range: %i", i);
		retval = 0;
	    }
	}
    case CGV_STRING:
	if (cs->cgs_regex == NULL)
	    break;
	if ((retval = match_regexp(cv_string_get(cv), cs->cgs_regex)) < 0)
	    break;
	if (retval == 0){
	    if (reason)
		*reason = cligen_reason("regexp match fail: %s does not match %s",
					cv_string_get(cv), cs->cgs_regex);
	    retval = 0;
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
	assert(retval == 0);
    return retval;
}

/*! 
 * \brief Compare two cv:s
 *
 * Return 0 if match or a strcmp style return if it does not match.
 */
int 
cv_cmp(cg_var *cgv1, cg_var *cgv2)
{
    int n;

    /* Same type? */
    if(cgv1->var_type != cgv2->var_type)
	return cgv1->var_type - cgv2->var_type;

    switch (cgv1->var_type) {
    case CGV_INT:
	return (cgv1->var_int - cgv2->var_int);
    case CGV_LONG:
	return (cgv1->var_long - cgv2->var_long);
    case CGV_BOOL:
	return (cgv1->var_bool - cgv2->var_bool);
    case CGV_REST:
    case CGV_STRING:	
    case CGV_INTERFACE:  /* All strings have the same address */
	return strcmp(cgv1->var_string, cgv2->var_string);
    case CGV_IPV4ADDR:
	return memcmp(&cgv1->var_ipv4addr, &cgv2->var_ipv4addr,
		      sizeof(cgv1->var_ipv4addr));
    case CGV_IPV4PFX:
	if ((n =  memcmp(&cgv1->var_ipv4addr, &cgv2->var_ipv4addr,
			 sizeof(cgv1->var_ipv4addr))))
	    return n;
	return cgv1->var_ipv4masklen - cgv2->var_ipv4masklen;
    case CGV_IPV6ADDR:
	return memcmp(&cgv1->var_ipv6addr, &cgv2->var_ipv6addr,
		      sizeof (cgv1->var_ipv6addr));
    case CGV_IPV6PFX:
	if ((n =  memcmp(&cgv1->var_ipv6addr, &cgv2->var_ipv6addr,
			 sizeof(cgv1->var_ipv6addr))))
	    return n;
	return cgv1->var_ipv6masklen - cgv2->var_ipv6masklen;
    case CGV_MACADDR:
	return memcmp(&cgv1->var_macaddr, &cgv2->var_macaddr,
		      sizeof (cgv1->var_macaddr));
	
    case CGV_URL:
	if ((n = strcmp(cgv1->var_urlproto, cgv2->var_urlproto)))
	    return n;
	if ((n = strcmp(cgv1->var_urladdr, cgv2->var_urladdr)))
	    return n;
	if ((n = strcmp(cgv1->var_urlpath, cgv2->var_urlpath)))
	    return n;
	if ((n = strcmp(cgv1->var_urluser, cgv2->var_urluser)))
	    return n;
	return  strcmp(cgv1->var_urlpasswd, cgv2->var_urlpasswd);
    case CGV_UUID:
	return memcmp(cgv1->var_uuid, cgv2->var_uuid, 16);
    case CGV_TIME:
	return memcmp(&cgv1->var_time, &cgv2->var_time, sizeof(struct timeval));
    case CGV_VOID: /* compare pointers */
	return (cgv1->var_void == cgv2->var_void);
    }

    return -1;
}

/*! 
 * \brief Copy from one cv to a new cv.
 *
 * The new cv should have been be initialized, such as after cv_new() or
 * after cv_reset().
 * The new cv may involve duplicating strings, etc.
 * Returns:
 *  0  0n success, 
 *  -1 On error with errno set (strdup errors)
 */
int
cv_cp(cg_var *new, cg_var *old)
{
    int retval = -1;

    memcpy(new, old, sizeof(*old)); 
    if (old->var_name)
	if ((new->var_name = strdup(old->var_name)) == NULL)
	    goto done;
    switch (new->var_type) {
    case CGV_INT:
    case CGV_LONG:
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
    }
    retval = 0;
  done:
    return retval;
}

/*! 
 * \brief Create a new cgv and copy the contents from the original. 
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

/*! 
 * \brief Create new cligen variable. 
 *
 * Return:
 *  On success the malloc.ed cligen variable that needs to be freed after use.
 *  NULL on error, error printed on stderr
 *  See also cvec_add
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

/*! 
 * \brief Free pointers and resets a single CLIgen variable cv
 *
 * But does not free the cgv itself! 
 * the type is maintained after reset.
 */
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
	free(cgv->var_string);	/* All strings have the same address */
	break;
    case CGV_URL:
	free(cgv->var_urlproto);
	free(cgv->var_urladdr);
	free(cgv->var_urlpath);
	free(cgv->var_urluser);
	free(cgv->var_urlpasswd);
	break;
    case CGV_VOID: /* XXX: freeit ? */
	break;
    default:
	break;
    }
    memset(cgv, 0, sizeof(*cgv));
    cgv->var_type = type;
    return 0;
}

/*! 
 * \brief Free a single CLIgen variable (cv) AND frees the cv itself
 *
 * For new code
 */
int
cv_free(cg_var *cv)
{
    cv_reset(cv);
    free(cv);
    return 0;
}

/*
 * Makes a regexp check of <string> with <pattern>.
 * It is implicitly assumed that the match should be done at the beginning and
 * the end,
 * therefore, the pattern is prefixed with a ^, and postfixed with a $.
 * So, a match is made if <pattern> matches the beginning of <string>.
 * For example <pattern> "foobar" will not match <string> "foo".
 *
 * Returns -1 on error, 0 on no match, 1 on match
 * XXX: alternative if REGEX is not present on system??
 */
int 
match_regexp(char *string, char *pattern0)
{
#ifdef HAVE_REGEX_H
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
    strncat(pattern, "$",  sizeof(pattern)-len0-1);
    if (regcomp(&re, pattern, REG_NOSUB|REG_EXTENDED) != 0) 
	return(0);      /* report error */
    status = regexec(&re, string, (size_t) 0, NULL, 0);
    regfree(&re);
    if (status != 0) {
	regerror(status, &re, errbuf, sizeof(errbuf));
	return(0);      /* report error */
    }
#endif /* HAVE_REGEX_H */
    return(1);
}
