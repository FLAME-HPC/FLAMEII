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
# libtool versioning. See http://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html

# Note: first 3 segments of the version number should contain only digits
# Note: package name (1st arg) must match the src directory name
AC_INIT(flame2, 0.0.2-pre, shawn.chin@_NOSPAM_stfc.ac.uk)

# Automake
AM_INIT_AUTOMAKE

AC_PREREQ(2.59)
AC_REVISION($Revision$)
AC_COPYRIGHT([
  Copyright (c) 2013 STFC Rutherford Appleton Laboratory
  Copyright (c) 2013 University of Sheffield
])

# check user-defined paths to make sure the do not contain spaces
FLAME_CHECK_DIRS_NO_SPACES

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
#AC_PROG_MAKE_SET

# Initialse flags that will be applied to all compilations/linking
COMMON_LDFLAGS=""
COMMON_CPPFLAGS=""

# Assign include path relative to Makefile
AC_SUBST(AM_CPPFLAGS,['-I$(top_srcdir)'])

# Build specific flags
CPPFLAGS_DBG="-DDEBUG -DDBGBUILD -g"
CPPFLAGS_TEST="-DDEBUG -DTESTBUILD -g"
CPPFLAGS_PROD="-DNDEBUG -DPRODBUILD -DDISABLE_RUNTIME_TYPE_CHECKING -O2"

############ CHOOSE BUILD TYPES ############################
BUILDS=""
WANT_TEST_BUILD=0
WANT_PROD_BUILD=1
WANT_DBG_BUILD=1

AC_ARG_ENABLE([dev-mode],
              [AC_HELP_STRING(
                  [--enable-dev-mode],
                  [Change default build to test-build only and disable static libraries to speed up builds]
              )])
AS_IF([test "x$enable_dev_mode" = "xyes"], [
  WANT_TEST_BUILD=1
  WANT_PROD_BUILD=0
  WANT_DBG_BUILD=0
  enable_static=no
])

# Conditionally include tests and build test code into library
# (default: disabled)
AC_ARG_ENABLE([test-build],
              [AC_HELP_STRING(
                  [--enable-test-build],
                  [Build tests and a test version of the library]
              )])
AS_IF([test "x$enable_test_build" = "xyes"], [WANT_TEST_BUILD=1])
AM_CONDITIONAL(BUILD_TEST, test $WANT_TEST_BUILD = 1)
if test $WANT_TEST_BUILD = 1; then BUILDS="test $BUILDS"; fi

# Conditionally build production libraries
# (default: enable)

AC_ARG_ENABLE([prod-build],
              [AC_HELP_STRING(
                  [--disable-prod-build],
                  [Do not build and install production library]
              )])
AS_IF([test "x$enable_prod_build" = "xno"], [WANT_PROD_BUILD=0])
AM_CONDITIONAL(BUILD_PROD, test $WANT_PROD_BUILD = 1)
if test $WANT_PROD_BUILD = 1; then BUILDS="production $BUILDS"; fi

# Conditionally build debug libraries
# (default: enable)

AC_ARG_ENABLE([debug-build],
              [AC_HELP_STRING(
                  [--disable-debug-build],
                  [Do not build and install debug library]
              )])
AS_IF([test "x$enable_debug_build" = "xno"], [WANT_DBG_BUILD=0])
AM_CONDITIONAL(BUILD_DEBUG, test $WANT_DBG_BUILD = 1)
if test $WANT_DBG_BUILD = 1; then BUILDS="debug $BUILDS"; fi

if test "x${BUILDS}" = "x"; then
  AC_MSG_ERROR([All build types disabled. Nothing left to do.])
fi

# Sets LOWEST_BUILD_SUFFIX to the lowest level library suffix available
if test $WANT_TEST_BUILD = 1; then
  AC_SUBST([LOWEST_BUILD_SUFFIX], [test])
