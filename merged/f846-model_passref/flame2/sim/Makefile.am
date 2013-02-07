# ============================================================================
# Desc    : automake for flame2/sim
# Author  : Shawn Chin
# Date    : October 2012
# License : GNU Lesser General Public License
# Copyright (c) 2012 STFC Rutherford Appleton Laboratory
# Copyright (c) 2012 University of Sheffield
# ============================================================================
# $Id$

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

noinst_LTLIBRARIES =
common_flags = @COMMON_CPPFLAGS@ @AM_CPPFLAGS@

if BUILD_DEBUG
noinst_LTLIBRARIES += lib_flame_sim-dbg.la
lib_flame_sim_dbg_la_CPPFLAGS = $(common_flags) @CPPFLAGS_DBG@
lib_flame_sim_dbg_la_SOURCES  = $(module_headers) $(module_sources)
endif

if BUILD_PROD
noinst_LTLIBRARIES += lib_flame_sim-prod.la
lib_flame_sim_prod_la_CPPFLAGS = $(common_flags) @CPPFLAGS_PROD@
lib_flame_sim_prod_la_SOURCES  = $(module_headers) $(module_sources)
endif

if BUILD_TEST
noinst_LTLIBRARIES += lib_flame_sim-test.la
lib_flame_sim_test_la_CPPFLAGS = $(common_flags) @CPPFLAGS_TEST@
lib_flame_sim_test_la_SOURCES  = $(module_headers) $(module_sources)
endif
