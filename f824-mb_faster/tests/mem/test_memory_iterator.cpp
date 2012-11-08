/*!
 * \file tests/mem/test_memory_iterator.cpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Test suite for the agent memory iterator
 */
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <vector>
#include "flame2/mem/memory_manager.hpp"
#include "flame2/mem/memory_iterator.hpp"
#include "flame2/exceptions/all.hpp"

BOOST_AUTO_TEST_SUITE(MemModule)

namespace mem = flame::mem;
namespace e = flame::exceptions;

BOOST_AUTO_TEST_CASE(memiter_initialise_memory_manager) {
  mem::MemoryManager& mgr = mem::MemoryManager::GetInstance();
  mgr.RegisterAgent("Circle");
  mgr.RegisterAgentVar<int>("Circle", "x_int");
  mgr.RegisterAgentVar<double>("Circle", "y_dbl");
  mgr.RegisterAgentVar<double>("Circle", "z_dbl");
  mgr.RegisterAgentVar<double>("Circle", "q_dbl");
  mgr.HintPopulationSize("Circle", (size_t)10);
  BOOST_CHECK_EQUAL(mgr.GetAgentCount(), (size_t)1);

  // populate vectors
  std::vector<int>* x_ptr = mgr.GetVector<int>("Circle", "x_int");
  std::vector<double>* y_ptr = mgr.GetVector<double>("Circle", "y_dbl");
  std::vector<double>* z_ptr = mgr.GetVector<double>("Circle", "z_dbl");
  std::vector<double>* q_ptr = mgr.GetVector<double>("Circle", "q_dbl");
  for (int i = 0; i < 10; ++i) {
    x_ptr->push_back(i);
    y_ptr->push_back(i * 1.0);
    z_ptr->push_back(i * 2.0);
    q_ptr->push_back(i * 3.0);
  }
}

BOOST_AUTO_TEST_CASE(memiter_test_noaccess) {
  mem::MemoryManager& mgr = mem::MemoryManager::GetInstance();

  BOOST_CHECK_THROW(mgr.GetAgentShadow("Square"), e::invalid_agent);
  mem::AgentShadowPtr shadow = mgr.GetAgentShadow("Circle");

  // We shouldn't need access to memory to detect population size
  BOOST_CHECK_EQUAL(shadow->get_size(), (size_t)10);

  // Get memory iterator
  mem::MemoryIteratorPtr iptr = shadow->GetMemoryIterator();
  BOOST_CHECK_EQUAL(iptr->get_size(), (size_t)10);
  BOOST_CHECK_EQUAL(iptr->get_position(), (size_t)0);

  // Iterable?
  for (int i = 0; i < 10; i++) {
    BOOST_CHECK_EQUAL(iptr->AtEnd(), false);
    BOOST_CHECK_EQUAL(iptr->get_position(), (size_t)i);
    BOOST_CHECK_EQUAL(iptr->Step(), true);
  }
  // Should now be at end of iteration
  BOOST_CHECK_EQUAL(iptr->Step(), false);
  BOOST_CHECK_EQUAL(iptr->AtEnd(), true);
}

