# ============================================================================
# Desc    : Master automake configuration
# Author  : Shawn Chin
# Date    : October 2012
# License : GNU Lesser General Public License
# Copyright (c) 2012 STFC Rutherford Appleton Laboratory
# Copyright (c) 2012 University of Sheffield
# ============================================================================
# $Id$

# Pass additional flags to make
ACLOCAL_AMFLAGS = -I m4

# Directories to search and build
SUBDIRS = \
  src/io \
  src/mb \
  src/mem \
  src/exe \
  src/sim \
  src/model \
  src/compat/C \
  headers/exceptions

# Library to install
lib_LTLIBRARIES = libflame2.la

# Flags to build that library
libflame2_la_SOURCES =
libflame2_la_LDFLAGS = @LIBFLAME_LDFLAGS@
libflame2_la_LIBADD  = \
  src/exe/lib_flame_exe.la \
  src/io/lib_flame_io.la  \
  src/mb/lib_flame_mb.la  \
  src/mem/lib_flame_mem.la  \
  src/sim/lib_flame_sim.la  \
  src/model/lib_flame_model.la \
  src/compat/C/lib_flame_compat_c.la

# C API header
include_HEADERS = headers/include/flame.h

# Exceptions 
nobase_include_HEADERS = \
  headers/exceptions/all.hpp \
  headers/exceptions/base.hpp \
  headers/exceptions/exe.hpp \
  headers/exceptions/io.hpp \
  headers/exceptions/mem.hpp
  

# Extra files to add into distribution
EXTRA_DIST = AUTHORS NEWS INSTALL README ChangeLog

if BUILD_TESTS

test:
	@(make && cd tests && make && ./run_tests --report_level=short --show_progress)
	
else

test:
	@echo "** TEST BUILD NOT ENABLED"
	@echo "   To build the tests, rerun ./configure with --enable-test-build"
	
endif


