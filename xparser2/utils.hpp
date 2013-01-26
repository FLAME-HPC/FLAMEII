/*!
 * \file xparser2/utils.hpp
 * \author Shawn Chin
 * \date January 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Misc utility functions
 */
#ifndef XPARSER__UTILS_HPP_
#define XPARSER__UTILS_HPP_
#include <string>

namespace xparser { namespace utils {

/*! \brief Locates and returns full path to template with given filename
 *
 * The following directories are search in order:
 * - ./xparser2/templates
 * - $PKGDATA_INSTALL_DIRECTORY
 *
 * If the template is not found, an empty string is returned
 */
std::string locate_template(const char* template_name);

//! Returns a random string of specified length containing alpha-numeric chars
std::string gen_random_string(const int len);

//! Dumps template to stdout
void print_template(const char* template_name);

}}  // namespace xparser::utils
#endif   // XPARSER__UTILS_HPP_
