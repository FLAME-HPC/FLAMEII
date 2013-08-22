Installing Boost {#install-boost}
================

[TOC]

Installing Boost - system-wide (requires root access) {#install-boost-system}
==============================

The most convenient approach would be to do a system-wide install of Boost -- this allows
you to compile FLAME and run models without having to specify additional flags or
environment variables. This can be achieved either by using the package manager that
comes with your Linux distribution, or by building and installing Boost from source.

Using package manager (ubuntu) {#install-boost-system-ubuntu}
-------------------------------

The simplest approach would be to install all boost libraries. You can do so by running
the following command as root:

    apt-get install libboost1.48-all-dev
    
Or, if you prefer to only install the required portions of the boost library"

    apt-get install libboost-thread1.48-dev libboost-filesystem1.48-dev libboost-test1.48-dev libboost-program_options1.48-dev

From source {#install-boost-system-source}
-----------------------------

 1. Download the latest source from the [Boost website](http://www.boost.org)
 2. Unpack the source
 3. Within the extracted source directory, run:
 
        ./bootstrap.sh
 4. Build and install boost using:
 
        ./b2 install
        

Note that the steps above will build and install the whole Boost library which may take
a while. To speed things up, we can choose to build only the modules that are needed
by FLAME-II. You can do this by replacing step 3 with the following command:

    ./bootstrap.sh --with-libraries=filesystem,system,thread,test,program_options
    

Installing Boost to a custom location {#install-boost-custom}
=====================================
If you do not have root access of if you prefer to install Boost to a custom location, 
additional steps may be required to build and run the FLAME framework and models.

 1. Download the latest source from the [Boost website](http://www.boost.org)
 2. Unpack the source
 3. Within the extracted source directory, run:
 
        BOOST_INSTALL_DIR=$HOME/boost   # specify install location
        ./bootstrap.sh --prefix=$BOOST_INSTALL_DIR --with-libraries=filesystem,system,thread,test,program_options
 4. Build and install
 
        ./b2 install
        
The boost header files would be installed to `$BOOST_INSTALL_DIR/include` and the
libraries in `$BOOST_INSTALL_DIR/lib`.

Note that the commands above assumes you're using the g++ compiler. Other compilers
can be used by specifying a suitable target during `./bootstrap.sh`. For example, to 
use the Intel C++ compiler, use the `--target=intel-linux` option.

