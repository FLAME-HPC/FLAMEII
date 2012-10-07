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
#include "../agent_memory.hpp"
#include "exceptions/mem.hpp"

namespace m = flame::mem;
namespace e = flame::exceptions;

BOOST_AUTO_TEST_SUITE(MemModule)

BOOST_AUTO_TEST_CASE(test_register_var) {
  size_t size_hint = 1000;
  m::AgentMemory am("circle");

  am.RegisterVar<int>("x_int");
  am.RegisterVar<double>("y_dbl");

  // hint population size and ensure that vars can no longer be registered
  am.HintPopulationSize(size_hint);
  BOOST_CHECK_THROW(am.RegisterVar<double>("z_dbl"), e::logic_error);
  BOOST_CHECK_EQUAL(am.GetPopulationSize(), (size_t)0);

  // retrieve vector that does not exist
  BOOST_CHECK_THROW(am.GetVector<int>("z"), e::invalid_variable);
  // retrieve vector with wrong type
  BOOST_CHECK_THROW(am.GetVector<int>("y_dbl"), e::invalid_type);
  // retrieve vector properly
  am.GetVector<int>("x_int");
  BOOST_CHECK_NO_THROW(am.GetVector<int>("x_int"));

  // Check allocated capacity
  BOOST_CHECK_EQUAL(am.GetVector<int>("x_int")->capacity(), size_hint);
  BOOST_CHECK_EQUAL(am.GetVector<double>("y_dbl")->capacity(), size_hint);

  // modify and access vectors directly
  am.GetVector<int>("x_int")->push_back(10);
  am.GetVector<int>("x_int")->push_back(20);
  BOOST_CHECK_EQUAL(am.GetVector<int>("x_int")->size(), (size_t)2);

  // get reference to memory vector
  std::vector<int> &v = *(am.GetVector<int>("x_int"));
  v.push_back(30);
  v.push_back(40);
  BOOST_CHECK_EQUAL(v.size(), (unsigned int)4);
  BOOST_CHECK_EQUAL(am.GetVector<int>("x_int")->size(), (size_t)4);

  BOOST_CHECK_THROW(am.GetPopulationSize(), e::flame_mem_exception);
  std::vector<double> &y = *(am.GetVector<double>("y_dbl"));
  y.push_back(0);
  y.push_back(0);
  BOOST_CHECK_THROW(am.GetPopulationSize(), e::flame_mem_exception);
  y.push_back(0);
  y.push_back(0);
  BOOST_CHECK_EQUAL(am.GetPopulationSize(), (size_t)4);
}

BOOST_AUTO_TEST_SUITE_END()
