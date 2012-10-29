# ============================================================================
# Desc    : automake for flame2/model
# Author  : Shawn Chin
# Date    : October 2012
# License : GNU Lesser General Public License
# Copyright (c) 2012 STFC Rutherford Appleton Laboratory
# Copyright (c) 2012 University of Sheffield
# ============================================================================
# $Id$

noinst_LTLIBRARIES = lib_flame_model.la

module_headers = \
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

lib_flame_model_la_LIBS     = @COMMON_LIBS@
lib_flame_model_la_LDFLAGS  = @COMMON_LDFLAGS@
lib_flame_model_la_CPPFLAGS = @COMMON_CPPFLAGS@ @AM_CPPFLAGS@
lib_flame_model_la_SOURCES  = $(module_headers) $(module_sources)
  


