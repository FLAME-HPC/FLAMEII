# ============================================================================
# Desc    : automake for xparser2
# Author  : Shawn Chin
# Date    : Jan 2013
# License : GNU Lesser General Public License
# Copyright (c) 2013 STFC Rutherford Appleton Laboratory
# Copyright (c) 2013 University of Sheffield
# ============================================================================
# $Id: $

bin_PROGRAMS = xparser2
xparser2_LIBS = @COMMON_LIBS@
xparser2_LDFLAGS = @COMMON_LDFLAGS@ -rpath @BOOST_LIBDIR@
xparser2_CPPFLAGS = @COMMON_CPPFLAGS@ @AM_CPPFLAGS@
xparser2_LDADD = $(top_srcdir)/libflame2-dbg.la @COMMON_LIBS@
  
xparser2_SOURCES = xparser2.cpp
noinst_HEADERS = xparser2.hpp
