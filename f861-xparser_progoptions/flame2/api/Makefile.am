# ============================================================================
# Desc    : automake for flame2/api
# Author  : Shawn Chin
# Date    : November 2012
# License : GNU Lesser General Public License
# Copyright (c) 2012 STFC Rutherford Appleton Laboratory
# Copyright (c) 2012 University of Sheffield
# ============================================================================
# $Id$

module_headers = \
  flame2.hpp \
  agent_api.hpp \
  message_iterator_wrapper.hpp

# Note: flame2.h is installed by the master Makefile.am

# compile dummy file so "ar" on OSX does not choke on empty archive
module_sources = dummy.cpp

#module_sources = \
#  compatibility_manager.cpp \
#  flame_compat_c.cpp

# Header install path
library_includedir = $(pkgincludedir)/api

# Install all headers
library_include_HEADERS = $(module_headers)

noinst_LTLIBRARIES =
common_flags = @COMMON_CPPFLAGS@ @AM_CPPFLAGS@

if BUILD_DEBUG
noinst_LTLIBRARIES += lib_flame_api-dbg.la
lib_flame_api_dbg_la_CPPFLAGS = $(common_flags) @CPPFLAGS_DBG@
lib_flame_api_dbg_la_SOURCES  = $(module_headers) $(module_sources)
endif

if BUILD_PROD
noinst_LTLIBRARIES += lib_flame_api-prod.la
lib_flame_api_prod_la_CPPFLAGS = $(common_flags) @CPPFLAGS_PROD@
lib_flame_api_prod_la_SOURCES  = $(module_headers) $(module_sources)
endif

if BUILD_TEST
noinst_LTLIBRARIES += lib_flame_api-test.la
lib_flame_api_test_la_CPPFLAGS = $(common_flags) @CPPFLAGS_TEST@
lib_flame_api_test_la_SOURCES  = $(module_headers) $(module_sources)
endif

