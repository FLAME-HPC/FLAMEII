# ============================================================================
# Desc    : automake configuration for /tests
# Author  : Shawn Chin
# Date    : October 2012
# License : GNU Lesser General Public License
# Copyright (c) 2012 STFC Rutherford Appleton Laboratory
# Copyright (c) 2012 University of Sheffield
# ============================================================================
# $Id$
if BUILD_TEST

noinst_PROGRAMS = run_tests
run_tests_LIBS = @COMMON_LIBS@
run_tests_LDFLAGS = @BOOST_UNIT_TEST_FRAMEWORK_LIB@ @COMMON_LDFLAGS@ -rpath @BOOST_LIBDIR@ -no-install
run_tests_CPPFLAGS = @COMMON_CPPFLAGS@ @AM_CPPFLAGS@ @CPPFLAGS_TEST@
run_tests_LDADD = $(top_srcdir)/libflame2-test.la @COMMON_LIBS@ 
run_tests_SOURCES = run_tests.cpp

#### Note: ####
#
# We list all source files here instead of using convenience libraries since
# independent compilation breaks automatic test registration of boost_unit_test
#

# Add tests for "mem"
run_tests_SOURCES += \
  mem/test_agent_memory.cpp \
  mem/test_memory_iterator.cpp \
  mem/test_memory_manager.cpp \
  mem/test_vector_wrapper.cpp

# Add tests for "exe"
run_tests_SOURCES += \
  exe/test_task_manager.cpp \
  exe/test_execution_module.cpp \
  exe/test_task_access_to_mb.cpp \
  exe/test_task_splitter.cpp

# Add tests for "mb"
run_tests_SOURCES += \
  mb/test_board_writer.cpp \
  mb/test_board_manager.cpp \
  mb/test_message_iterator.cpp \
  mb/test_board_client.cpp
  
# Add tests for "io"
run_tests_SOURCES += \
  io/test_io_manager.cpp \
  io/test_io_xml_model.cpp \
  io/test_io_xml_pop.cpp

# Add tests for "model"
run_tests_SOURCES += \
  model/test_model_validation.cpp \
  model/test_xgraph.cpp

# Add tests for "sim"
run_tests_SOURCES += \
  sim/test_sim_manager.cpp \
  sim/test_simulation.cpp

# Add tests for "api"
run_tests_SOURCES += \
  api/test_message_iterator_wrapper.cpp \
  api/test_flame_api.cpp

  
else
run: all
all:
	@echo "** TEST BUILD NOT ENABLED"
	@echo "   To build the tests, rerun ./configure with --enable-test-build"
endif
