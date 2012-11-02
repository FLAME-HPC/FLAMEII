# ============================================================================
# Desc    : autoconf configuration
# Author  : Shawn Chin
# Date    : October 2012
# License : GNU Lesser General Public License
# Copyright (c) 2012 STFC Rutherford Appleton Laboratory
# Copyright (c) 2012 University of Sheffield
# ============================================================================
#
# Note: Instead of CXXFLAGS (for C++ builds) we deal mainly with CPPFLAGS so
#       that the flags will be applied to both C and C++ code compilation.
#

##### TODOs ############
# --disable-compat-C
# Doxyfile.in
# make cpplint
# make cccc
# make coverage
# (more for buildbot) ensure that all .cpp files include flame2/config.hpp
#   -- find flame2/ -name "*.cpp" | xargs grep flame2/config.hpp -L

# Note: first 3 segments of the version number should contain only digits
# Note: package name (1st arg) must match the src directory name
AC_INIT(flame2, 0.0.1, shawn.chin@_NOSPAM_stfc.ac.uk)

# Automake
AM_INIT_AUTOMAKE

AC_PREREQ(2.59)
AC_REVISION($Revision$)
AC_COPYRIGHT([
  Copyright (c) 2012 STFC Rutherford Appleton Laboratory
  Copyright (c) 2012 University of Sheffield
])

# Initiliase libtool
# - Turn off shared libraries during beta-testing, since they
#   make the build process take too long.
#LT_INIT([disable-shared])
LT_INIT

# Specify any unique file in the src directory
# used by generated script to validate path provided by --srcdir
AC_CONFIG_SRCDIR([flame2/mem/agent_memory.cpp])

# Path to generated config header file
AC_CONFIG_HEADER([flame2/cfg.h])

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

# Initialse flags that will be applied to all compilations/linking
COMMON_LIBS=""
COMMON_LDFLAGS=""
COMMON_CPPFLAGS=""

# Assign include path relative to Makefile
AC_SUBST(AM_CPPFLAGS,['-I$(top_srcdir)'])

# Build specific flags
CPPFLAGS_DEV="-DDEBUG -g"
CPPFLAGS_TEST="${CPPFLAGS_DEV} -DTESTBUILD"
CPPFLAGS_PROD="-DNDEBUG -DDISABLE_RUNTIME_TYPE_CHECKING -O2"

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
  COMMON_CPPFLAGS+="${CPPFLAGS_PROD} "
  BUILD_TYPE="production"
elif test $WANT_TEST_BUILD -eq 1; then
  COMMON_CPPFLAGS+="${CPPFLAGS_TEST} "
  BUILD_TYPE="test"
else
  COMMON_CPPFLAGS+="${CPPFLAGS_DEV} "
  BUILD_TYPE="development"
fi
AC_DEFINE_UNQUOTED([BUILD_TYPE], ["$BUILD_TYPE"], [Library build type])
AC_SUBST(BUILD_TYPE)

##### CHECK BOOST #####

# Check boost version
#  - calls : AC_SUBST(BOOST_CPPFLAGS) / AC_SUBST(BOOST_LDFLAGS)
FLAME_BOOST_BASE([1.46])

# Allows user to change boost library suffix
#  - sets var BOOST_LIB_SUFFIX
FLAME_BOOST_LIB_SUFFIX

# Header files we need (do we really need to test these?)
AC_LANG_PUSH([C++])  # build tests using C++
STORE_CPPFLAGS="${CPPFLAGS}"
CPPFLAGS="${CPPFLAGS} ${BOOST_CPPFLAGS}"
AC_CHECK_HEADER([boost/graph/adjacency_list.hpp], [],
                [AC_MSG_FAILURE([Boost Graph Library headers missing])])
AC_CHECK_HEADER([boost/any.hpp], [],
                [AC_MSG_FAILURE([Could not find boost/any.hpp])]) 
AC_CHECK_HEADER([boost/function.hpp], [],
                [AC_MSG_FAILURE([Could not find boost/function.hpp])])
AC_CHECK_HEADER([boost/foreach.hpp], [],
                [AC_MSG_FAILURE([Could not find boost/foreach.hpp])])
AC_CHECK_HEADER([boost/shared_ptr.hpp], [],
                [AC_MSG_FAILURE([Could not find boost/shared_ptr.hpp])])
