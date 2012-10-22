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

base_CPPFLAGS = -I $(top_srcdir)/headers -I$(top_srcdir)/src

lib_flame_exe_la_LIBS     = @COMMON_LIBS@
lib_flame_exe_la_LDFLAGS  = @COMMON_LDFLAGS@ 
lib_flame_exe_la_CPPFLAGS = @COMMON_CPPFLAGS@ $(base_CPPFLAGS)
lib_flame_exe_la_SOURCES  = \
  agent_task.cpp \
  agent_task.hpp \
  fifo_task_queue.cpp \
  fifo_task_queue.hpp \
  io_task.cpp \
  io_task.hpp \
  message_board_task.cpp \
  message_board_task.hpp \
  scheduler.cpp \
  scheduler.hpp \
  splitting_fifo_task_queue.cpp \
  splitting_fifo_task_queue.hpp \
  task_interface.hpp \
  task_manager.cpp \
  task_manager.hpp \
  task_queue_interface.hpp \
  task_splitter.cpp \
  task_splitter.hpp \
  worker_thread.cpp \
  worker_thread.hpp

