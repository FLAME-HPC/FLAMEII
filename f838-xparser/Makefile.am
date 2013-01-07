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
  flame2/mb \
  flame2/io \
  flame2/mem \
  flame2/exe \
  flame2/api \
  flame2/exceptions \
  flame2/model \
  flame2/sim \
  utils \
  xparser2



# Directories to include in the distribution
DIST_SUBDIRS = $(SUBDIRS) tests

# Libraries to link and install (populated by conditional block below)
lib_LTLIBRARIES =

# List of convenience libraries to link to form the final library.
# Note that the build type and file extensions are automatically appended
# depending on the build type
LIBADD_LIST = \
  flame2/mb/lib_flame_mb  \
  flame2/exe/lib_flame_exe \
  flame2/mem/lib_flame_mem \
  flame2/api/lib_flame_api \
  flame2/io/lib_flame_io \
  flame2/model/lib_flame_model \
  flame2/sim/lib_flame_sim  

  
if BUILD_DEBUG
lib_LTLIBRARIES += libflame2-dbg.la
libflame2_dbg_la_SOURCES = 
libflame2_dbg_la_LDFLAGSS = @COMMON_LDFLAGS@
libflame2_dbg_la_LIBADD = $(LIBADD_LIST:=-dbg.la) 
endif

if BUILD_PROD
lib_LTLIBRARIES += libflame2-prod.la
libflame2_prod_la_SOURCES = 
libflame2_prod_la_LDFLAGSS = @COMMON_LDFLAGS@
libflame2_prod_la_LIBADD = $(LIBADD_LIST:=-prod.la) 
endif

if BUILD_TEST
lib_LTLIBRARIES += libflame2-test.la
libflame2_test_la_SOURCES = 
libflame2_test_la_LDFLAGSS = @COMMON_LDFLAGS@
libflame2_test_la_LIBADD = $(LIBADD_LIST:=-test.la) 
endif

# API headers
include_HEADERS = \
  flame2/api/flame2.hpp \
  flame2/config.hpp

# Extra files to add into distribution
EXTRA_DIST = AUTHORS NEWS INSTALL README ChangeLog \
  tests/io/models \
  tests/sim/models \
  tests/model/models

# Install custom libtool along with distribution
bin_SCRIPTS = libtool

# Commands to run after installation complete
install-exec-hook:
	@mv $(DESTDIR)$(bindir)/libtool $(DESTDIR)$(bindir)/flame2-libtool

uninstall-hook:
	@rm $(DESTDIR)$(bindir)/flame2-libtool

# Commands to run before packing up the distribution
dist-hook:
	@find $(distdir) -name ".svn" -print0 | xargs -0 rm -rf

if BUILD_TEST

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
		fi ;\
		find flame2 -name "*.hpp" | xargs grep flame2/config.hpp -l > test_cfg_incl ;\
		if test -s test_cfg_incl; then \
			echo "---------------------------------------------------------------" ;\
			echo "" ;\
			echo "OOppps. The following hpp files include flame2/config.hpp" ;\
			while read line; do echo "- $$line"; done < test_cfg_incl ;\
			echo "" ;\
			echo "Please do not include config.hpp in header files as it contains " ;\
			echo "platform-specific definitions and is not installed along with" ;\
			echo "other header files." ;\
			echo "" ;\
			echo "---------------------------------------------------------------" ;\
			rm test_cfg_incl ;\
			exit 1 ;\
		fi ;\
		rm test_cfg_incl ;\
	)
	@(make && cd tests && make && ./run_tests --report_level=short --show_progress)

else

test:
	@echo "** TEST BUILD NOT ENABLED"
	@echo "   To build the tests, rerun ./configure with --enable-test-build"

endif


