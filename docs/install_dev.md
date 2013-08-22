Building FLAME-II for framework development {#devbuild}
===========================================

[TOC]

To work on the source, you should use the source from the 
[CCPForge repository](http://ccpforge.cse.rl.ac.uk/gf/project/flame/scmsvn/) and 
not from the release files. Do read the 
[Developer Manual](http://www.softeng.rl.ac.uk/wiki/flame/Notes/DeveloperManual) and 
[SVN Workflow](http://www.softeng.rl.ac.uk/wiki/flame/Notes/DeveloperManual/SVNWorkflow)
document if you intend to contribute changes to the project.

Prerequisites {#devbuild-prereq}
==============

We recommend using `g++` for development work but builds should be tested on other 
compilers before release.

You will need the `autotools` commands (`autoconf`, `automake`, `aclocal`, ...) and 
`libtool` to build FLAME-II using the development source.

You will also need the Boost headers and selected modules. We recommend 
[installing Boost to a custom location](@ref install-boost-custom) so you can build and test
against different versions of Boost before each release.


Preparing the dev source {#devbuild-prep}
=========================

When using the dev source for the first time (or whenever `configure.ac` is modified), 
run the `./autogen.sh` script. This launches the commands needed to prepare the 
source tree and generates the `./configure` script as well as `Makefile.in` templates.

In general, this is only required for a fresh source checkout where the `./configure` 
script is not yet available. However, when in doubt you can always run `./autogen.sh` 
again to rebuild the scripts, especially after an svn update or svn merge. 


Configuring and Compiling {#devbuild-compiling}
=========================

Once the `./autogen.sh` script is run, the standard configure+make workflow can be used.

For framework developement, we suggest the following options for `./configure`: 

    ./configure --enable-dev-mode --with-boost=$BOOST_INSTALL_DIR
    
The `--enable-dev-mode` option is a shortcut for 
`--enable-test-build --disable-debug-build --disable-prod-build --disable-static`. 
This enables the test builds and disables other build options to drastically speed up 
the compilaton process.

This assumes that you will only be testing your work using unit tests. To install and 
test the framework with a model, you'll need to enable the other build types and set 
your install path using `--prefix`.

Once you've configure the code, you're ready to compile the framework and the tests:

    make && make test
    
After each code modification, you should only need to run `make test` to rebuild the 
necessary objects and relink. The configuration script will be automatically run if 
required.

To use a different compiler, e.g. the intel compilers, run `export CXX=icc` before 
`./configure`.  


Enabling support for HDF3 or SQLite3  {#devbuild-hdf}
-------------------------------------

Support for hdf5 or sqlite3 for model input and output can be enabled thus:

    --with-hdf5
    
or

    --with-hdf5=path

where path is the complete path to where `lib/libhdf5*` libraries and 
`include/H5*` include files reside.

    --with-sqlite3
    
or

    --with-sqlite3=path

where path is the installation prefix to the SQLite library. 