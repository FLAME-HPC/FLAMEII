# ============================================================================
# Desc    : autoconf configuration
# Author  : Shawn Chin
# Date    : October 2012
# License : GNU Lesser General Public License
# Copyright (c) 2012 STFC Rutherford Appleton Laboratory
# Copyright (c) 2012 University of Sheffield
# ============================================================================

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

# initialise CFLAGS so it does not default to -g -O2
CPPFLAGS=""

# Check for programs
AC_PROG_CXX([g++ pgCC icc, xlC])
AC_PROG_LIBTOOL
#AC_PROG_MAKE_SET

# Turn off shared libraries during beta-testing, since they
# make the build process take too long.
AC_DISABLE_SHARED

#### CUSTOM FLAGS #####
CPPFLAGS_COMMON=""
CPPFLAGS_TEST="-DTESTBUILD"
CPPFLAGS_DEBUG="-DDEBUG"
CPPFLAGS_PROD="-DNDEBUG -DDISABLE_RUNTIME_TYPE_CHECKING"

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
    #CPPFLAGS="${CPPFLAGS} ${GCC_FLAGS_LVL4}"
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

############ PARAMS FOR SUBSTITUTION #################
COMMON_CPPFLAGS="${CPPFLAGS} ${CPPFLAGS_COMMON} ${CPPFLAGS_DEBUG}"
# TODO: Add ${CPPFLAGS_TEST} for --enable-tests
# TODO: Swap DEBUG for PROD for --production
# TODO: Consider building both test/debug and prod on --multi-lib ?
AC_SUBST(COMMON_CPPFLAGS)

COMMON_LDFLAGS=""
AC_SUBST(COMMON_LDFLAGS)

COMMON_LIBS=""
AC_SUBST(COMMON_LIBS)

############# CHECKS ####################
AC_TYPE_SIZE_T  # we use size_t

# check boost stuff
# check libxml2

############ Libtool stuff ##############
LT_INIT()

############# Create Output ##############

AC_CONFIG_FILES([Makefile])

# convert *.in files
AC_OUTPUT([
README
src/io/Makefile
src/mb/Makefile
src/exe/Makefile
src/mem/Makefile
src/model/Makefile
src/compat/C/Makefile
])
