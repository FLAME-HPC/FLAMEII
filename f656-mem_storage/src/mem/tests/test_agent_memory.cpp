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
#include "../agent_memory.hpp"

namespace m = flame::mem;

BOOST_AUTO_TEST_SUITE(MemModule)

BOOST_AUTO_TEST_CASE(test_register_var) {
  unsigned int size_hint = 1000;
  m::AgentMemory am("circle", size_hint);

  am.RegisterVar<int>("x_int");
  am.RegisterVar<double>("y_dbl");

  // retrieve vector that does not exist
  BOOST_CHECK_THROW(am.GetMemoryVector<int>("z"), std::out_of_range);
  // retrieve vector with wrong type
  BOOST_CHECK_THROW(am.GetMemoryVector<int>("y_dbl"), std::domain_error);
  // retrieve vector properly
  BOOST_CHECK_NO_THROW(am.GetMemoryVector<int>("x_int"));

  // Check allocated capacity
  BOOST_CHECK_EQUAL(am.GetMemoryVector<int>("x_int").capacity(), size_hint);
  BOOST_CHECK_EQUAL(am.GetMemoryVector<double>("y_dbl").capacity(), size_hint);

  // modify and access vectors directly
  am.GetMemoryVector<int>("x_int").push_back(10);
  am.GetMemoryVector<int>("x_int").push_back(20);
  BOOST_CHECK_EQUAL(am.GetMemoryVector<int>("x_int").size(), (unsigned int)2);

  // get reference to memory vector
  std::vector<int> &v = am.GetMemoryVector<int>("x_int");
  v.push_back(30);
  v.push_back(40);
  BOOST_CHECK_EQUAL(v.size(), (unsigned int)4);
  BOOST_CHECK_EQUAL(am.GetMemoryVector<int>("x_int").size(), (unsigned int)4);
}

BOOST_AUTO_TEST_SUITE_END()