AC_CHECK_HEADER([boost/ptr_container/ptr_map.hpp], [],
                [AC_MSG_FAILURE([Could not find boost/ptr_container/ptr_map.hpp])])
AC_CHECK_HEADER([boost/ptr_container/ptr_vector.hpp], [],
                [AC_MSG_FAILURE([Could not find boost/ptr_container/ptr_vector.hpp])])
CPPFLAGS="${STORE_CPPFLAGS}"
AC_LANG_POP([C++])

# ----------- non-header-only libs ----------------------

# Check for boost_thread
# - calls : AC_SUBST(BOOST_THREAD_LIB)
FLAME_BOOST_THREAD

# Check for boost_system (required by boost_filesystem)
# - calls : AC_SUBST(BOOST_SYSTEM_LIB)
FLAME_BOOST_SYSTEM

# Check for boost_thread
# - calls : AC_SUBST(BOOST_FILESYSTEM_LIB)
FLAME_BOOST_FILESYSTEM


if test $WANT_TEST_BUILD -eq 1; then
  # Check for boost_unit_test_framework
  # - calls : AC_SUBST(BOOST_UNIT_TEST_FRAMEWORK_LIB)
  FLAME_BOOST_UNIT_TEST_FRAMEWORK
fi

###### CHECK OTHER DEPENDENCIES ###################

# Check for libxml2
# - calls : AC_SUBST(LIBXML2_CPPFLAGS) / AC_SUBST(LIBXML2_LIBS)
FLAME_CHECK_LIBXML2

# Notes:
# - We don't use AM_PATH_XML2 since that is provided by libxml2-devel.
#   and so the macro is not available if users do not have libxml2 installed
# - libxml2 uses pkg-config, so we could use PKG_CHECK_MODULES
#   * e.g. PKG_CHECK_MODULES([XML2], [libxml-2.0])
#   * This does not allow a uniform way of specifying custom libxml2 location,
#     i.e. --with-libxml2=LIBXML2_PATH

#### UPDATE FLAGS BASED ON DEPENDENCIES

COMMON_CPPFLAGS+="${BOOST_CPPFLAGS} ${LIBXML2_CPPFLAGS} "
COMMON_LDFLAGS+="${BOOST_LDFLAGS} ${BOOST_THREAD_LIB} "
COMMON_LDFLAGS+="${BOOST_SYSTEM_LIB} ${BOOST_FILESYSTEM_LIB} "

# For now, we're hard-coded to require libxml2 ($XML_CPPFLAGS)
COMMON_LDFLAGS+="${LIBXML2_LIBS} "

#### SET DEFAULT CPPFLAGS (Can be overridden by users) ####

GCC_FLAGS_LVL1="-fno-common -Wall -pedantic"
# (C-only) -Wstrict-prototypes -Wmissing-prototypes
GCC_FLAGS_LVL2="-Wextra -Wpointer-arith"
GCC_FLAGS_LVL3="-Wreturn-type -Wswitch -Wshadow -Wcast-align -Wunused"
GCC_FLAGS_LVL4="-Wwrite-strings -Wcast-qual "
GCC_FLAGS_SHOWSTOPPER="-Werror" # OTT

# Default CPPFLAGS
# Need extra checks to set this depending on compiler
case "$CXX" in
  g++*|gcc*)
    CPPFLAGS="${CPPFLAGS} ${GCC_FLAGS_LVL1}"
    CPPFLAGS="${CPPFLAGS} ${GCC_FLAGS_LVL2}"
    CPPFLAGS="${CPPFLAGS} ${GCC_FLAGS_LVL3}"
    CPPFLAGS="${CPPFLAGS} ${GCC_FLAGS_LVL4}"
    ##CPPFLAGS="${CPPFLAGS} ${GCC_FLAGS_SHOWSTOPPER}"
  
    CFLAGS="${CFLAGS} -std=c99 -Wstrict-prototypes -Wmissing-prototypes"
    CXXFLAGS="${CXXFLAGS} -std=c++98"
    ;;
  pgCC*|pgcc*)
    CPPFLAGS="${CPPFLAGS} -c9x -Minform=warn"
    ;;
  icc*)
    CPPFLAGS="${CPPFLAGS} -Wall"
    CXXFLAGS="${CXXFLAGS} -std=gnu++98"
    CFLAGS="${CFLAGS} -std=c99"
    ;;
  xlC*|xlc*)
    CPPFLAGS="${CPPFLAGS} -qinfo=all -qflag=w:u -qcheck=all"
    CXXFLAGS="${CXXFLAGS} -qlang=strict98"
    CFLAGS="${CFLAGS} -qlanglvl=stdc99"
    ;;
  *)
    CPPFLAGS="${CPPFLAGS}"
    ;;
