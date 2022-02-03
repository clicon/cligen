/*
  CLI generator regular expressions

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

  This file includes support for regular expressions
 */
#include "cligen_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <regex.h>

#ifdef HAVE_LIBXML_XMLREGEXP_H
#include <libxml/xmlregexp.h>
#endif

#include "cligen_buf.h"
#include "cligen_cv.h"
#include "cligen_cvec.h"
#include "cligen_parsetree.h"
#include "cligen_pt_head.h"
#include "cligen_callback.h"
#include "cligen_object.h"
#include "cligen_handle.h"
#include "cligen_regex.h"

/*-------------------------- POSIX -------------------------*/

/*! Compile a regexp according to posix regexps
 * It is implicitly assumed that the match should be done at the beginning and
 * the end,
 * therefore, the pattern is prefixed with a ^, and postfixed with a $.
 * So, a match is made if <pattern> matches the beginning of <string>.
 * For example <pattern> "foobar" will not match <string> "foo".
 *
 * @param[in]   regexp  Regular expression string in XSD regex format
 * @param[out]  recomp  Compiled regular expression (malloc:d, should be freed)
 * @retval      1       OK
 * @retval      0       Invalid regular expression (syntax error?)
 * @retval     -1       Error
 */
int
cligen_regex_posix_compile(char  *regexp,
			   void **recomp)
{
    int      retval = -1;
    cbuf    *cb = NULL;
    regex_t *re = NULL;
    int      len0;

    len0 = strlen(regexp);
    if ((cb = cbuf_new()) == NULL)
	goto done;
    /* Check if prepended by ^ */
    if (len0 > 0 && regexp[0] == '^'){
	if (regexp[len0-1] == '$')
	    cprintf(cb, "%s", regexp);
	else if (len0 > 1 && regexp[1] == '(')
	    cprintf(cb, "%s)$", regexp);
	else
	    cprintf(cb, "%s$", regexp);
    }
    /* Check if terminated by $ only */
    else if (len0 > 0 && regexp[len0-1] == '$'){
	if (len0 > 1 && regexp[len0-2] == ')')
	    cprintf(cb, "^(%s", regexp);
	else
	    cprintf(cb, "^%s", regexp);
    }
    else /* Neither ^or $ */
	cprintf(cb, "^(%s)$", regexp);

    if ((re = malloc(sizeof(regex_t))) == NULL)
	goto done;
    memset(re, 0, sizeof(regex_t));
    if (regcomp(re, cbuf_get(cb), REG_NOSUB|REG_EXTENDED) != 0) {
	goto fail;
    }
    *recomp = re;
    re = NULL;
    retval = 1;
 done:
    if (re)
	free(re);
    if (cb)
	cbuf_free(cb);
    return retval;
 fail:
    retval = 0;
    goto done;
}

/*! Exec a regexp according to posix regexp
 * @param[in]   recomp  Compiled regular expression (malloc:d, should be freed)
 * @param[in]   string  Content string to match
 * @retval  1   Match
 * @retval  0   No match
 * @retval -1   Error
 */
int
cligen_regex_posix_exec(void *recomp,
			char *string)
{
    int      retval = -1;
    int      status;
    regex_t *re;
    char     errbuf[1024];
    
    re = (regex_t*)recomp;
    status = regexec(re, string, (size_t) 0, NULL, 0);
    if (status == 0) 
	retval = 1;
    else {
	regerror(status, re, errbuf, sizeof(errbuf)); /* XXX error is ignored */
	retval = 0;
    }
    return retval;
}

/*! Free compiled regular expression i
 * @param[in]   recomp  Compiled regular expression 
 */
int
cligen_regex_posix_free(void *recomp)
{
    if (recomp){
	regfree(recomp);
    }
    return 0;
}

/*-------------------------- Libxml2 -----------------------------------*/
/*! Compile a regexp according to libxml regex
 * @param[in]   regexp  Regular expression string in XSD regex format
 * @param[out]  recomp  Compiled regular expression (malloc:d, should be freed)
 * @retval      1       OK
 * @retval      0       Invalid regular expression (syntax error?)
 * @retval     -1       Error
 */
