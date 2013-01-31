/*!
 * \file flame2/api/dummy.cpp
 * \author Shawn Chin
 * \date Jan 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Dummy file to appease libtool (mainly for OSX builds)
 */
#include "flame2/config.hpp"

// Force rebuild when build config changes (e.g. --prefix). This
// then causes all the libs to be relinked which solves the problem of
// rpaths being wrong for post-make --prefix changes.
#include "flame2/build_config.hpp"
