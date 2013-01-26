/*!
 * \file tests/xparser2/test_xparser2_utils.cpp
 * \author Shawn Chin
 * \date January 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Test suite for xparser2
 */
#define BOOST_TEST_DYN_LINK
#include <unistd.h>  // for chdir()
#include <set>
#include <string>
#include <boost/filesystem/operations.hpp>
#include <boost/test/unit_test.hpp>
#include "xparser2/utils.hpp"

BOOST_AUTO_TEST_SUITE(XPARSER2)

BOOST_AUTO_TEST_CASE(xp_random_string) {
  std::string s;

  // invalid count should return empty string
  s = xparser::utils::gen_random_string(0);
  BOOST_CHECK(s.empty());
  s = xparser::utils::gen_random_string(-1);
  BOOST_CHECK(s.empty());

  // make sure the returned string lengths are correct
  s = xparser::utils::gen_random_string(1);
  BOOST_CHECK_EQUAL(s.size(), 1);
  s = xparser::utils::gen_random_string(5);
  BOOST_CHECK_EQUAL(s.size(), 5);

  // can't really test if they are actually random, but we can do a quick check
  // to ensure that we're getting different values for a series of runs
  int sample_size = 100;
  std::set<std::string> returned_strings;
  for (int i = 0; i < sample_size; ++i) {
    returned_strings.insert(xparser::utils::gen_random_string(8));
  }
  BOOST_CHECK_EQUAL(returned_strings.size(), sample_size);
}

BOOST_AUTO_TEST_CASE(xp_locate_template) {
  // We cannot easily test for templates in PKGDATA_INSTALL_DIRECTORY.
  // Do what we can and just test for known templates within the distribution
  std::string s;
  std::string cwd = boost::filesystem::current_path().string();

  // Change working dir to source root
  chdir("../");

  s = xparser::utils::locate_template("unknown");
  BOOST_CHECK(s.empty());

  // make sure it returns a valid path file filename
  s = xparser::utils::locate_template("Makefile.tmpl");
  BOOST_CHECK(!s.empty());
  BOOST_CHECK(boost::filesystem::exists(s));

  // restore working directory
  chdir(cwd.c_str());
}

BOOST_AUTO_TEST_SUITE_END()
