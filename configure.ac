# Autoconf script for CLIgen
# $Id: configure.ac,v 1.41 2013/07/02 05:47:50 olof Exp $
# To rebuild the `configure' script from this, execute the command
# 	autoconf
# in the directory containing this script.
#
#  Copyright (C) 2001-2011 Olof Hagsand
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

AC_INIT(cligen.h)
AC_CONFIG_HEADER(cligen_config.h)

# cligen versions spread to Makefile's (.so files)
AC_SUBST(CLIGEN_VERSION)
AC_SUBST(CLIGEN_VERSION_MAJOR)
AC_SUBST(CLIGEN_VERSION_MINOR)
AC_SUBST(CLIGEN_VERSION_PATCH)

CLIGEN_VERSION_MAJOR="3"
CLIGEN_VERSION_MINOR="4"
CLIGEN_VERSION_PATCH="0"
CLIGEN_VERSION="$CLIGEN_VERSION_MAJOR.$CLIGEN_VERSION_MINOR.${CLIGEN_VERSION_PATCH}_PRE1"

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
AC_SUBST(COPT)
AC_SUBST(OBJ_SUFFIX)
AC_SUBST(AR_SUFFIX)
AC_SUBST(SH_SUFFIX)
AC_SUBST(EXE_SUFFIX)
AC_SUBST(AR)
AC_SUBST(RANLIB)
#
AC_DEFINE(AUTOCONF)

echo "$host_cpu $host_vendor $host_os"
echo "yacc=$YACC lex=$LEX"

if test "$YACC" != "bison -y"; then
   AC_MSG_ERROR(CLIGEN does not find bison.  There are several problems with yacc and byacc. Please install bison.)
   # byacc leaks, may need byacc -P?
   echo "Please install bison."
   #  YACC="byacc -P"
   AC_DEFINE(USE_BYACC)
fi


# Check if explicit given a CC which overrides the default
AC_ARG_WITH(cc,
[  --with-cc=CC            Set C-compiler to CC],
  if test "$withval" = "gcc"; then
      CC=gcc 
  elif test "$withval" = "vcc" -o "$withval" = "cl"; then
      # vcc (MS visual c++) 
      CC=vcc
  else
      CC=$withval  #unknown
  fi
)

#
# Set options for each compiler
# NOTE: There is code for MS visualc++ but it is currently not supported
if test "$CC" = "vcc"; then # Microsoft VisualC++ 
      #  CFLAGS="${CFLAGS} -wccc-"    # Suppress "Cond. always true/false"
      CFLAGS=""
      CC=cl  # Actual visual cc name
      AC_DEFINE(WIN32)	
      LIBS="${LIBS} WS2_32.lib" # For GetMessage()
	#      LDFLAGS="-g"
      OBJ_SUFFIX=".obj"
      EXE_SUFFIX=".exe"
      AR_SUFFIX=".lib"
      SH_SUFFIX=".dll"
      AR="rm \$@; lib"
      COPT=""
      RANLIB="echo"  # Just a dummy, we don't have any replacement
else
      LIBS="${LIBS} -L." # 
      CFLAGS="-Wall -Werror -x c" 
      LDFLAGS=""
      COPT=-O2
      AC_CHECK_LIB(m, main)
      EXE_SUFFIX=""
      OBJ_SUFFIX=".o"
      AR_SUFFIX=".a"
      SH_SUFFIX=".so"
      AR="ar"
fi
AC_MSG_RESULT(compiler is $CC)	

AC_ARG_WITH(cflags,
[  --with-cflags=CFLAGS        Set non-default cflags],
   CFLAGS=$withval
)
AC_MSG_RESULT(CFLAGS is $CFLAGS)	

AC_ARG_WITH(copt,
[  --with-copt=COPT        Set to -g or -O2 (default) for example],
   COPT=$withval
)
AC_MSG_RESULT(COPT is $COPT)	

# Check for package options
AC_ARG_ENABLE(gl_select, [  --enable-gl-select         enable gl_select - Allow more file descriptors than stdin
[[default=no]]],[
  if test "$enableval" = no; then
    ac_enable_gl_select=no
  else
    ac_enable_gl_select=yes
  fi
  ],[ ac_enable_gl_select=no])

if test "$ac_enable_gl_select" = "yes"; then
   AC_DEFINE(GL_SELECT)
fi

AC_PATH_PROG(PDFLATEX, pdflatex)
AC_DEFINE_UNQUOTED(PROG_PDFLATEX, "$PDFLATEX")

# Experimental sub-modes
AC_DEFINE(CLIGEN_SUBMODE)

AC_CHECK_LIB(socket, socket)
AC_CHECK_FUNCS(strsep strverscmp)

AC_CHECK_HEADERS(regex.h termios.h)

AC_OUTPUT(Makefile)

