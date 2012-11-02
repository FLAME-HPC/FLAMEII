# ============================================================================
# Desc    : automake for flame2/mb
# Author  : Shawn Chin
# Date    : October 2012
# License : GNU Lesser General Public License
# Copyright (c) 2012 STFC Rutherford Appleton Laboratory
# Copyright (c) 2012 University of Sheffield
# ============================================================================
# $Id$

noinst_LTLIBRARIES = lib_flame_mb.la

module_headers = \
  board_writer.hpp \
  client.hpp \
  mb_common.hpp \
  message_board.hpp \
  message_board_manager.hpp \
  message.hpp \
  message_iterator_backend.hpp \
  message_iterator_backend_raw.hpp \
  message_iterator.hpp \
  proxy.hpp \
  type_validator.hpp

module_sources = \
  board_writer.cpp \
  client.cpp \
  message_board.cpp \
  message_board_manager.cpp \
  message.cpp \
  message_iterator_backend.cpp \
  message_iterator_backend_raw.cpp \
  message_iterator.cpp \
  proxy.cpp

# Header install path
library_includedir = $(pkgincludedir)/mb

# Install all headers
library_include_HEADERS = $(module_headers)
  
lib_flame_mb_la_CPPFLAGS = @COMMON_CPPFLAGS@ @AM_CPPFLAGS@
lib_flame_mb_la_SOURCES  = $(module_headers) $(module_sources)