esac

######### Clean vars (for aesthetic reasons only) ############

AC_DEFUN([CLEAN_VAR], [
dnl Remove trailing and leading spaces, and suppress repeated spaces
  $1=`echo [$]$1 | sed 's/^ *//g; s/ *$//g; s/  */ /g'`
])

CLEAN_VAR(COMMON_CPPFLAGS)
CLEAN_VAR(COMMON_LDFLAGS)
CLEAN_VAR(COMMON_LIBS)
CLEAN_VAR(CPPFLAGS)
CLEAN_VAR(CXXFLAGS)
CLEAN_VAR(LDFLAGS)
CLEAN_VAR(LIBS)

########### Substitute vars into Makefiles ################

AC_SUBST(COMMON_CPPFLAGS)
AC_SUBST(COMMON_LDFLAGS)
AC_SUBST(COMMON_LIBS)
AC_DEFINE_UNQUOTED([COMMON_CPPFLAGS], ["$COMMON_CPPFLAGS"],
                   [extra CPPFLAGS applied by ./configure options])
AC_DEFINE_UNQUOTED([COMMON_LDFLAGS], ["$COMMON_LDFLAGS"],
                   [extra LDFLAGS applied by ./configure options])

FLAME2_CONFIG_DIR=`pwd`
FLAME2_CONFIG_DATE=`date`
FLAME2_CONFIG_USER=`whoami`
FLAME2_CONFIG_ARGS=$ac_configure_args
AC_SUBST(FLAME2_CONFIG_DIR)
AC_SUBST(FLAME2_CONFIG_DATE)
AC_SUBST(FLAME2_CONFIG_USER)
AC_SUBST(FLAME2_CONFIG_ARGS)

AC_DEFINE_DIR([FLAME2_INSTALL_BINDIR], [bindir], [Install dir for binaries])
AC_DEFINE_DIR([FLAME2_INSTALL_LIBDIR], [libdir], [Install dir for libraries])
AC_DEFINE_DIR([FLAME2_INSTALL_INCLUDEDIR], [includedir], [Install dir for include files])

if test "x${enable_shared}" = "xyes"; then
  BUILD_SHARED="yes"
else
  BUILD_SHARED="no"
fi
AC_SUBST(BUILD_SHARED)

if test "x${enable_static}" = "xyes"; then
  BUILD_STATIC="yes"
else
  BUILD_STATIC="no"
fi
AC_SUBST(BUILD_STATIC)

############# CHECKS ####################
AC_TYPE_SIZE_T  # we use size_t

############# Create Output ##############

AC_CONFIG_FILES([Makefile])

# convert *.in files
AC_OUTPUT([
README
utils/flame2-config
utils/Makefile
tests/Makefile
flame2/io/Makefile
flame2/mb/Makefile
flame2/exe/Makefile
flame2/mem/Makefile
flame2/sim/Makefile
flame2/model/Makefile
flame2/compat/C/Makefile
flame2/exceptions/Makefile
])

############# Output upon successfule configuration ######################

echo ""
echo "------------------- Configuration successful --------------------------"
echo ""
echo "Build type       : $BUILD_TYPE"
echo ""
echo "Default compiler : $CXX"
echo "Default CXXFLAGS : $CXXFLAGS"
echo "Default CPPFLAGS : $CPPFLAGS"
echo "Default LDFLAGS  : $LDFLAGS"
echo "Default LIBS     : $LIBS"
echo ""
echo "Additional CPPFLAGS : $COMMON_CPPFLAGS"
echo "Additional LDFLAGS  : $COMMON_LDFLAGS"
echo "Additional LIBS     : $COMMON_LIBS"
echo ""
echo "-----------------------------------------------------------------------"
echo ""
echo "If you wish to use a different compiler and/or FLAGS, you can "
echo "override those variables during the invocation of 'make'. For example:"
echo '# make CXX="xlC_r" CPPFLAGS="-qinfo=all -qflag=w:u -qcheck=all "'
echo ""

