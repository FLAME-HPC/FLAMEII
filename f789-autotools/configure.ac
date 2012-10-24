# ============================================================================
# Desc    : autoconf configuration
# Author  : Shawn Chin
# Date    : October 2012
# License : GNU Lesser General Public License
# Copyright (c) 2012 STFC Rutherford Appleton Laboratory
# Copyright (c) 2012 University of Sheffield
# ============================================================================

##### TODOs ############
# --disable-compat-C
# --with-boost
#  * unit_test_framework
#  * thread
#  * system
#  * filesystem
# --with-libxml2
# Doxyfile.in
# make cpplint
# make cccc
# make coverage

# Note: first 3 segments of the version number should contain only digits
AC_INIT(libflame2, 0.0.1, shawn.chin@_NOSPAM_stfc.ac.uk)

# Automake
AM_INIT_AUTOMAKE
#AM_MAINTAINER_MODE

AC_PREREQ(2.59)
AC_REVISION($Revision$)
AC_COPYRIGHT([Copyright (c) 2012 STFC Rutherford Appleton Laboratory])

# Specify any unique file in the src directory
# used by generated script to validate path provided by --srcdir
AC_CONFIG_SRCDIR([src/mem/agent_memory.cpp])

# Path to generated config header file
AC_CONFIG_HEADER([headers/include/config.h])

# Path to custom m4 commands
AC_CONFIG_MACRO_DIR([m4])

# initialise CXXFLAGS so it AC_PROG_CXX does not add in "-g -O2"
if test -z $CXXFLAGS; then
  CXXFLAGS=""
fi

# Check for programs
AC_PROG_CXX([g++ pgCC icc, xlC])
AC_PROG_LIBTOOL
#AC_PROG_MAKE_SET

# Turn off shared libraries during beta-testing, since they
# make the build process take too long.
AC_DISABLE_SHARED

##### CHECK BOOST #####

# Check boost version
#  - calls : AC_SUBST(BOOST_CPPFLAGS) / AC_SUBST(BOOST_LDFLAGS)
FLAME_BOOST_BASE([1.46])

# Check for boost_thread
# - calls : AC_SUBST(BOOST_THREAD_LIB)
FLAME_BOOST_THREAD

# Check 
#### CUSTOM FLAGS #####

COMMON_LIBS=""
COMMON_LDFLAGS=""
COMMON_CPPFLAGS="${BOOST_CPPFLAGS}"

# Assign include path relative to Makefile
AC_SUBST(AM_CPPFLAGS,['-I$(top_srcdir)/src -I$(top_builddir)/headers'])

# For now, we're hard-coded to require libxml2 
COMMON_CPPFLAGS+="-I/usr/include/libxml2"

CPPFLAGS_DEV="-DDEBUG -g"
CPPFLAGS_TEST="${CPPFLAGS_DEV} -DTESTBUILD"
CPPFLAGS_PROD="-DNDEBUG -DDISABLE_RUNTIME_TYPE_CHECKING -O2"

GCC_FLAGS_LVL1="-fno-common -std=c++98 -Wall -pedantic"
# (C-only) -Wstrict-prototypes -Wmissing-prototypes
GCC_FLAGS_LVL2="-Wextra -Wpointer-arith"
GCC_FLAGS_LVL3="-Wreturn-type -Wswitch -Wshadow -Wcast-align -Wunused"
GCC_FLAGS_LVL4="-Wwrite-strings -Wcast-qual "
GCC_FLAGS_SHOWSTOPPER="-Werror" # OTT

# Default CPPFLAGS
# Need extra checks to set this depending on compiler
case "$CXX" in
  g++*)
    CPPFLAGS="${CPPFLAGS} ${GCC_FLAGS_LVL1}"
    CPPFLAGS="${CPPFLAGS} ${GCC_FLAGS_LVL2}"
    CPPFLAGS="${CPPFLAGS} ${GCC_FLAGS_LVL3}"
    CPPFLAGS="${CPPFLAGS} ${GCC_FLAGS_LVL4}"
    #CPPFLAGS="${CPPFLAGS} ${GCC_FLAGS_SHOWSTOPPER}"
    ;;
  pgCC*)
    CPPFLAGS="${CPPFLAGS} -c9x -Minform=warn"
    ;;
  icc*)
    CPPFLAGS="${CPPFLAGS} -std=gnu++98 -Wall"
    ;;
  xlC*)
    CPPFLAGS="${CPPFLAGS} -qlang=strict98 -qinfo=all -qflag=w:u -qcheck=all"
    ;;
  *)
    CPPFLAGS="${CPPFLAGS}"
    ;;
esac


############ CHOOSE BUILD TYPES ############################

# Build production library but removing checks and debug symbols
# (default: disabled)
WANT_PROD_BUILD=0
AC_ARG_ENABLE([production-build],
              [AC_HELP_STRING(
                  [--enable-production-build],
                  [Build a production version of the libraryy]
              )])
AS_IF([test "x$enable_production_build" = "xyes"], [
  WANT_PROD_BUILD=1
])
AC_SUBST(WANT_PROD_BUILD)

# Include tests and build test code into library
# (default: disabled)
WANT_TEST_BUILD=0
AC_ARG_ENABLE([test-build],
              [AC_HELP_STRING(
                  [--enable-test-build],
                  [Build tests and a test version of the library]
              )])
AS_IF([test "x${enable_test_build}" = "xyes"], [
  if test $WANT_PROD_BUILD -eq 1; then
    AC_MSG_ERROR([
** CONFLICTING OPTIONS.
   Tests cannot be built using production code. You can use only ONE of
   the following options:
     --enable-production-build
     --enable-test-build
    ])
    exit 1
  fi
  WANT_TEST_BUILD=1
])
AC_SUBST(WANT_TEST_BUILD)
AM_CONDITIONAL(BUILD_TESTS, test $WANT_TEST_BUILD = 1)

# Update CPPFLAGS_COMMON based on build type
if test $WANT_PROD_BUILD -eq 1; then
  COMMON_CPPFLAGS="${COMMON_CPPFLAGS} ${CPPFLAGS_PROD}"
  AC_DEFINE([BUILD_TYPE], ["production"], [Library build type])
elif test $WANT_TEST_BUILD -eq 1; then
  COMMON_CPPFLAGS="${COMMON_CPPFLAGS} ${CPPFLAGS_TEST}"
  AC_DEFINE([BUILD_TYPE], ["test"], [Library build type])
else
  COMMON_CPPFLAGS="${COMMON_CPPFLAGS} ${CPPFLAGS_DEV}"
  AC_DEFINE([BUILD_TYPE], ["development"], [Library build type])
fi


# Substitute vars into Makefiles
AC_SUBST(COMMON_CPPFLAGS)
AC_SUBST(COMMON_LDFLAGS)
AC_SUBST(COMMON_LIBS)

############# CHECKS ####################
AC_TYPE_SIZE_T  # we use size_t

############ Libtool stuff ##############
LT_INIT()

############# Create Output ##############

AC_CONFIG_FILES([Makefile])

# convert *.in files
AC_OUTPUT([
README
tests/Makefile
src/io/Makefile
src/mb/Makefile
src/exe/Makefile
src/mem/Makefile
src/sim/Makefile
src/model/Makefile
src/compat/C/Makefile
])
