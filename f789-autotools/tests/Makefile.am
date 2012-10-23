# ============================================================================
# Desc    : automake configuration for /tests
# Author  : Shawn Chin
# Date    : October 2012
# License : GNU Lesser General Public License
# Copyright (c) 2012 STFC Rutherford Appleton Laboratory
# Copyright (c) 2012 University of Sheffield
# ============================================================================
# $Id$
if BUILD_TESTS

noinst_PROGRAMS = run_tests
run_tests_LDFLAGS = -lboost_unit_test_framework
run_tests_CPPFLAGS = @COMMON_CPPFLAGS@ @AM_CPPFLAGS@
run_tests_LDADD = $(top_srcdir)/libflame2.la
run_tests_SOURCES = ./run_tests.cpp

# Add tests for "mem"
run_tests_SOURCES += \
  mem/test_agent_memory.cpp \
  mem/test_memory_iterator.cpp \
  mem/test_memory_manager.cpp \
  mem/test_vector_wrapper.cpp

else

endif
