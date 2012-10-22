# ============================================================================
# Desc    : automake for src/mem
# Author  : Shawn Chin
# Date    : October 2012
# License : GNU Lesser General Public License
# Copyright (c) 2012 STFC Rutherford Appleton Laboratory
# Copyright (c) 2012 University of Sheffield
# ============================================================================
# $Id$

noinst_LTLIBRARIES = lib_flame_mem.la

base_CPPFLAGS = -I $(top_srcdir)/headers -I$(top_srcdir)/src

lib_flame_mem_la_LIBS     = @COMMON_LIBS@
lib_flame_mem_la_LDFLAGS  = @COMMON_LDFLAGS@
lib_flame_mem_la_CPPFLAGS = @COMMON_CPPFLAGS@ $(base_CPPFLAGS)
lib_flame_mem_la_SOURCES  = \
  agent_memory.cpp \
  agent_memory.hpp \
  agent_shadow.cpp \
  agent_shadow.hpp \
  memory_iterator.cpp \
  memory_iterator.hpp \
  memory_manager.cpp \
  memory_manager.hpp \
  vector_wrapper.hpp
