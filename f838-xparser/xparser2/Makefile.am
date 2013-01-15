# ============================================================================
# Desc    : automake for xparser2
# Author  : Shawn Chin
# Date    : Jan 2013
# License : GNU Lesser General Public License
# Copyright (c) 2013 STFC Rutherford Appleton Laboratory
# Copyright (c) 2013 University of Sheffield
# ============================================================================
# $Id: $
# xparser2 should be in a separate build that depends on libflame2.
# For now, we simply link to the required modules.

# do not include xparser2.cpp in this list
sources = \
  printer.cpp \
  file_generator.cpp \
  utils.cpp \
  codegen/gen_datastruct.cpp \
  codegen/gen_makefile.cpp \
  codegen/gen_file.cpp \
  codegen/gen_headerfile.cpp \
  codegen/gen_maincpp.cpp \
  codegen/gen_model.cpp \
  codegen/gen_agent.cpp \
  codegen/gen_message_registration.cpp \
  codegen/gen_agentfunc.cpp
 
headers = \
  utils.hpp \
  printer.hpp \
  file_generator.hpp \
  codegen/code_generator.hpp \
  codegen/gen_snippets.hpp \
  codegen/gen_datastruct.hpp \
  codegen/gen_makefile.hpp \
  codegen/gen_file.hpp \
  codegen/gen_headerfile.hpp \
  codegen/gen_maincpp.cpp \
  codegen/gen_model.hpp \
  codegen/gen_agent.hpp \
  codegen/gen_message_registration.hpp \
  codegen/gen_agentfunc.hpp

templates = \
  templates/Makefile.tmpl \
  templates/main_header.cpp.tmpl \
  templates/main_footer.cpp.tmpl

LDADD_LIST = \
  $(top_srcdir)/flame2/model/lib_flame_model \
  $(top_srcdir)/flame2/mem/lib_flame_mem \
  $(top_srcdir)/flame2/exe/lib_flame_exe \
  $(top_srcdir)/flame2/io/lib_flame_io \
  $(top_srcdir)/flame2/mb/lib_flame_mb

# Use test libs is available, else use debug libs. If both not available, use prod
LDADD_LIBFLAME = $(LDADD_LIST:=-@LOWEST_BUILD_SUFFIX@.la)

noinst_HEADERS = $(headers)
dist_pkgdata_DATA = $(templates)

noinst_LTLIBRARIES = lib_xparser2.la
lib_xparser2_la_CPPFLAGS = @COMMON_CPPFLAGS@ @CPPFLAGS_DBG@ @AM_CPPFLAGS@ -DPKGDATADIR="\"$(pkgdatadir)\""
lib_xparser2_la_SOURCES = $(sources)

bin_PROGRAMS = xparser2
xparser2_LIBS = @COMMON_LIBS@
xparser2_SOURCES = xparser2.cpp
xparser2_CPPFLAGS = @COMMON_CPPFLAGS@ @CPPFLAGS_DBG@ @AM_CPPFLAGS@ -DPKGDATADIR="\"$(pkgdatadir)\""
xparser2_LDFLAGS = @COMMON_LDFLAGS@ -rpath @BOOST_LIBDIR@
xparser2_LDADD = lib_xparser2.la $(LDADD_LIBFLAME) @COMMON_LIBS@


