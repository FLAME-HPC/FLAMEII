# ============================================================================
# Desc    : Master automake configuration
# Author  : Shawn Chin
# Date    : October 2012
# License : GNU Lesser General Public License
# Copyright (c) 2012 STFC Rutherford Appleton Laboratory
# Copyright (c) 2012 University of Sheffield
# ============================================================================
# $Id: $

# Pass additional flags to make
ACLOCAL_AMFLAGS = -I m4

# Directories to search
SUBDIRS = src/exe src/io src/mb src/mem src/model

# header files
include_HEADERS = \
  $(top_srcdir)/headers/exceptions/all.hpp \
  $(top_srcdir)/headers/exceptions/base.hpp \
  $(top_srcdir)/headers/exceptions/exe.hpp \
  $(top_srcdir)/headers/exceptions/io.hpp \
  $(top_srcdir)/headers/exceptions/mem.hpp
  
# Library to install
lib_LTLIBRARIES = libflame2.la

# Flags to build that library
libflame2_la_SOURCES =
libflame2_la_LDFLAGS =
libflame2_la_LIBADD  = \
  src/exe/lib_flame_exe.la \
  src/io/lib_flame_io.la  \
  src/mb/lib_flame_mb.la  \
  src/mem/lib_flame_mem.la  \
  src/model/lib_flame_model.la

# TODO: conditionally include Compatibily code
SUBDIRS += src/compat/C
libflame2_la_LIBADD  += src/compat/C/lib_flame_compat_c.la


# Extra files to add into distribution
EXTRA_DIST = AUTHORS NEWS INSTALL README ChangeLog
                    

