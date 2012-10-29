# ============================================================================
# Desc    : automake for flame2/sim
# Author  : Shawn Chin
# Date    : October 2012
# License : GNU Lesser General Public License
# Copyright (c) 2012 STFC Rutherford Appleton Laboratory
# Copyright (c) 2012 University of Sheffield
# ============================================================================
# $Id$

noinst_LTLIBRARIES = lib_flame_sim.la

module_headers = \
  sim_manager.hpp \
  simulation.hpp

module_sources = \
  sim_manager.cpp \
  simulation.cpp

# Header install path
library_includedir = $(pkgincludedir)/sim

# Install all headers
library_include_HEADERS = $(module_headers)

lib_flame_sim_la_LIBS     = @COMMON_LIBS@
lib_flame_sim_la_LDFLAGS  = @COMMON_LDFLAGS@
lib_flame_sim_la_CPPFLAGS = @COMMON_CPPFLAGS@ @AM_CPPFLAGS@
lib_flame_sim_la_SOURCES  = $(module_headers) $(module_sources)


