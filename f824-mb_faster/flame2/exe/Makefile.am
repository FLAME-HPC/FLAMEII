# ============================================================================
# Desc    : automake for flame2/exe
# Author  : Shawn Chin
# Date    : October 2012
# License : GNU Lesser General Public License
# Copyright (c) 2012 STFC Rutherford Appleton Laboratory
# Copyright (c) 2012 University of Sheffield
# ============================================================================
# $Id$

module_headers = \
  agent_task.hpp \
  fifo_task_queue.hpp \
  message_board_task.hpp \
  scheduler.hpp \
  splitting_fifo_task_queue.hpp \
  task_interface.hpp \
  task_manager.hpp \
  task_queue_interface.hpp \
  task_splitter.hpp \
  worker_thread.hpp \
  io_task.hpp
  
module_sources = \
  agent_task.cpp \
  fifo_task_queue.cpp \
  message_board_task.cpp \
  scheduler.cpp \
  splitting_fifo_task_queue.cpp \
  task_manager.cpp \
  task_splitter.cpp \
  worker_thread.cpp \
  io_task.cpp 

# Header install path
library_includedir = $(pkgincludedir)/exe

# Install all headers
library_include_HEADERS = $(module_headers)

noinst_LTLIBRARIES =
common_flags = @COMMON_CPPFLAGS@ @AM_CPPFLAGS@

if BUILD_DEBUG
noinst_LTLIBRARIES += lib_flame_exe-dbg.la
lib_flame_exe_dbg_la_CPPFLAGS = $(common_flags) @CPPFLAGS_DBG@
lib_flame_exe_dbg_la_SOURCES  = $(module_headers) $(module_sources)
endif

if BUILD_PROD
noinst_LTLIBRARIES += lib_flame_exe-prod.la
lib_flame_exe_prod_la_CPPFLAGS = $(common_flags) @CPPFLAGS_PROD@
lib_flame_exe_prod_la_SOURCES  = $(module_headers) $(module_sources)
endif

if BUILD_TEST
noinst_LTLIBRARIES += lib_flame_exe-test.la
lib_flame_exe_test_la_CPPFLAGS = $(common_flags) @CPPFLAGS_TEST@
lib_flame_exe_test_la_SOURCES  = $(module_headers) $(module_sources)
endif


