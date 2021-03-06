/*!
 * \file tests/mem/test_agent_memory.cpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Test suite for the memory module
 */
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <map>
#include <vector>
#include <string>
#include <stdexcept>
#include "flame2/mem/memory_manager.hpp"

namespace m = flame::mem;
namespace e = flame::exceptions;

BOOST_AUTO_TEST_SUITE(MemModule)

BOOST_AUTO_TEST_CASE(initialise_and_test_singleton) {
  m::MemoryManager& mgr1 = m::MemoryManager::GetInstance();
  mgr1.RegisterAgent("TestSingleton");

  m::MemoryManager& mgr = m::MemoryManager::GetInstance();
  // That agent should already be registered
  BOOST_CHECK_THROW(mgr.RegisterAgent("TestSingleton"), e::logic_error);
}

BOOST_AUTO_TEST_CASE(test_register_agent) {
  size_t s1 = 100, s2 = 123;
  m::MemoryManager& mgr = m::MemoryManager::GetInstance();

  mgr.RegisterAgent("Circle");
  mgr.RegisterAgent("Square");

  // Registering a single agent variable
  mgr.RegisterAgentVar<int>("Circle", "val");

  BOOST_CHECK_NO_THROW(mgr.AssertVarRegistered("Circle", "val"));
  BOOST_CHECK_THROW(mgr.AssertVarRegistered("Round", "val"), e::invalid_agent);
  BOOST_CHECK_THROW(mgr.AssertVarRegistered("Circle", "vol"),
                    e::invalid_variable);

  // Registering multiple agent variables
  std::vector<std::string> var_names;
  var_names.push_back("x");
  var_names.push_back("y");
  var_names.push_back("z");
  mgr.RegisterAgentVar<double>("Circle", var_names);

  // Register var to the other agent
  mgr.RegisterAgentVar<int>("Square", "val");

  // Population Size hint
  mgr.HintPopulationSize("Circle", s1);
  BOOST_CHECK_THROW(mgr.RegisterAgentVar<double>("Circle", "z_dbl"),
                    e::logic_error);
  BOOST_CHECK_NO_THROW(mgr.RegisterAgentVar<double>("Square", "z_dbl"));

  mgr.HintPopulationSize("Square", s2);
  BOOST_CHECK_THROW(mgr.RegisterAgentVar<double>("Circle", "q_dbl"),
                    e::logic_error);
  BOOST_CHECK_THROW(mgr.RegisterAgentVar<double>("Square", "q_dbl"),
                    e::logic_error);

  // retrieve vector from non-existent agent
  BOOST_CHECK_THROW((mgr.GetVector<int>("Triangle", "val")),
                    e::invalid_agent);
  // retrieve unregistered vector from agent
  BOOST_CHECK_THROW((mgr.GetVector<int>("Circle", "dummy")),
                    e::invalid_variable);
  // retrieve valid vector with wrong type
  BOOST_CHECK_THROW((mgr.GetVector<int>("Circle", "x")), e::invalid_type);
  // retrieve valid vector
  BOOST_CHECK_NO_THROW((mgr.GetVector<int>("Circle", "val")));
  BOOST_CHECK_NO_THROW((mgr.GetVector<int>("Square", "val")));
  BOOST_CHECK_NO_THROW((mgr.GetVector<double>("Circle", "y")));

  // Check capacity of vectors within
  BOOST_CHECK_EQUAL(mgr.GetVector<double>("Circle", "x")->capacity(), s1);
  BOOST_CHECK_EQUAL(mgr.GetVector<int>("Circle", "val")->capacity(), s1);
  BOOST_CHECK_EQUAL(mgr.GetVector<int>("Square", "val")->capacity(), s2);
}

BOOST_AUTO_TEST_CASE(test_typed_vector_access) {
  m::MemoryManager& mgr = m::MemoryManager::GetInstance();
  mgr.RegisterAgent("Circle2");
  mgr.RegisterAgentVar<int>("Circle2", "val");
  mgr.HintPopulationSize("Circle2", 10);

  std::vector<int>* vec = mgr.GetVector<int>("Circle2", "val");
  BOOST_CHECK(vec->begin() == vec->end());
  BOOST_CHECK(vec->empty());
  BOOST_CHECK_EQUAL(vec->size(), (size_t)0);

  vec->push_back(10);
  vec->push_back(20);
  BOOST_CHECK_EQUAL(vec->size(), (size_t)2);
}


void test_access_int(m::MemoryManager* mgr,
                     std::string agent_name,
                     std::string var_name,
                     size_t pop_size) {
  m::VectorWrapperBase* base_ptr = mgr->GetVectorWrapper(agent_name, var_name);

  BOOST_CHECK(typeid(double) != *(base_ptr->GetDataType()));
  BOOST_CHECK(typeid(int) == *(base_ptr->GetDataType()));

  std::vector<int>* vector_ptr;
  vector_ptr = static_cast<std::vector<int>*>(base_ptr->GetVectorPtr());
  BOOST_CHECK_EQUAL(vector_ptr->capacity(), pop_size);

  vector_ptr->push_back(10);
  vector_ptr->push_back(20);
  BOOST_CHECK_EQUAL(vector_ptr->size(), (size_t)2);
}