int
cligen_regex_libxml2_compile(char  *regexp0,
			     void **recomp)    
{
    int        retval = -1;
#ifdef HAVE_LIBXML_XMLREGEXP_H
    xmlChar   *regexp  = (xmlChar*)regexp0;
    xmlRegexp *xrp = NULL;
    
    if ((xrp = xmlRegexpCompile(regexp)) == NULL)
	goto fail;
    *recomp = xrp;
    retval = 1;
 done:
    return retval;
 fail:
    retval = 0;
    goto done;
#endif
    return retval;
}

/*! Exec a regexp according to libxml2
 * @param[in]   recomp  Compiled regular expression (malloc:d, should be freed)
 * @param[in]   string  Content string to match
 * @retval  1   Match
 * @retval  0   No match
 * @retval -1   Error
 */
int
cligen_regex_libxml2_exec(void *recomp,
			  char *string0)
{
    int        retval = -1;
#ifdef HAVE_LIBXML_XMLREGEXP_H
    xmlChar   *content = (xmlChar*)string0;
    xmlRegexp *xrp = (xmlRegexp *)recomp;
    
    /* Returns 1 if matches, 0 if not and a negative value in case of error */
    if ((retval = xmlRegexpExec(xrp, content)) < 0)
	goto done;
 done:
#endif
    return retval;
}

/*! Free compiled regular expression 
 * @param[in]   recomp  Compiled regular expression (malloc:d, should be freed)
 */
int
cligen_regex_libxml2_free(void *recomp)
{
#ifdef HAVE_LIBXML_XMLREGEXP_H
    if (recomp)
	xmlRegFreeRegexp(recomp);
#endif
    return 0;
}

/*-------------------------- Generic -----------------------------------*/
/*! Compilation of regular expression / pattern
 * @param[in]   h       Clicon handle
 * @param[in]   regexp  Regular expression string in XSD regex format
 * @param[out]  recomp  Compiled regular expression (malloc:d, should be freed)
 * @retval      1       OK
 * @retval      0       Invalid regular expression (syntax error?)
 * @retval     -1       Error
 */
int
cligen_regex_compile(cligen_handle h,
		     char         *regexp,
		     void        **recomp)
{
    int   retval = -1;

    if (cligen_regex_xsd(h) == 0) 
	retval = cligen_regex_posix_compile(regexp, recomp);
    else 
	retval = cligen_regex_libxml2_compile(regexp, recomp);
    return retval;
}

/*! Execution of (pre-compiled) regular expression / pattern
 * @param[in]  h   Clicon handle
 * @param[in]  recomp Compiled regexp
 */
int
cligen_regex_exec(cligen_handle h,
		  void         *recomp,
		  char         *string)
{
    int   retval = -1;

    if (cligen_regex_xsd(h) == 0) 
	retval = cligen_regex_posix_exec(recomp, string);
    else 
	retval = cligen_regex_libxml2_exec(recomp, string);
    return retval;
}

/*! Free compiled regular expression 
 * @param[in]  h       Clicon handle
 * @param[in]  recomp  Compiled regular expression 
 */
int
cligen_regex_free(cligen_handle h,
		  void         *recomp)
{
    int   retval = -1;

    if (cligen_regex_xsd(h) == 0) {
	retval = cligen_regex_posix_free(recomp);
	free(recomp);
    }
    else 
	retval = cligen_regex_libxml2_free(recomp);
    return retval;
}

/*! Makes a regexp check of <string> with <pattern>.
 *
 * @param[in] h       Clicon handle
 * @param[in] string  Content string to match
 * @param[in] pattern Pattern string to match
 * @param[in] invert  Invert match
 * @retval   -1       Error, 
 * @retval    0       No match
 * @retval    1       Match
 */
int 
match_regexp(cligen_handle h,
	     char         *string, 
	     char         *pattern,
	     int           invert)
{
    int   retval = -1;
    int   ret;
    void *re = NULL;

    if (string == NULL || pattern == NULL){
	errno = EINVAL;
	goto done;
    }
    if ((ret = cligen_regex_compile(h, pattern, &re)) < 0)
	goto done;
    if (ret == 0)
	goto fail;
    if ((ret = cligen_regex_exec(h, re, string)) < 0)
	goto done;
    if (invert)
	ret = !ret;
    if (ret == 0)
	goto fail;
    retval = 1;
 done:
    if (re != NULL)
	cligen_regex_free(h, re);
    return retval;
 fail:
    retval = 0;
    goto done;
}
