# ===========================================================================
#
# SYNOPSIS
#
#   FLAME_BOOST_LIB_SUFFIX
#
# DESCRIPTION
#
#   Choose an library suffix used when linking to boost library. This will
#   affect choice of boost library used in latter calls to FLAME_BOOST_*.
#
#   The macro requires a preceding call to FLAME_BOOST_BASE.
#
#   This macro sets the following vars:
#
#     BOOST_LIB_SUFFIX
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
# $Id$

AC_DEFUN([FLAME_BOOST_LIB_SUFFIX],
[
  AC_ARG_WITH([boost-lib-suffix],
    [AS_HELP_STRING(
      [--with-boost-lib-suffix=BOOST_LIB_SUFFIX],
      [Choose an library suffix used when linking to boost library, e.g. --with-boost-lib-suffix=gcc-mt]
    )],
    [ax_boost_user_lib_suffix="$withval"],
    [ax_boost_user_lib_suffix=""]
  )

  # TODO : check that the suffix is valid? Is it possible without assuming the
  #        presence  of a specific boost lib?
  
  BOOST_LIB_SUFFIX="$ax_boost_user_lib_suffix"
])
