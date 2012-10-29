# ============================================================================
# Desc    : automake for src/exe
# Author  : Shawn Chin
# Date    : October 2012
# License : GNU Lesser General Public License
# Copyright (c) 2012 STFC Rutherford Appleton Laboratory
# Copyright (c) 2012 University of Sheffield
# ============================================================================
# $Id$

noinst_LTLIBRARIES = lib_flame_exe.la

module_headers = \
  agent_task.hpp \
  fifo_task_queue.hpp \
  io_task.hpp \
  message_board_task.hpp \
  scheduler.hpp \
  splitting_fifo_task_queue.hpp \
  task_interface.hpp \
  task_manager.hpp \
  task_queue_interface.hpp \
  task_splitter.hpp \
  worker_thread.hpp

module_sources = \
  agent_task.cpp \
  fifo_task_queue.cpp \
  io_task.cpp \
  message_board_task.cpp \
  scheduler.cpp \
  splitting_fifo_task_queue.cpp \
  task_manager.cpp \
  task_splitter.cpp \
  worker_thread.cpp

# Header install path
library_includedir = $(pkgincludedir)/exe

# Install all headers
library_include_HEADERS = $(module_headers)

lib_flame_exe_la_LIBS     = @COMMON_LIBS@
lib_flame_exe_la_LDFLAGS  = @COMMON_LDFLAGS@ 
lib_flame_exe_la_CPPFLAGS = @COMMON_CPPFLAGS@ @AM_CPPFLAGS@
lib_flame_exe_la_SOURCES  = $(module_headers) $(module_sources)
  


