# ============================================================================
# Desc    : automake for flame2/compat/C
# Author  : Shawn Chin
# Date    : October 2012
# License : GNU Lesser General Public License
# Copyright (c) 2012 STFC Rutherford Appleton Laboratory
# Copyright (c) 2012 University of Sheffield
# ============================================================================
# $Id$

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

noinst_LTLIBRARIES =
common_flags = @COMMON_CPPFLAGS@ @AM_CPPFLAGS@

if BUILD_DEBUG
noinst_LTLIBRARIES += lib_flame_compat_c-dbg.la
lib_flame_compat_c_dbg_la_CPPFLAGS = $(common_flags) @CPPFLAGS_DBG@
lib_flame_compat_c_dbg_la_SOURCES  = $(module_headers) $(module_sources)
endif

if BUILD_PROD
noinst_LTLIBRARIES += lib_flame_compat_c-prod.la
lib_flame_compat_c_prod_la_CPPFLAGS = $(common_flags) @CPPFLAGS_PROD@
lib_flame_compat_c_prod_la_SOURCES  = $(module_headers) $(module_sources)
endif

if BUILD_TEST
noinst_LTLIBRARIES += lib_flame_compat_c-test.la
lib_flame_compat_c_test_la_CPPFLAGS = $(common_flags) @CPPFLAGS_TEST@
lib_flame_compat_c_test_la_SOURCES  = $(module_headers) $(module_sources)
endif

