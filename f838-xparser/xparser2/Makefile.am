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

sources = \
  xparser2.cpp \
  printer.cpp \
  file_generator.cpp \
  utils.cpp \
  codegen/gen_datastruct.cpp \
  codegen/gen_makefile.cpp \
  codegen/gen_file.cpp \
  codegen/gen_headerfile.cpp \
  codegen/gen_maincpp.cpp \
  codegen/gen_model.cpp
 
headers = \
  xparser2.hpp \
  printer.hpp \
  file_generator.hpp \
  utils.hpp \
  codegen/code_generator.hpp \
  codegen/gen_snippets.hpp \
  codegen/gen_datastruct.hpp \
  codegen/gen_makefile.hpp \
  codegen/gen_file.hpp \
  codegen/gen_headerfile.hpp \
  codegen/gen_maincpp.cpp \
  codegen/gen_model.hpp

templates = \
  templates/Makefile.tmpl \
  templates/main_header.cpp.tmpl \
  templates/main_footer.cpp.tmpl

LIBADD_LIST = \
  $(top_srcdir)/flame2/model/lib_flame_model \
  $(top_srcdir)/flame2/mem/lib_flame_mem \
  $(top_srcdir)/flame2/exe/lib_flame_exe \
  $(top_srcdir)/flame2/io/lib_flame_io \
  $(top_srcdir)/flame2/mb/lib_flame_mb

bin_PROGRAMS = xparser2
xparser2_LIBS = @COMMON_LIBS@
xparser2_LDFLAGS = @COMMON_LDFLAGS@ -rpath @BOOST_LIBDIR@
xparser2_CPPFLAGS = @COMMON_CPPFLAGS@ @CPPFLAGS_DBG@ @AM_CPPFLAGS@ -DPKGDATADIR="\"$(pkgdatadir)\""
xparser2_LDADD = $(LIBADD_LIST:=-dbg.la) @COMMON_LIBS@
xparser2_SOURCES = $(sources)
noinst_HEADERS = $(headers)
dist_pkgdata_DATA = $(templates)