elif test $WANT_DBG_BUILD = 1; then
  AC_SUBST([LOWEST_BUILD_SUFFIX], [dbg])
else
  AC_SUBST([LOWEST_BUILD_SUFFIX], [prod])
fi

##### CHECK BOOST #####

# Check boost version
#  - calls : AC_SUBST(BOOST_CPPFLAGS) / AC_SUBST(BOOST_LDFLAGS) / AC_SUBST(BOOST_LIBDIR)
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
#AC_CHECK_HEADER([boost/any.hpp], [],
#                [AC_MSG_FAILURE([Could not find boost/any.hpp])]) 
#AC_CHECK_HEADER([boost/function.hpp], [],
#                [AC_MSG_FAILURE([Could not find boost/function.hpp])])
#AC_CHECK_HEADER([boost/foreach.hpp], [],
#                [AC_MSG_FAILURE([Could not find boost/foreach.hpp])])
#AC_CHECK_HEADER([boost/shared_ptr.hpp], [],
#                [AC_MSG_FAILURE([Could not find boost/shared_ptr.hpp])])
#AC_CHECK_HEADER([boost/ptr_container/ptr_map.hpp], [],
#                [AC_MSG_FAILURE([Could not find boost/ptr_container/ptr_map.hpp])])
#AC_CHECK_HEADER([boost/ptr_container/ptr_vector.hpp], [],
#                [AC_MSG_FAILURE([Could not find boost/ptr_container/ptr_vector.hpp])])
AC_CHECK_HEADER([boost/container/flat_set.hpp], [],
                [AC_MSG_FAILURE([Could not find boost/container/flat_set.hpp])])
CPPFLAGS="${STORE_CPPFLAGS}"
AC_LANG_POP([C++])

# ----------- non-header-only libs ----------------------

# Check for boost_thread
# - calls : AC_SUBST(BOOST_THREAD_LIB)
FLAME_BOOST_THREAD

# Check for boost_system (required by boost_filesystem)
# - calls : AC_SUBST(BOOST_SYSTEM_LIB)
FLAME_BOOST_SYSTEM

# Check for boost_filesystem
# - calls : AC_SUBST(BOOST_FILESYSTEM_LIB)
FLAME_BOOST_FILESYSTEM

# Check for boost_program_options
# - calls : AC_SUBST(BOOST_PROGRAM_OPTIONS_LIB)
FLAME_BOOST_PROGRAM_OPTIONS

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

# Check for hdf5
AX_LIB_HDF5()
AM_CONDITIONAL(HAVE_HDF5, test "$with_hdf5" = "yes")

#### UPDATE FLAGS BASED ON DEPENDENCIES

COMMON_CPPFLAGS+="${BOOST_CPPFLAGS} ${LIBXML2_CPPFLAGS} "
COMMON_LDFLAGS+="${BOOST_LDFLAGS} ${BOOST_THREAD_LIB} "
COMMON_LDFLAGS+="${BOOST_SYSTEM_LIB} ${BOOST_FILESYSTEM_LIB} "
COMMON_LDFLAGS+="${BOOST_PROGRAM_OPTIONS_LIB} "

# For now, we're hard-coded to require libxml2 ($XML_CPPFLAGS)
COMMON_LDFLAGS+="${LIBXML2_LIBS} "

# For HDF5
COMMON_CPPFLAGS+="${HDF5_CPPFLAGS} "
COMMON_LDFLAGS+="${HDF5_LDFLAGS} "

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

########### initialise libtool ################
LT_INIT()

######### extract -l* flags from COMMON_LDFLAGS to COMMON_LIBS #########

X_LDFLAGS="${COMMON_LDFLAGS}"
COMMON_LDFLAGS=`for f in ${X_LDFLAGS}; do echo $f | awk '!/^-l/{printf "%s ", $0};'; done`
COMMON_LIBS=`for f in ${X_LDFLAGS}; do echo $f | awk '/^-l/{printf "%s ", $0};'; done`

