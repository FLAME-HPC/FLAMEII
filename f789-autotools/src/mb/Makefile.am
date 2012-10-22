# ============================================================================
# Desc    : automake for src/mb
# Author  : Shawn Chin
# Date    : October 2012
# License : GNU Lesser General Public License
# Copyright (c) 2012 STFC Rutherford Appleton Laboratory
# Copyright (c) 2012 University of Sheffield
# ============================================================================
# $Id$

noinst_LTLIBRARIES = lib_flame_mb.la

base_CPPFLAGS = -I $(top_srcdir)/headers -I$(top_srcdir)/src

lib_flame_mb_la_LIBS     = @COMMON_LIBS@
lib_flame_mb_la_LDFLAGS  = @COMMON_LDFLAGS@
lib_flame_mb_la_CPPFLAGS = @COMMON_CPPFLAGS@ $(base_CPPFLAGS)
lib_flame_mb_la_SOURCES  = \
  board_writer.cpp \
  board_writer.hpp \
  client.cpp \
  client.hpp \
  mb_common.hpp \
  message_board.cpp \
  message_board.hpp \
  message_board_manager.cpp \
  message_board_manager.hpp \
  message.cpp \
  message.hpp \
  message_iterator_backend.cpp \
  message_iterator_backend.hpp \
  message_iterator_backend_raw.cpp \
  message_iterator_backend_raw.hpp \
  message_iterator.cpp \
  message_iterator.hpp \
  proxy.cpp \
  proxy.hpp \
  type_validator.hpp
