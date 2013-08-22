Developer Notes {#developer}
===============

[TOC]

Standards used on the code base include:
 * [Style](@ref codestyle)
 * [Metrics](@ref codemetrics)
 * [Memory](@ref memory)
 * [Compiler](@ref compiler)
 * [Documentation](@ref docs)
 
 * No GCC warnings.
  * Use: make 2> gcc_warnings.log
 * No Doxygen warnings.

Style {#codestyle}
=====

Using [Google Coding Standards](http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml).

Usage: ../flame/scripts/run_standards.sh . lint

There should be no errors when running this standard.

Metrics {#codemetrics}
=======

Using [C and C++ Code Counter (CCCC)](http://cccc.sourceforge.net/).

Usage: ../flame/scripts/run_standards.sh . cccc

Split CCCC warnings into 2 stages.
Stage one includes:
Per class:
 * 7 lines of code per line of comment
 * 500 lines of code
 * 200 McCabes cyclomatic complexity
 * 30 methods per class
 * 10 public methods per class
 * 5 McCabes cyclomatic complexity per line of comment
Per function:
 * 7 lines of code per line of comment
 * 30 lines of code
 * 10 McCabes cyclomatic complexity
 * 5 McCabes cyclomatic complexity per line of comment
 
Try and get stage one error count to zero except for 'God' classes that require more than 30 methods.
These include some module managers, model, model task and graph classes.

Stage two includes metrics for information flow but it is slightly broken so it is currently not used.

Memory {#memory}
======

Check memory leaks using [valgrind](http://valgrind.org/).

Usage:

>cd tests
>valgrind --leak-check=full --show-reachable=yes ./run_tests

There should be no lost memory at all.
Still reachable memory is acceptable as this can be attributed to libraries used and not flame2.

Fix any uninitialised values in flame2 but not much we can do about ones from libraries.

Compiler {#compiler}
========

[GCC Warnings](http://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html).

Usage: make 2> gcc_warnings.log

This directs std::err to the specified file (makes it easier to read the warnings if any).
Make sure there are no warnings.

Effective C++ Warnings
----------------------

Usage: make CPPFLAGS="-Weffc++ " 2> gcc_warnings.log

As per the guidelines set in "Effective C++ Third Edition" by Scott Meyers there is
a GCC flag for warnings.

All warnings have been fixed except for a warning from a boost library base class:
 * xgraph.cpp:231: warning: base class 'class boost::dfs_visitor<boost::null_visitor>' has a non-virtual destructor

Documentation {#docs}
=============

Using [doxygen](www.doxygen.org/).

Make sure there are no warnings about anything undocumented.

Usage: make docs 2> doxygen_warnings.log
