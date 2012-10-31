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
  flame2/io \
  flame2/mb \
  flame2/mem \
  flame2/exe \
  flame2/sim \
  flame2/model \
  flame2/compat/C \
  flame2/exceptions \
  utils

# Directories to include in the distribution
DIST_SUBDIRS = $(SUBDIRS) tests

# Library to install
lib_LTLIBRARIES = libflame2.la

# Flags to build that library
libflame2_la_SOURCES =
libflame2_la_LDFLAGS = @COMMON_LDFLAGS@
libflame2_la_LIBADD  = \
  flame2/exe/lib_flame_exe.la \
  flame2/io/lib_flame_io.la  \
  flame2/mb/lib_flame_mb.la  \
  flame2/mem/lib_flame_mem.la  \
  flame2/sim/lib_flame_sim.la  \
  flame2/model/lib_flame_model.la \
  flame2/compat/C/lib_flame_compat_c.la

# C API header
include_HEADERS = flame2/compat/C/flame2.h
# flame2 config header
nobase_include_HEADERS = flame2/config.hpp

# Extra files to add into distribution
EXTRA_DIST = AUTHORS NEWS INSTALL README ChangeLog \
  tests/io/models \
  tests/sim/models \
  tests/model/models

# Commands to run before packing up the distribution
dist-hook:
	@find $(distdir) -name ".svn" -print0 | xargs -0 rm -rf

if BUILD_TESTS

test:
	@( \
		find flame2 -name "*.cpp" | xargs grep flame2/config.hpp -L > test_cfg_incl ;\
		if test -s test_cfg_incl; then \
			echo "---------------------------------------------------------------" ;\
			echo "" ;\
			echo "OOppps. The following cpp files do no include flame2/config.hpp" ;\
			while read line; do echo "- $$line"; done < test_cfg_incl ;\
			echo "" ;\
			echo "Please add the following include statement to those files to " ;\
			echo "ensure that they are properly rebuilt when required:" ;\
			echo "#include \"flame2/config.hpp\"" ;\
			echo "" ;\
			echo "---------------------------------------------------------------" ;\
			rm test_cfg_incl ;\
			exit 1 ;\
		else \
			rm test_cfg_incl ;\
		fi \
	)
	@(make && cd tests && make && ./run_tests --report_level=short --show_progress)

else

test:
	@echo "** TEST BUILD NOT ENABLED"
	@echo "   To build the tests, rerun ./configure with --enable-test-build"

endif