void test_access_double(m::MemoryManager* mgr,
                     std::string agent_name,
                     std::string var_name,
                     size_t pop_size) {
  m::VectorWrapperBase* base_ptr = mgr->GetVectorWrapper(agent_name, var_name);

  BOOST_CHECK(typeid(int) != *(base_ptr->GetDataType()));
  BOOST_CHECK(typeid(double) == *(base_ptr->GetDataType()));

  std::vector<double>* vector_ptr;
  vector_ptr = static_cast<std::vector<double>*>(base_ptr->GetVectorPtr());
  BOOST_CHECK_EQUAL(vector_ptr->capacity(), pop_size);

  vector_ptr->push_back(10.1);
  BOOST_CHECK_EQUAL(vector_ptr->size(), (size_t)1);
}

BOOST_AUTO_TEST_CASE(test_anonymous_vector_access) {
  size_t pop_size = 100;
  std::string agent_name = "Circle3", int_var = "x", dbl_var = "y";
  m::MemoryManager& mgr = m::MemoryManager::GetInstance();

  mgr.RegisterAgent(agent_name);
  mgr.RegisterAgentVar<int>(agent_name, int_var);
  mgr.RegisterAgentVar<double>(agent_name, dbl_var);
  mgr.HintPopulationSize(agent_name, pop_size);

  test_access_int(&mgr, agent_name, int_var, pop_size);
  test_access_double(&mgr, agent_name, dbl_var, pop_size);
}

BOOST_AUTO_TEST_CASE(test_reset) {
  m::MemoryManager& mgr1 = m::MemoryManager::GetInstance();
  BOOST_CHECK_THROW(mgr1.RegisterAgent("TestSingleton"), e::logic_error);
  BOOST_CHECK(mgr1.GetAgentCount() != (size_t)0);
  mgr1.Reset();

  m::MemoryManager& mgr = m::MemoryManager::GetInstance();
  BOOST_CHECK_EQUAL(mgr1.GetAgentCount(), (size_t)0);

  BOOST_CHECK_NO_THROW(mgr.RegisterAgent("TestSingleton"));
  BOOST_CHECK_EQUAL(mgr1.GetAgentCount(), (size_t)1);

  mgr.Reset();  // reset again so as not to affect next test suite
  BOOST_CHECK_EQUAL(mgr1.GetAgentCount(), (size_t)0);
}
/*

struct F {  // test fixture
  F() {
    mgr.RegisterAgent("Circle", 10);
    mgr.RegisterAgentVar<int>("Circle", "val");

    // populate vector by accessing hook into private method (test build only)
    std::vector<int> &vec = mgr.GetMemoryVector_test<int>("Circle", "val");
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
  }

  m::MemoryManager mgr;
};

BOOST_FIXTURE_TEST_CASE(test_vector_access_readonly, F) {
  m::VectorReader<int> ro = mgr.GetReader<int>("Circle", "val");
  // check iteration using RO method
  BOOST_CHECK(ro.begin() != ro.end());
  BOOST_CHECK(!ro.empty());
  BOOST_CHECK_EQUAL(ro.size(), (size_t)3);

  int expected[] = {1, 2, 3};
  BOOST_CHECK_EQUAL_COLLECTIONS(expected, expected+3, ro.begin(), ro.end());
  BOOST_CHECK(typeid(ro.begin()) == typeid(std::vector<int>::const_iterator));
}

BOOST_FIXTURE_TEST_CASE(test_vector_access_readwrite, F) {
  // check iteration using RW method
  int expected[] = {1, 2, 3};
  m::VectorWriter<int> rw = mgr.GetWriter<int>("Circle", "val");
  BOOST_CHECK_EQUAL_COLLECTIONS(expected, expected+3, rw.begin(), rw.end());
  BOOST_CHECK(typeid(rw.begin()) == typeid(std::vector<int>::iterator));

  // update values using RW method
  m::VectorWriter<int>::iterator it;
  for (it = rw.begin(); it < rw.end(); ++it) {
    *it *= 2;
  }
  // check updated values
  int expected2[] = {2, 4, 6};
  BOOST_CHECK_EQUAL_COLLECTIONS(expected2, expected2+3, rw.begin(), rw.end());

  // test internal vector using hook into private method (test build only)
  std::vector<int> &vec = mgr.GetMemoryVector_test<int>("Circle", "val");
  BOOST_CHECK_EQUAL_COLLECTIONS(expected2, expected2+3, vec.begin(), vec.end());
}
*/
BOOST_AUTO_TEST_SUITE_END()
