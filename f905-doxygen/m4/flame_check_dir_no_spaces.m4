# ========================================================================
# SYNOPSIS
#
#   FLAME_CHECK_DIRS_NO_SPACES
#
# DESCRIPTION
#
#   libtool does not handle paths with spaces very well and chokes during
#   make install. To avoid the problem altogether we check all user specified
#   dirs and raise a warning if there are spaces in them.
#
# LICENSE
#
#  Copyright (c) 2013 Shawn Chin
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.
#
# $Id$

AC_DEFUN([FLAME_CHECK_DIRS_NO_SPACES],
[
  func_check_path_in_var()
  {
    eval _PATH_VAR="\$[]1"
    eval _CHECK_PATH="\$[$_PATH_VAR]"

    if echo "$_CHECK_PATH" | grep -q " "; then
      AC_MSG_ERROR(path defined with --$_PATH_VAR should not contain spaces)
    fi
  }
  
  AC_MSG_CHECKING(for spaces in paths)
  func_check_path_in_var "prefix"
  func_check_path_in_var "exec_prefix"
  func_check_path_in_var "bindir"
  func_check_path_in_var "sbindir"
  func_check_path_in_var "libexecdir"
  func_check_path_in_var "sysconfdir"
  func_check_path_in_var "sharedstatedir"
  func_check_path_in_var "localstatedir"
  func_check_path_in_var "libdir"
  func_check_path_in_var "includedir"
  func_check_path_in_var "oldincludedir"
  func_check_path_in_var "datarootdir"
  func_check_path_in_var "datadir"
  func_check_path_in_var "infodir"
  func_check_path_in_var "localedir"
  func_check_path_in_var "mandir"
  func_check_path_in_var "docdir"
  func_check_path_in_var "htmldir"
  func_check_path_in_var "dvidir"
  func_check_path_in_var "pdfdir"
  func_check_path_in_var "psdir"
  AC_MSG_RESULT(OK)
])
