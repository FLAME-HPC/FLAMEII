# ============================================================================
# Desc    : automake for flame2/mem
# Author  : Shawn Chin
# Date    : October 2012
# License : GNU Lesser General Public License
# Copyright (c) 2012 STFC Rutherford Appleton Laboratory
# Copyright (c) 2012 University of Sheffield
# ============================================================================
# $Id$

module_headers = \
  agent_memory.hpp \
  agent_shadow.hpp \
  memory_iterator.hpp \
  memory_manager.hpp \
  vector_wrapper.hpp
  
module_sources = \
  agent_memory.cpp \
  agent_shadow.cpp \
  memory_iterator.cpp \
  memory_manager.cpp

# Header install path
library_includedir = $(pkgincludedir)/mem

# Headers to install
library_include_HEADERS = $(module_headers)

noinst_LTLIBRARIES =
common_flags = @COMMON_CPPFLAGS@ @AM_CPPFLAGS@

if BUILD_DEBUG
noinst_LTLIBRARIES += lib_flame_mem-dbg.la
lib_flame_mem_dbg_la_CPPFLAGS = $(common_flags) @CPPFLAGS_DBG@
lib_flame_mem_dbg_la_SOURCES  = $(module_headers) $(module_sources)
endif

if BUILD_PROD
noinst_LTLIBRARIES += lib_flame_mem-prod.la
lib_flame_mem_prod_la_CPPFLAGS = $(common_flags) @CPPFLAGS_PROD@
lib_flame_mem_prod_la_SOURCES  = $(module_headers) $(module_sources)
endif

if BUILD_TEST
noinst_LTLIBRARIES += lib_flame_mem-test.la
lib_flame_mem_test_la_CPPFLAGS = $(common_flags) @CPPFLAGS_TEST@
lib_flame_mem_test_la_SOURCES  = $(module_headers) $(module_sources)
endif
