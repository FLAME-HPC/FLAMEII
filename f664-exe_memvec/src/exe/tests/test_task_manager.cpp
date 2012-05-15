/*!
 * \file src/exe/tests/test_task_manager.cpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Test suite for task manager
 */
#define BOOST_TEST_DYN_LINK
#include <vector>
#include "boost/test/unit_test.hpp"
#include "../task_manager.hpp"

BOOST_AUTO_TEST_SUITE(TaskManager)

namespace exe = flame::exe;
namespace mem = flame::mem;
BOOST_AUTO_TEST_CASE(initialise_memory_manager) {
  mem::MemoryManager& mgr = mem::MemoryManager::GetInstance();
  mgr.RegisterAgent("Circle");
  mgr.RegisterAgentVar<int>("Circle", "x_int");
  mgr.RegisterAgentVar<double>("Circle", "y_dbl");
  mgr.HintPopulationSize("Circle", (size_t)10);
  BOOST_CHECK_EQUAL(mgr.GetAgentCount(), (size_t)1);

  mgr.GetVector<int>("Circle", "x_int")->push_back(1);
  mgr.GetVector<double>("Circle", "y_dbl")->push_back(0.1);
}

// dummy function
FLAME_AGENT_FUNC(func1) { return 0; }

BOOST_AUTO_TEST_CASE(test_create_task) {
  exe::TaskManager tm;
  BOOST_CHECK_THROW(tm.CreateTask("t1", "NotAnAgent", &func1),
                    flame::exceptions::invalid_agent);
  BOOST_CHECK_THROW(tm.CreateTask("t1", "Circle", NULL),
                    flame::exceptions::invalid_argument);

  exe::Task& task = tm.CreateTask("t1", "Circle", &func1);
  mem::MemoryIteratorPtr m_iter = task.get_memory_iterator();
  m_iter->AllowAccess("x_int");
  m_iter->AllowAccess("y_dbl", true);
  BOOST_CHECK_THROW(m_iter->AllowAccess("NotAVar"),
                    flame::exceptions::invalid_variable);

  BOOST_CHECK_THROW(tm.GetTask("NotATask"),
                    flame::exceptions::invalid_argument);
  exe::Task& t1 = tm.GetTask("t1");
  BOOST_CHECK_EQUAL(t1.get_task_name(), "t1");
}

BOOST_AUTO_TEST_CASE(reset_memory_manager) {
  mem::MemoryManager& mgr = mem::MemoryManager::GetInstance();
  mgr.Reset();  // reset again so as not to affect next test suite
  BOOST_CHECK_EQUAL(mgr.GetAgentCount(), (size_t)0);
}

BOOST_AUTO_TEST_SUITE_END()