BOOST_AUTO_TEST_CASE(memiter_test_memoryiterator) {
  mem::MemoryManager& mgr = mem::MemoryManager::GetInstance();

  BOOST_CHECK_THROW(mgr.GetAgentShadow("Square"), e::invalid_agent);
  mem::AgentShadowPtr shadow = mgr.GetAgentShadow("Circle");

  // Adding access to invalid var
  BOOST_CHECK_THROW(shadow->AllowAccess("NotVar"), e::invalid_variable);

  // Adding access to vars
  shadow->AllowAccess("x_int");
  shadow->AllowAccess("y_dbl");
  shadow->AllowAccess("z_dbl", true);  // writeable

  // Get memory iterator
  mem::MemoryIteratorPtr iptr = shadow->GetMemoryIterator();
  BOOST_CHECK_EQUAL(iptr->get_size(), (size_t)10);
  BOOST_CHECK_EQUAL(iptr->get_position(), (size_t)0);

  // Accessing var without access permission
  BOOST_CHECK_THROW(iptr->Get<double>("q_dbl"), e::invalid_variable);
  // Accessing invalid var
  BOOST_CHECK_THROW(iptr->Get<double>("NotVar"), e::invalid_variable);

  // retrieving data using wrong type
  BOOST_CHECK_THROW(iptr->Get<double>("x_int"), e::invalid_type);
  BOOST_CHECK_THROW(iptr->GetReadPtr<int>("y_dbl"), e::invalid_type);
  BOOST_CHECK_THROW(iptr->GetWritePtr<int>("z_dbl"), e::invalid_type);

  // Iterate and check content
  for (int i = 0; i < 10; i++) {
    BOOST_CHECK_EQUAL(iptr->AtEnd(), false);
    BOOST_CHECK_EQUAL(iptr->Get<int>("x_int"), i);
    BOOST_CHECK_CLOSE(iptr->Get<double>("y_dbl"), i * 1.0, 0.00001);
    BOOST_CHECK_CLOSE(iptr->Get<double>("z_dbl"), i * 2.0, 0.00001);

    BOOST_CHECK_EQUAL(iptr->get_position(), (size_t)i);
    BOOST_CHECK_EQUAL(iptr->Step(), true);
  }
  // Should now be at end of iteration
  BOOST_CHECK_THROW(iptr->Get<int>("x_int"), e::out_of_range);
  BOOST_CHECK_EQUAL(iptr->Step(), false);
  BOOST_CHECK_EQUAL(iptr->AtEnd(), true);

  // rewind
  iptr->Rewind();
  BOOST_CHECK_EQUAL(iptr->get_position(), (size_t)0);
  BOOST_CHECK_EQUAL(iptr->AtEnd(), false);

  // try to get writeable pointer without permission
  BOOST_CHECK_THROW(iptr->GetWritePtr<int>("x_int"), e::invalid_operation);

  // Get writeable pointer and attempt writing
  double* z_writeable_ptr = iptr->GetWritePtr<double>("z_dbl");
  *z_writeable_ptr = 9.9;
  BOOST_CHECK_EQUAL(iptr->Get<double>("z_dbl"), 9.9);
  *z_writeable_ptr = 0.0;
  BOOST_CHECK_EQUAL(iptr->Get<double>("z_dbl"), 0.0);

  // iterate and write into z_dbl using GetWritePtr<T>()
  iptr->Rewind();
  while (!iptr->AtEnd()) {
    z_writeable_ptr = iptr->GetWritePtr<double>("z_dbl");
    *z_writeable_ptr *= 2;
    BOOST_CHECK_EQUAL(iptr->Step(), true);
  }

  // Iterate and check content
  iptr->Rewind();
  for (int i = 0; i < 10; i++) {
    BOOST_CHECK_EQUAL(iptr->AtEnd(), false);
    BOOST_CHECK_EQUAL(iptr->Get<int>("x_int"), i);
    BOOST_CHECK_CLOSE(iptr->Get<double>("y_dbl"), i * 1.0, 0.00001);
    BOOST_CHECK_CLOSE(iptr->Get<double>("z_dbl"), i * 4.0, 0.00001);

    BOOST_CHECK_EQUAL(iptr->get_position(), (size_t)i);
    BOOST_CHECK_EQUAL(iptr->Step(), true);
  }

  // iterate and write into z_dbl using Set<T>()
  iptr->Rewind();
  while (!iptr->AtEnd()) {
    iptr->Set<double>("z_dbl", 99.9);
    BOOST_CHECK_EQUAL(iptr->Step(), true);
  }

  // Iterate and check content
  iptr->Rewind();
  for (int i = 0; i < 10; i++) {
    BOOST_CHECK_EQUAL(iptr->AtEnd(), false);
    BOOST_CHECK_EQUAL(iptr->Get<int>("x_int"), i);
    BOOST_CHECK_CLOSE(iptr->Get<double>("y_dbl"), i * 1.0, 0.00001);
    BOOST_CHECK_CLOSE(iptr->Get<double>("z_dbl"), 99.9, 0.00001);

    BOOST_CHECK_EQUAL(iptr->get_position(), (size_t)i);
    BOOST_CHECK_EQUAL(iptr->Step(), true);
  }

  // Get a new iterator instead of rewinding
  iptr = shadow->GetMemoryIterator();
  BOOST_CHECK_EQUAL(iptr->get_position(), (size_t)0);
  for (int i = 0; i < 10; i++) {
    BOOST_CHECK_EQUAL(iptr->AtEnd(), false);
    BOOST_CHECK_EQUAL(iptr->Get<int>("x_int"), i);
    BOOST_CHECK_CLOSE(iptr->Get<double>("y_dbl"), i * 1.0, 0.00001);
    BOOST_CHECK_CLOSE(iptr->Get<double>("z_dbl"), 99.9, 0.00001);

    BOOST_CHECK_EQUAL(iptr->get_position(), (size_t)i);
    BOOST_CHECK_EQUAL(iptr->Step(), true);
  }

  // Testing subset iteration
  BOOST_CHECK_THROW(shadow->GetMemoryIterator(11, 1), e::invalid_argument);
  BOOST_CHECK_THROW(shadow->GetMemoryIterator(1, 10), e::invalid_argument);
  int j;

  iptr = shadow->GetMemoryIterator(0, 10);
  BOOST_CHECK_EQUAL(iptr->get_position(), (size_t)0);
  j = 0;
  while (!iptr->AtEnd()) {
    BOOST_CHECK_EQUAL(iptr->Get<int>("x_int"), j);
    iptr->Step();
    j++;
  }
  BOOST_CHECK_EQUAL(j, 10);
  BOOST_CHECK_EQUAL(iptr->get_position(), (size_t)10);

  iptr = shadow->GetMemoryIterator(0, 5);
  BOOST_CHECK_EQUAL(iptr->get_position(), (size_t)0);
  j = 0;
  while (!iptr->AtEnd()) {
    BOOST_CHECK_EQUAL(iptr->Get<int>("x_int"), j);
    iptr->Step();
    j++;
  }
  BOOST_CHECK_EQUAL(j, 5);
  BOOST_CHECK_EQUAL(iptr->get_position(), (size_t)5);

  iptr = shadow->GetMemoryIterator(2, 5);
  BOOST_CHECK_EQUAL(iptr->get_position(), (size_t)0);
  j = 2;
  while (!iptr->AtEnd()) {
    BOOST_CHECK_EQUAL(iptr->Get<int>("x_int"), j);
    iptr->Step();
    j++;
  }
  BOOST_CHECK_EQUAL(j, 7);
  BOOST_CHECK_EQUAL(iptr->get_position(), (size_t)5);

  j = 2;
  iptr->Rewind();
  while (!iptr->AtEnd()) {
    BOOST_CHECK_EQUAL(iptr->Get<int>("x_int"), j);
    iptr->Step();
    j++;
  }
  BOOST_CHECK_EQUAL(j, 7);
  BOOST_CHECK_EQUAL(iptr->get_position(), (size_t)5);
}


