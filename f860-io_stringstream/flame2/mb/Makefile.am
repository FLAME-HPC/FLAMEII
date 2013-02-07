# ============================================================================
# Desc    : automake for flame2/mb2
# Author  : Shawn Chin
# Date    : November 2012
# License : GNU Lesser General Public License
# Copyright (c) 2012 STFC Rutherford Appleton Laboratory
# Copyright (c) 2012 University of Sheffield
# ============================================================================
# $Id$

module_headers = \
  board_writer.hpp \
  client.hpp \
  message_board.hpp \
  message_board_manager.hpp \
  message_iterator.hpp \
  message_iterator_backend.hpp \
  message_iterator_backend_raw.hpp \
  proxy.hpp

module_sources = \
  board_writer.cpp \
  client.cpp \
  message_board.cpp

# Header install path
library_includedir = $(pkgincludedir)/mb

# Install all headers
library_include_HEADERS = $(module_headers)
  
noinst_LTLIBRARIES =
common_flags = @COMMON_CPPFLAGS@ @AM_CPPFLAGS@

if BUILD_DEBUG
noinst_LTLIBRARIES += lib_flame_mb-dbg.la
lib_flame_mb_dbg_la_CPPFLAGS = $(common_flags) @CPPFLAGS_DBG@
lib_flame_mb_dbg_la_SOURCES  = $(module_headers) $(module_sources)
endif

if BUILD_PROD
noinst_LTLIBRARIES += lib_flame_mb-prod.la
lib_flame_mb_prod_la_CPPFLAGS = $(common_flags) @CPPFLAGS_PROD@
lib_flame_mb_prod_la_SOURCES  = $(module_headers) $(module_sources)
endif

if BUILD_TEST
noinst_LTLIBRARIES += lib_flame_mb-test.la
lib_flame_mb_test_la_CPPFLAGS = $(common_flags) @CPPFLAGS_TEST@
lib_flame_mb_test_la_SOURCES  = $(module_headers) $(module_sources)
endif

