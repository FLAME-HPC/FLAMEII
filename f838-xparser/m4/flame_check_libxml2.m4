# ===========================================================================
#
# SYNOPSIS
#
#   FLAME_CHECK_LIBXML2
#
# DESCRIPTION
#
#   Test for existance of libxml2.
#
#   This macro calls:
#
#     AC_SUBST(LIBXML2_CPPFLAGS)
#     AC_SUBST(LIBXML2_LIBS)
#
#   And sets:
#
#     HAVE_LIBXML2
#
# LICENSE
#
#   Copyright (c) 2012 Shawn Chin
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.
#
# TODO
#
#   - Use AC_CACHE_CHECK to cache results of this macro.
#
# $Id$

AC_DEFUN([FLAME_CHECK_LIBXML2], [

  dnl ==== Allow users to specify libxml2 installation directory
  AC_ARG_WITH([libxml2],
    [AS_HELP_STRING(
      [--with-libxml2=LIBXML2_DIR],
      [Specify path to libxml2 installation]
    )],
  [
    if test -d "$withval"; then
      AC_PATH_PROG(XML2_CONFIG, xml2-config, no, "${withval}/bin")
    else
      AC_MSG_ERROR(--with-libxml2 expects a valid directory name)
    fi
  ],
  [AC_PATH_PROG(XML2_CONFIG, xml2-config, no)]
  )

  dnl ==== Locate xml2-config
  if test "$XML2_CONFIG" = "no"; then
    AC_MSG_ERROR([
** libxml2 not found. If you do have libxml2 installed, try specifying the
   installation path using --with-libxml2=LIBXML2_DIR
    ])
  fi

  dnl ==== Assume libxml2 is around
  LIBXML2_CPPFLAGS=`$XML2_CONFIG --cflags`  
  LIBXML2_LIBS=`$XML2_CONFIG --libs`

  dnl ==== Check if it actually works
  STORE_LIBS="$LIBS"
  STORE_CPPFLAGS="$CPPFLAGS"
  LIBS="${LIBXML2_LIBS} ${LIBS} "
  CPPFLAGS="${LIBXML2_CPPFLAGS} ${CPPFLAGS} "

  AC_LANG_PUSH([C])
  AC_CHECK_HEADER([libxml/parser.h],
  [
    AC_MSG_CHECKING([if libxml2 works])
    AC_LINK_IFELSE(
      [AC_LANG_PROGRAM(
        [[
          #include <stdlib.h>
          #include <libxml/parser.h>
        ]],
        [[
          xmlParseFile(NULL);
        ]],
      )],

      [AC_MSG_RESULT(yes)],
      [AC_MSG_ERROR([
** Failed to link test code with libxml2.
   The liker flags provided by xml2-config appears to be invalid. This might
   mean that the wrong copy of the xml2-config shell script has been found.
   Make sure your installation of libxml2 is complete then specify the
   installation path using --with-libxml2=LIBXML2_DIR
      ])]
    )
    
  ], [
    AC_MSG_ERROR([
** Failed to compile test code against libxml.
   The compile flags provided by xml2-config appears to be invalid and so the
   header files for libxml2 could not be found. This might mean that the wrong
   copy of the xml2-config shell script has been found.
   Make sure your installation of libxml2 is complete then specify the
   installation path using --with-libxml2=LIBXML2_DIR
    ])
  ])
  
  dnl ---- restore values
  CPPFLAGS="$STORE_CPPFLAGS"
  LIBS="$STORE_LIBS"
  AC_LANG_POP([C])

  dnl ==== hack
  # Note: libxml2 requires -lz for static linking but it is not exposed
  #       by xml2-config --libs
  # LIBXML2_LIBS="${LIBXML2_LIBS} -lz "
  
  dnl ==== Success. Apply side-effects.
  AC_DEFINE(HAVE_LIBXML2,,[define if the libxml2 is available])
  AC_SUBST(LIBXML2_CPPFLAGS)
  AC_SUBST(LIBXML2_LIBS)
  
]) dnl FLAME_CHECK_LIBXML2
