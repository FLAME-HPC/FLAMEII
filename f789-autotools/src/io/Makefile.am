# ============================================================================
# Desc    : automake for src/io
# Author  : Shawn Chin
# Date    : October 2012
# License : GNU Lesser General Public License
# Copyright (c) 2012 STFC Rutherford Appleton Laboratory
# Copyright (c) 2012 University of Sheffield
# ============================================================================
# $Id$

noinst_LTLIBRARIES = lib_flame_io.la

lib_flame_io_la_LIBS     = @COMMON_LIBS@
lib_flame_io_la_LDFLAGS  = @COMMON_LDFLAGS@ -lxml2
lib_flame_io_la_CPPFLAGS = @COMMON_CPPFLAGS@ @AM_CPPFLAGS@
lib_flame_io_la_SOURCES  = \
  io_manager.cpp \
  io_manager.hpp \
  io_xml_model.cpp \
  io_xml_model.hpp \
  io_xml_pop.cpp \
  io_xml_pop.hpp
