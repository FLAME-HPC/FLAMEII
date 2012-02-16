/*!
 * \file src/mem/tests/test_agent_memory.cpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Test suite for the memory module
 */
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <vector>
#include <string>
#include "../memory_manager.hpp"

namespace m = flame::mem;

BOOST_AUTO_TEST_SUITE(MemModule)

BOOST_AUTO_TEST_CASE(test_register_agent) {
  size_t s1 = 100, s2 = 123;
  m::MemoryManager mgr;

  mgr.RegisterAgent("Circle", s1);
  mgr.RegisterAgent("Square", s2);

  // Registering a single agent variable
  mgr.RegisterAgentVar<int>("Circle", "val");

  // Registering multiple agent variavles
  std::vector<std::string> var_names;
  var_names.push_back("x");
  var_names.push_back("y");
  var_names.push_back("z");
  mgr.RegisterAgentVar<double>("Circle", var_names);

  // Register var to the other agent
  mgr.RegisterAgentVar<int>("Square", "val");

  // retrieve vector from non-existent agent
  BOOST_CHECK_THROW(mgr.GetMemoryVector<int>("Triangle", "val"),
                    std::invalid_argument);
  // retrieve unregistered vector from agent
  BOOST_CHECK_THROW(mgr.GetMemoryVector<int>("Circle", "dummy"),
                    std::invalid_argument);
  // retrieve valid vector with wrong type
  BOOST_CHECK_THROW(mgr.GetMemoryVector<int>("Circle", "x"),
                    std::domain_error);
  // retrieve valid vector
  BOOST_CHECK_NO_THROW(mgr.GetMemoryVector<int>("Circle", "val"));
  BOOST_CHECK_NO_THROW(mgr.GetMemoryVector<int>("Square", "val"));
  BOOST_CHECK_NO_THROW(mgr.GetMemoryVector<double>("Circle", "y"));

  // check capacity
  BOOST_CHECK_EQUAL(mgr.GetMemoryVector<double>("Circle", "z").capacity(), s1);
  BOOST_CHECK_EQUAL(mgr.GetMemoryVector<int>("Square", "val").capacity(), s2);

  // populate returned vector then check size
  std::vector<int> &val_vec = mgr.GetMemoryVector<int>("Square", "val");
  val_vec.push_back(1);
  val_vec.push_back(4);
  val_vec.push_back(2);
  BOOST_CHECK_EQUAL(mgr.GetMemoryVector<int>("Square", "val").size(),
                    (size_t)3);
  // size of unpopulated vector
  BOOST_CHECK_EQUAL(mgr.GetMemoryVector<int>("Circle", "val").size(),
                    (size_t)0);
}

BOOST_AUTO_TEST_SUITE_END()
