# ============================================================================
# Desc    : automake for flame2/api/cxx
# Author  : Shawn Chin
# Date    : November 2012
# License : GNU Lesser General Public License
# Copyright (c) 2012 STFC Rutherford Appleton Laboratory
# Copyright (c) 2012 University of Sheffield
# ============================================================================
# $Id$

module_headers = \
  flame2.hpp

# Note: flame2.h is installed by the master Makefile.am

module_sources =
#module_sources = \
#  compatibility_manager.cpp \
#  flame_compat_c.cpp

# Header install path
library_includedir = $(pkgincludedir)/api/cxx

# Install all headers
library_include_HEADERS = $(module_headers)

noinst_LTLIBRARIES =
common_flags = @COMMON_CPPFLAGS@ @AM_CPPFLAGS@

if BUILD_DEBUG
noinst_LTLIBRARIES += lib_flame_api_cxx-dbg.la
lib_flame_api_cxx_dbg_la_CPPFLAGS = $(common_flags) @CPPFLAGS_DBG@
lib_flame_api_cxx_dbg_la_SOURCES  = $(module_headers) $(module_sources)
endif

if BUILD_PROD
noinst_LTLIBRARIES += lib_flame_api_cxx-prod.la
lib_flame_api_cxx_prod_la_CPPFLAGS = $(common_flags) @CPPFLAGS_PROD@
lib_flame_api_cxx_prod_la_SOURCES  = $(module_headers) $(module_sources)
endif

if BUILD_TEST
noinst_LTLIBRARIES += lib_flame_api_cxx-test.la
lib_flame_api_cxx_test_la_CPPFLAGS = $(common_flags) @CPPFLAGS_TEST@
lib_flame_api_cxx_test_la_SOURCES  = $(module_headers) $(module_sources)
endif

