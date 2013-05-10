Building and Installing FLAME-II  {#installation}
=================================

[TOC]

Dependencies {#install-dependencies}
============

FLAME-II is written in C++ and makes extensive use of the 
[Boost C++ Libaries](http://www.boost.org/). Specifically, we rely on the Boost headers 
as well as the following compiled Boost modules:

 * Boost.Thread
 * Boost.System
 * Boost.Filesystem
 * Boost.Program_options
 * Boost.Test
 
Boost version >=1.48 is required. See [installing boost](@ref install-boost).

FLAME-II also uses [libxml2](http://www.xmlsoft.org/) to read and parse XML files. 
As we currently rely on XML for the model definition and input population data, 
libxml2 is a required dependency. In the future, we hope to provide other formats for 
defining models and population data at which point this dependency would be optional.

 
Compiling and Installing FLAME-II {#install-compile}
=================================

Releases can be 
[downloaded from CPPForge](http://ccpforge.cse.rl.ac.uk/gf/project/flame/frs/) 
under the FLAME-II package. For the time being, releases are private and you will need 
to be logged in as a project member for that package to be visible.

FLAME-II can be installed using the standard `./configure && make && make install` 
workflow.

At this stage of development we recommend using the g++ compiler. 
This will be the first compiler that the `configure` and `make` step will try to pick up. 
While we do intend to support the fully range of compilers where possible, we have yet to 
test them thoroughly; it is possible to switch to a different compiler but we cannot 
promise that the process that lies ahead will be uneventful.

The configuration step can be customised using the following options:

 * `--prefix=<INSTALL_PATH>` - specify a custom location to install to 
   (defaults to `/usr/local`)
   
 *  `--with-libxml2=<XML2_INSTALL_DIR>` - specify where we should look for a libxml2 
    installation (use this if you've installed libxml2 to a custom location)
    
 *  `--with-boost=<BOOST_INSTALL_DIR>` - specify where we should look for a Boost 
    installation (use this if you've installed Boost to a custom location)

 * `--with-boost-lib-suffix=<SUFFIX>` - in some cases, Boost libraries are installed 
   with additional suffixes e.g. `libboost_thread-gcc-mt.a` instead of 
   `libboost_thread.a`. Use this option to specify the suffix, e.g. 
   `--with-boost-lib-suffix=gcc-mt`. 

Run `./configure --help` to get a list of more options.

Here's an example of how one would install FLAME-II to a custom location, assuming Boost 
and libxml2 have been installed into the standard location:


    [me@home]$ FLAME2_DIR="$HOME/build/flame2 "
    [me@home]$ ./configure --prefix=$FLAME2_DIR  # specify location where we want flame2 installed to
    [me@home]$ make  # compile. This may take a while
    [me@home]$ make install  # install to specified path

That will install the FLAME-II headers to `$FLAME2_DIR/include`, 
the libraries to `$FLAME2_DIR/lib`, the binaries (including the xparser) to 
`$FLAME2_DIR/bin`, plus some additional files to `$FLAME2_DIR/share/flame2`. 