# ============================================================================
# Desc    : automake for flame2/model
# Author  : Shawn Chin
# Date    : October 2012
# License : GNU Lesser General Public License
# Copyright (c) 2012 STFC Rutherford Appleton Laboratory
# Copyright (c) 2012 University of Sheffield
# ============================================================================
# $Id$

module_headers = \
  agentfunction.hpp \
  dependency.hpp \
  model.hpp \
  task.hpp \
  xadt.hpp \
  xcondition.hpp \
  xfunction.hpp \
  xgraph.hpp \
  xioput.hpp \
  xmachine.hpp \
  xmessage.hpp \
  xmodel.hpp \
  xmodel_validate.hpp \
  xtimeunit.hpp \
  xvariable.hpp

module_sources = \
  agentfunction.cpp \
  dependency.cpp \
  model.cpp \
  task.cpp \
  xadt.cpp \
  xcondition.cpp \
  xfunction.cpp \
  xgraph.cpp \
  xioput.cpp \
  xmachine.cpp \
  xmessage.cpp \
  xmodel.cpp \
  xmodel_validate.cpp \
  xtimeunit.cpp \
  xvariable.cpp

# Header install path
library_includedir = $(pkgincludedir)/model

# Install all headers
library_include_HEADERS = $(module_headers)

noinst_LTLIBRARIES =
common_flags = @COMMON_CPPFLAGS@ @AM_CPPFLAGS@

if BUILD_DEBUG
noinst_LTLIBRARIES += lib_flame_model-dbg.la
lib_flame_model_dbg_la_CPPFLAGS = $(common_flags) @CPPFLAGS_DBG@
lib_flame_model_dbg_la_SOURCES  = $(module_headers) $(module_sources)
endif

if BUILD_PROD
noinst_LTLIBRARIES += lib_flame_model-prod.la
lib_flame_model_prod_la_CPPFLAGS = $(common_flags) @CPPFLAGS_PROD@
lib_flame_model_prod_la_SOURCES  = $(module_headers) $(module_sources)
endif

if BUILD_TEST
noinst_LTLIBRARIES += lib_flame_model-test.la
lib_flame_model_test_la_CPPFLAGS = $(common_flags) @CPPFLAGS_TEST@
lib_flame_model_test_la_SOURCES  = $(module_headers) $(module_sources)
endif

  


