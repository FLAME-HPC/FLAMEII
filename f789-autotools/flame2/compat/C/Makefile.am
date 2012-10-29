# ============================================================================
# Desc    : automake for flame2/compat/C
# Author  : Shawn Chin
# Date    : October 2012
# License : GNU Lesser General Public License
# Copyright (c) 2012 STFC Rutherford Appleton Laboratory
# Copyright (c) 2012 University of Sheffield
# ============================================================================
# $Id$

noinst_LTLIBRARIES = lib_flame_compat_c.la

module_headers = \
  compatibility_manager.hpp \
  flame_compat_c.hpp \
  message_board_wrapper.hpp \
  message_iterator_wrapper.hpp

# Note: flame2.h is installed by the master Makefile.am

module_sources = \
  compatibility_manager.cpp \
  flame_compat_c.cpp

# Header install path
library_includedir = $(pkgincludedir)/compat/C

# Install all headers
library_include_HEADERS = $(module_headers)

lib_flame_compat_c_la_LIBS     = @COMMON_LIBS@
lib_flame_compat_c_la_LDFLAGS  = @COMMON_LDFLAGS@
lib_flame_compat_c_la_CPPFLAGS = @COMMON_CPPFLAGS@ @AM_CPPFLAGS@
lib_flame_compat_c_la_SOURCES  = $(module_headers) $(module_sources)
  
