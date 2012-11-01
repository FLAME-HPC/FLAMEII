# ===========================================================================
#
# SYNOPSIS
#
#   FLAME_BOOST_FILESYSTEM
#
# NOTE
#
#   This macro has adapted from AX_BOOST_FILESYSTEM.
#   See: http://www.gnu.org/software/autoconf-archive/ax_boost_filesystem.html
#
# DESCRIPTION
#
#   Test for Filesystem library from the Boost C++ libraries. The macro
#   requires a preceding call to FLAME_BOOST_BASE and FLAME_BOOST_LIB_SUFFIX
#   This macro calls:
#
#     AC_SUBST(BOOST_FILESYSTEM_LIB)
#
#   And sets:
#
#     HAVE_BOOST_FILESYSTEM
#
# LICENSE
#
#   Copyright (c) 2009 Thomas Porschberg <thomas@randspringer.de>
#   Copyright (c) 2009 Michael Tindal
#   Copyright (c) 2009 Roman Rybalko <libtorrent@romanr.info>
#   Copyright (c) 2012 Shawn Chin
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.
# 
# $Id$

AC_DEFUN([FLAME_BOOST_FILESYSTEM],
[
  want_boost="yes"  # We definitely want boost.
  base_lib="boost_filesystem"
  
  if test "x$BOOST_LIB_SUFFIX" = "x"; then
    ax_boost_user_filesystem_lib=""
  else
    ax_boost_user_filesystem_lib="${base_lib}-${BOOST_LIB_SUFFIX}"
  fi
    
#  AC_ARG_WITH([boost-filesystem],
#    [AS_HELP_STRING(
#      [--with-boost-filesystem=BOOST_FILESYSTEM_LIB],
#      [Choose specific boost filesystem library for the linker, e.g. --with-boost-filesystem=boost_filesystem-gcc-mt]
#    )],
#    [ax_boost_user_filesystem_lib="$withval"],
#    [ax_boost_user_filesystem_lib=""]
#  )
  
#  AC_ARG_WITH([boost-filesystem],
#  AS_HELP_STRING([--with-boost-filesystem@<:@=special-lib@:>@],
#                   [use the Filesystem library from boost - it is possible to specify a certain library for the linker
#                        e.g. --with-boost-filesystem=boost_filesystem-gcc-mt ]),
#        [
#        if test "$withval" = "no"; then
#      want_boost="no"
#        elif test "$withval" = "yes"; then
#            want_boost="yes"
#            ax_boost_user_filesystem_lib=""
#        else
#        want_boost="yes"
#    ax_boost_user_filesystem_lib="$withval"
#    fi
#        ],
#        [want_boost="yes"]
#  )

  if test "x$want_boost" = "xyes"; then
    AC_REQUIRE([AC_PROG_CC])
    CPPFLAGS_SAVED="$CPPFLAGS"
    CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"
    export CPPFLAGS

    LDFLAGS_SAVED="$LDFLAGS"
    LDFLAGS="$LDFLAGS $BOOST_LDFLAGS"
    export LDFLAGS

    LIBS_SAVED=$LIBS
    LIBS="$LIBS $BOOST_SYSTEM_LIB"
    export LIBS

    AC_CACHE_CHECK(whether the Boost::Filesystem library is available,
                  ax_cv_boost_filesystem,
                  [
                    AC_LANG_PUSH([C++])
                    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[@%:@include <boost/filesystem/path.hpp>]],
                                      [[using namespace boost::filesystem;
                                      path my_path( "foo/bar/data.txt" );
                                      return 0;]])],
                    ax_cv_boost_filesystem=yes, ax_cv_boost_filesystem=no)
                    AC_LANG_POP([C++])
                  ])
    if test "x$ax_cv_boost_filesystem" = "xyes"; then
        AC_DEFINE(HAVE_BOOST_FILESYSTEM,,[define if the Boost::Filesystem library is available])
        BOOSTLIBDIR=`echo $BOOST_LDFLAGS | sed -e 's/@<:@^\/@:>@*//'`
        if test "x$ax_boost_user_filesystem_lib" = "x"; then
            for libextension in `find $BOOSTLIBDIR -maxdepth 1 -name "libboost_filesystem*" \( -type f -o -type l \) | awk -F. '/libboost_filesystem\.(a|so|dylib)(\.|$)/{sub(/.*\/lib/, ""); print $[1]}' | sort -u`; do
            #for libextension in `ls $BOOSTLIBDIR/libboost_filesystem*.so* $BOOSTLIBDIR/libboost_filesystem*.dylib* $BOOSTLIBDIR/libboost_filesystem*.a* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^lib\(boost_filesystem.*\)\.\(dylib\|a\|so\).*$;\1;' | sed -e '1!G;h;$!d'` ; do
                ax_lib=${libextension}
                AC_CHECK_LIB($ax_lib, exit,
                              [BOOST_FILESYSTEM_LIB="-l$ax_lib"; AC_SUBST(BOOST_FILESYSTEM_LIB) link_filesystem="yes"; break],
                              [link_filesystem="no"])
            done
            if test "x$link_filesystem" != "xyes"; then
                for libextension in `find $BOOSTLIBDIR -maxdepth 1 -name "boost_filesystem*" \( -type f -o -type l \) | awk -F. '/libboost_filesystem\.(a|so|dylib)(\.|$)/{print $[1]}' | sort -u`; do
                #for libextension in `ls $BOOSTLIBDIR/boost_filesystem*.dll* $BOOSTLIBDIR/boost_filesystem*.a* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^\(boost_filesystem.*\)\.\(dll\|a\).*$;\1;' | sed -e '1!G;h;$!d'` ; do
                    ax_lib=${libextension}
                    AC_CHECK_LIB($ax_lib, exit,
                              [BOOST_FILESYSTEM_LIB="-l$ax_lib"; AC_SUBST(BOOST_FILESYSTEM_LIB) link_filesystem="yes"; break],
                              [link_filesystem="no"])
                done 
            fi
        else
            for ax_lib in $ax_boost_user_filesystem_lib boost_filesystem-$ax_boost_user_filesystem_lib; do
                AC_CHECK_LIB($ax_lib, exit,
                              [BOOST_FILESYSTEM_LIB="-l$ax_lib"; AC_SUBST(BOOST_FILESYSTEM_LIB) link_filesystem="yes"; break],
                              [link_filesystem="no"])
            done

        fi
        if test "x$ax_lib" = "x"; then
            AC_MSG_ERROR(Could not find a version of the library!)
        fi
        if test "x$link_filesystem" != "xyes"; then
          AC_MSG_ERROR(Could not link against $ax_lib !)
        fi
    fi

    CPPFLAGS="$CPPFLAGS_SAVED"
    LDFLAGS="$LDFLAGS_SAVED"
    LIBS="$LIBS_SAVED"
  fi
])
