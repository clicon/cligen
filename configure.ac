# ***** BEGIN LICENSE BLOCK *****
# 
# Copyright (C) 2001-2018 Olof Hagsand
#
# This file is part of CLIgen.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# Alternatively, the contents of this file may be used under the terms of
# the GNU General Public License Version 2 or later (the "GPL"),
# in which case the provisions of the GPL are applicable instead
# of those above. If you wish to allow use of your version of this file only
# under the terms of the GPL, and not to allow others to
# use your version of this file under the terms of Apache License version 2, indicate
# your decision by deleting the provisions above and replace them with the 
# notice and other provisions required by the GPL. If you do not delete
# the provisions above, a recipient may use your version of this file under
# the terms of any one of the Apache License version 2 or the GPL.
#
# ***** END LICENSE BLOCK *****

#
# To rebuild the `configure' script from this, execute the command
# 	autoconf
# in the directory containing this script.

AC_INIT(cligen.h)

# Default CFLAGS unless set by environment.
: ${CFLAGS="-O2 -Wall"}
: ${INSTALLFLAGS="-s"}

AC_CONFIG_HEADER(cligen_config.h)

# cligen versions spread to Makefile's (.so files)
AC_SUBST(CLIGEN_VERSION)
AC_SUBST(CLIGEN_VERSION_MAJOR)
AC_SUBST(CLIGEN_VERSION_MINOR)
AC_SUBST(CLIGEN_VERSION_PATCH)

CLIGEN_VERSION_MAJOR="3"
CLIGEN_VERSION_MINOR="7"
CLIGEN_VERSION_PATCH="0"
CLIGEN_VERSION="$CLIGEN_VERSION_MAJOR.$CLIGEN_VERSION_MINOR.${CLIGEN_VERSION_PATCH}-PRE"

AC_MSG_RESULT(CLIGEN version is $CLIGEN_VERSION)

AC_CANONICAL_HOST
AC_PROG_CC()
AC_PROG_CPP
AC_PROG_YACC
AC_PROG_LEX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_SUBST(CC)
AC_SUBST(CFLAGS)
AC_SUBST(LDFLAGS)
AC_SUBST(INCLUDES)
AC_SUBST(LIBS)
AC_SUBST(OBJ_SUFFIX)
AC_SUBST(AR_SUFFIX)
AC_SUBST(SH_SUFFIX)
AC_SUBST(EXE_SUFFIX)
AC_SUBST(AR)
AC_SUBST(RANLIB)
AC_SUBST(INSTALL)
AC_SUBST(INSTALL_LIB)
AC_SUBST(INSTALL_DATA)
AC_SUBST(INSTALL_PROGRAM)
AC_SUBST(INSTALLFLAGS)
AC_SUBST(LN_S)
#

echo "$host_cpu $host_vendor $host_os"

if test "$LEX" = ":"; then
   AC_MSG_ERROR(CLIGEN does not find lex or flex.)
fi
if test "$YACC" != "bison -y"; then
   AC_MSG_ERROR(CLIGEN does not find bison.  There are several problems with yacc and byacc. Please install bison.)
   # byacc leaks, may need byacc -P?
fi

AC_MSG_RESULT(Compiler is $CC)	

#
# Set options for each compiler. Suffix is for WIN32
LIBS="${LIBS} -L." # 
LDFLAGS=""
CPPFLAGS="-DHAVE_CONFIG_H ${CPPFLAGS}"
AC_CHECK_LIB(m, main)
EXE_SUFFIX=""
OBJ_SUFFIX=".o"
AR_SUFFIX=".a"
SH_SUFFIX=".so"
AR="ar"

AC_MSG_RESULT(CFLAGS is $CFLAGS)	
AC_MSG_RESULT(CPPFLAGS is $CPPFLAGS)	
# Debug flag
AC_ARG_ENABLE(debug, AS_HELP_STRING([--enable-debug],[Build with debug symbols, default: no]),[
	  if test "$enableval" = no; then
	      ac_enable_debug=no
	  else	      
	      ac_enable_debug=yes
          fi
        ],
	[ ac_enable_debug=no])

AC_MSG_RESULT(debug is $ac_enable_debug)	
if test "$ac_enable_debug" = "yes"; then
   CFLAGS="-g -Wall"
   INSTALLFLAGS=""	
fi

# Enable BSD select semantics allowing multiple input file descriptors to be enabled at once,
# Not just stdin. Each extra fd requires a callback which is registered with cligen_regfd()])
AH_TEMPLATE([CLIGEN_REGFD], 
[Enable BSD select semantics allowing multiple input file descriptors to be enabled at once,
 Not just stdin. Each extra fd is registered with cligen_regfd()])
AC_DEFINE(CLIGEN_REGFD)

# Experimental sub-modes
AH_TEMPLATE([CLIGEN_SUBMODE], [Experimental CLIGEN sub-modes])
AC_DEFINE(CLIGEN_SUBMODE)

# 
AH_TEMPLATE([CLIGEN_COMPAT_INT], 
[This is for backward compatibility with int and long types that should be 
 replaced with int32 and int64 respectively. 
 Both in C-code (eg cv_int_get(), cv_long_get()) and CLIGEN parser (eg <x:int>, <x:long>)
])
AC_DEFINE(CLIGEN_COMPAT_INT)

AC_CHECK_LIB(socket, socket)
AC_CHECK_FUNCS(strsep strverscmp)

AC_CHECK_HEADERS(regex.h termios.h)

AH_BOTTOM([#include <cligen_custom.h>])

AC_OUTPUT([
	Makefile
	extras/rpm/Makefile
])