BOOST_AUTO_TEST_CASE(memiter_test_sizechange) {
  mem::MemoryManager& mgr = mem::MemoryManager::GetInstance();
  mem::AgentShadowPtr shadow = mgr.GetAgentShadow("Circle");

  // We shouldn't need access to memory to detect population size
  BOOST_CHECK_EQUAL(shadow->get_size(), (size_t)10);

  // Get memory iterator
  mem::MemoryIteratorPtr iptr = shadow->GetMemoryIterator();
  BOOST_CHECK_EQUAL(iptr->get_size(), (size_t)10);
  BOOST_CHECK_EQUAL(iptr->get_position(), (size_t)0);

  // Iterable?
  for (int i = 0; i < 10; i++) {
    BOOST_CHECK_EQUAL(iptr->AtEnd(), false);
    BOOST_CHECK_EQUAL(iptr->get_position(), (size_t)i);
    BOOST_CHECK_EQUAL(iptr->Step(), true);
  }
  // Should now be at end of iteration
  BOOST_CHECK_EQUAL(iptr->Step(), false);
  BOOST_CHECK_EQUAL(iptr->AtEnd(), true);

  // grow the vectors
  std::vector<int>* x_ptr = mgr.GetVector<int>("Circle", "x_int");
  std::vector<double>* y_ptr = mgr.GetVector<double>("Circle", "y_dbl");
  std::vector<double>* z_ptr = mgr.GetVector<double>("Circle", "z_dbl");
  std::vector<double>* q_ptr = mgr.GetVector<double>("Circle", "q_dbl");
  for (int i = 0; i < 10; ++i) {
    x_ptr->push_back(i);
    y_ptr->push_back(i * 1.0);
    z_ptr->push_back(i * 2.0);
    q_ptr->push_back(i * 3.0);
  }

  // using the same shadow, new iterator should see new size
  iptr = shadow->GetMemoryIterator();
  BOOST_CHECK_EQUAL(iptr->get_size(), (size_t)20);
  BOOST_CHECK_EQUAL(iptr->get_position(), (size_t)0);
  // Iterable?
  for (int i = 0; i < 20; i++) {
    BOOST_CHECK_EQUAL(iptr->AtEnd(), false);
    BOOST_CHECK_EQUAL(iptr->get_position(), (size_t)i);
    BOOST_CHECK_EQUAL(iptr->Step(), true);
  }

  // reduce size
  for (int i = 0; i < 15; ++i) {
    x_ptr->pop_back();
    y_ptr->pop_back();
    z_ptr->pop_back();
    q_ptr->pop_back();
  }

  // using the same shadow, new iterator should see new size
  iptr = shadow->GetMemoryIterator();
  BOOST_CHECK_EQUAL(iptr->get_size(), (size_t)5);
  BOOST_CHECK_EQUAL(iptr->get_position(), (size_t)0);
  // Iterable?
  for (int i = 0; i < 5; i++) {
    BOOST_CHECK_EQUAL(iptr->AtEnd(), false);
    BOOST_CHECK_EQUAL(iptr->get_position(), (size_t)i);
    BOOST_CHECK_EQUAL(iptr->Step(), true);
  }
}



BOOST_AUTO_TEST_CASE(memiter_reset_memory_manager) {
  mem::MemoryManager& mgr = mem::MemoryManager::GetInstance();
  mgr.Reset();  // reset again so as not to affect next test suite
  BOOST_CHECK_EQUAL(mgr.GetAgentCount(), (size_t)0);
}


BOOST_AUTO_TEST_SUITE_END()
