/*!
 * \file flame2/model/printerr.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Holds printerr functions for printing validation errors
 */
#ifndef TESTBUILD
#include <cstdarg>
#include <cstdio>
#endif
#include "printerr.hpp"
#include "flame2/config.hpp"

#ifdef TESTBUILD
/*!
 * \brief Don't print errors
 */
void printErr(const char */*format*/, ...) {}
#else
/*!
 * \brief Print error to standard out
 */
void printErr(const char *format, ...) {
  // Print message to stderr
  va_list arg;
  va_start(arg, format);
  std::vfprintf(stderr, format, arg);
  va_end(arg);
}
#endif