######### Clean vars (for aesthetic reasons only) ############

AC_DEFUN([CLEAN_VAR], [
dnl Remove trailing and leading spaces, and suppress repeated spaces and slashes
  $1=`echo [$]$1 | sed 's/^ *//g; s/ *$//g; s/  */ /g' | tr -s '/'`
])

CLEAN_VAR(BOOST_LIBDIR)
CLEAN_VAR(COMMON_CPPFLAGS)
CLEAN_VAR(COMMON_LDFLAGS)
CLEAN_VAR(COMMON_LIBS)
CLEAN_VAR(CPPFLAGS)
CLEAN_VAR(CXXFLAGS)
CLEAN_VAR(LDFLAGS)
CLEAN_VAR(LIBS)


############# CHECKS ####################
AC_TYPE_SIZE_T  # we use size_t

########### Substitute vars into Makefiles ################

AC_SUBST(CPPFLAGS_DBG)
AC_SUBST(CPPFLAGS_PROD)
AC_SUBST(CPPFLAGS_TEST)

AC_SUBST(COMMON_CPPFLAGS)
AC_SUBST(COMMON_LDFLAGS)
AC_SUBST(COMMON_LIBS)
#AC_DEFINE_UNQUOTED([COMMON_CPPFLAGS], ["$COMMON_CPPFLAGS"],
#                   [extra CPPFLAGS applied by ./configure options])
#AC_DEFINE_UNQUOTED([COMMON_LDFLAGS], ["$COMMON_LDFLAGS"],
#                   [extra LDFLAGS applied by ./configure options])

# TODO: extract -l.. from LDFLAGS and provide as COMMON_LIBS
#       insert those under LDADD in makefiles

FLAME2_CONFIG_DIR=`pwd`
FLAME2_CONFIG_DATE=`date`
FLAME2_CONFIG_USER=`whoami`
FLAME2_CONFIG_ARGS=$ac_configure_args
AC_SUBST(FLAME2_CONFIG_DIR)
AC_SUBST(FLAME2_CONFIG_DATE)
AC_SUBST(FLAME2_CONFIG_USER)
AC_SUBST(FLAME2_CONFIG_ARGS)

AC_SUBST_DIR([FLAME2_INSTALL_BINDIR], [bindir])
AC_SUBST_DIR([FLAME2_INSTALL_LIBDIR], [libdir])
AC_SUBST_DIR([FLAME2_INSTALL_INCLUDEDIR], [includedir])
AC_SUBST_DIR([FLAME2_INSTALL_DATADIR], datadir/$PACKAGE)
AC_SUBST_DIR([FLAME2_INSTALL_PLUGINDIR], datadir/plugins/)

#AC_DEFINE_DIR([FLAME2_INSTALL_BINDIR], [bindir], [Install dir for binaries])
#AC_DEFINE_DIR([FLAME2_INSTALL_LIBDIR], [libdir], [Install dir for libraries])
#AC_DEFINE_DIR([FLAME2_INSTALL_INCLUDEDIR], [includedir], [Install dir for include files])
#AC_DEFINE_DIR([FLAME2_INSTALL_DATADIR], datadir/$PACKAGE, [Install dir for data files])

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
flame2/api/Makefile
flame2/exceptions/Makefile
flame2/build_config.hpp
xparser2/Makefile
xparser2/templates/xparser_header.tmpl
])

############# Output upon successfule configuration ######################

echo ""
echo "------------------- Configuration successful --------------------------"
echo ""
echo "Build type       : $BUILDS"
echo ""
echo "Default compiler : $CXX"
echo "Default CXXFLAGS : $CXXFLAGS"
echo "Default CPPFLAGS : $CPPFLAGS"
echo "Default LDFLAGS  : $LDFLAGS"
echo "Default LIBS     : $LIBS"
echo ""
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

