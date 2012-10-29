# ============================================================================
# Desc    : automake for flame2/io
# Author  : Shawn Chin
# Date    : October 2012
# License : GNU Lesser General Public License
# Copyright (c) 2012 STFC Rutherford Appleton Laboratory
# Copyright (c) 2012 University of Sheffield
# ============================================================================
# $Id$

noinst_LTLIBRARIES = lib_flame_io.la

module_headers = \
  io_manager.hpp \
  io_xml_model.hpp \
  io_xml_pop.hpp

module_sources = \
  io_manager.cpp \
  io_xml_model.cpp \
  io_xml_pop.cpp

# Header install path
library_includedir = $(pkgincludedir)/io

# Install all headers
library_include_HEADERS = $(module_headers)

lib_flame_io_la_LIBS     = @COMMON_LIBS@
lib_flame_io_la_LDFLAGS  = @COMMON_LDFLAGS@ -lxml2
lib_flame_io_la_CPPFLAGS = @COMMON_CPPFLAGS@ @AM_CPPFLAGS@
lib_flame_io_la_SOURCES  = $(module_headers) $(module_sources)


