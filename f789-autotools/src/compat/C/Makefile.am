# ============================================================================
# Desc    : automake for src/compat/C
# Author  : Shawn Chin
# Date    : October 2012
# License : GNU Lesser General Public License
# Copyright (c) 2012 STFC Rutherford Appleton Laboratory
# Copyright (c) 2012 University of Sheffield
# ============================================================================
# $Id$

noinst_LTLIBRARIES = lib_flame_compat_c.la

base_CPPFLAGS = -I $(top_srcdir)/headers -I$(top_srcdir)/src

lib_flame_compat_c_la_LIBS     = @COMMON_LIBS@
lib_flame_compat_c_la_LDFLAGS  = @COMMON_LDFLAGS@
lib_flame_compat_c_la_CPPFLAGS = @COMMON_CPPFLAGS@ $(base_CPPFLAGS)
lib_flame_compat_c_la_SOURCES  = \
  mb_api.cpp \
  mem_api.cpp
