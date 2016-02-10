#
#  Copyright (C) 2001-2016 Olof Hagsand
#
#  This file is part of CLIgen.
#
#  CLIgen is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2, or (at your option)
#  any later version.
#
#  CLIgen is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with CLIgen; see the file COPYING.  
#
# To rebuild the `configure' script from this, execute the command
# 	autoconf
# in the directory containing this script.

AC_INIT(cligen.h)

# Default CFLAGS unless set by environment.
: ${CFLAGS="-O2"}

AC_CONFIG_HEADER(cligen_config.h)

# cligen versions spread to Makefile's (.so files)
AC_SUBST(CLIGEN_VERSION)
AC_SUBST(CLIGEN_VERSION_MAJOR)
AC_SUBST(CLIGEN_VERSION_MINOR)
AC_SUBST(CLIGEN_VERSION_PATCH)

CLIGEN_VERSION_MAJOR="3"
CLIGEN_VERSION_MINOR="5"
CLIGEN_VERSION_PATCH="0"
CLIGEN_VERSION="$CLIGEN_VERSION_MAJOR.$CLIGEN_VERSION_MINOR.${CLIGEN_VERSION_PATCH}"

AC_MSG_RESULT(CLIGEN version is $CLIGEN_VERSION)

AC_CANONICAL_HOST
AC_PROG_CC()
AC_PROG_CPP
AC_PROG_YACC
AC_PROG_LEX
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
CFLAGS="${CFLAGS} -Wall" 
AC_CHECK_LIB(m, main)
EXE_SUFFIX=""
OBJ_SUFFIX=".o"
AR_SUFFIX=".a"
SH_SUFFIX=".so"
AR="ar"

AC_MSG_RESULT(CFLAGS is $CFLAGS)	
AC_MSG_RESULT(CPPFLAGS is $CPPFLAGS)	

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

AC_OUTPUT(Makefile)

