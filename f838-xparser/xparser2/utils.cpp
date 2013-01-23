/*!
 * \file xparser2/utils.cpp
 * \author Shawn Chin
 * \date January 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Misc utility functions
 */
#include <string>
#include <iostream>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include "printer.hpp"

namespace xparser { namespace utils {

std::string locate_template(const char* template_name) {
  // first, look for the file relative to the current working directory
  boost::filesystem::path cwd(boost::filesystem::current_path());
  boost::filesystem::path out = cwd / "xparser2/templates" / template_name;
  if (boost::filesystem::exists(out) &&
      boost::filesystem::is_regular_file(out)) {
    return out.string();
  }

  // if package data install dir defined, look there next
  #ifdef PKGDATADIR
    boost::filesystem::path pkgdir(PKGDATADIR);
    out = pkgdir / template_name;
    if (boost::filesystem::exists(out) &&
      boost::filesystem::is_regular_file(out)) {
    return out.string();
  }
  #endif

  return "";
}

std::string gen_random_string(const int len) {
  static const char pool[] = "0123456789"
                                  "abcdefghijklmnopqrstuvwxyz"
                                  "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  static const int pool_size = sizeof(pool) - 1;  // exclude EOL anchor
  if (len < 1) {
    return "";
  }

  std::string s;
  s.reserve(len + 1);  // so we allocate memory only once
  for (int i = 0; i < len; ++i) {
    s.append(1, pool[rand() % pool_size]);  // NOLINT(runtime/threadsafe_fn)
  }
  return s;
}

void print_template(const char* template_name) {
  xparser::Printer out(&std::cout);
  out.PrintRawFromFile(locate_template(template_name));
}

}}  // namespace xparser::utils
