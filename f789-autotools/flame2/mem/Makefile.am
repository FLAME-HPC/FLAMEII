# ============================================================================
# Desc    : automake for flame2/mem
# Author  : Shawn Chin
# Date    : October 2012
# License : GNU Lesser General Public License
# Copyright (c) 2012 STFC Rutherford Appleton Laboratory
# Copyright (c) 2012 University of Sheffield
# ============================================================================
# $Id$

noinst_LTLIBRARIES = lib_flame_mem.la

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

lib_flame_mem_la_LIBS     = @COMMON_LIBS@
lib_flame_mem_la_LDFLAGS  = @COMMON_LDFLAGS@
lib_flame_mem_la_CPPFLAGS = @COMMON_CPPFLAGS@ @AM_CPPFLAGS@
lib_flame_mem_la_SOURCES  = $(module_headers) $(module_sources)
