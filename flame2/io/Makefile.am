# ============================================================================
# Desc    : automake for flame2/io
# Author  : Shawn Chin
# Date    : October 2012
# License : GNU Lesser General Public License
# Copyright (c) 2012 STFC Rutherford Appleton Laboratory
# Copyright (c) 2012 University of Sheffield
# ============================================================================
# $Id$

module_headers = \
  io_interface.hpp \
  io_manager.hpp \
  io_xml_model.hpp \
  io_xml_agents.hpp \
  io_xml_variables.hpp \
  io_xml_element.hpp \
	plugins/io_sqlite_pop.hpp \
	plugins/io_hdf5_pop.hpp \
  plugins/io_xml_pop.hpp \
  plugins/xml_writer.hpp \
  plugins/io_cli_pop.hpp \
  plugins/io_csv_pop.hpp 

module_sources = \
  io_manager.cpp \
  io_xml_model.cpp \
  io_xml_agents.cpp \
  io_xml_variables.cpp \
  io_xml_element.cpp \
	plugins/io_sqlite_pop.cpp \
	plugins/io_hdf5_pop.cpp \
  plugins/io_xml_pop.cpp \
  plugins/xml_writer.cpp \
  plugins/io_cli_pop.cpp \
  plugins/io_csv_pop.cpp 

# Header install path
library_includedir = $(pkgincludedir)/io

# Install all headers
library_include_HEADERS = $(module_headers)

noinst_LTLIBRARIES =
common_flags = @COMMON_CPPFLAGS@ @AM_CPPFLAGS@

if BUILD_DEBUG
noinst_LTLIBRARIES += lib_flame_io-dbg.la
lib_flame_io_dbg_la_CPPFLAGS = $(common_flags) @CPPFLAGS_DBG@
lib_flame_io_dbg_la_SOURCES  = $(module_headers) $(module_sources)
endif

if BUILD_PROD
noinst_LTLIBRARIES += lib_flame_io-prod.la
lib_flame_io_prod_la_CPPFLAGS = $(common_flags) @CPPFLAGS_PROD@
lib_flame_io_prod_la_SOURCES  = $(module_headers) $(module_sources)
endif

if BUILD_TEST
noinst_LTLIBRARIES += lib_flame_io-test.la
lib_flame_io_test_la_CPPFLAGS = $(common_flags) @CPPFLAGS_TEST@
lib_flame_io_test_la_SOURCES  = $(module_headers) $(module_sources)
endif
