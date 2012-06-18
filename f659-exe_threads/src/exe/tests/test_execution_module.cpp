/*!
 * \file src/exe/tests/test_execution_module.cpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Test suite for the execution module
 */
#define BOOST_TEST_DYN_LINK
#include <vector>
#include "boost/test/unit_test.hpp"
#include "mem/memory_manager.hpp"
#include "../task_manager.hpp"
#include "../task_interface.hpp"
#include "../fifo_task_queue.hpp"
#include "../scheduler.hpp"
#include "include/flame.h"

BOOST_AUTO_TEST_SUITE(ExeModule)

namespace exe = flame::exe;
namespace mem = flame::mem;


FLAME_AGENT_FUNC(test_func) {
  int x = flame_mem_get_int("x_int");
  double y = flame_mem_get_double("y_dbl");
  flame_mem_set_double("z_dbl", (x*y));
  return 0;
}

BOOST_AUTO_TEST_CASE(initialise_memory_manager_exemod) {
  mem::MemoryManager& mgr = mem::MemoryManager::GetInstance();
  mgr.RegisterAgent("Circle");
  mgr.RegisterAgentVar<int>("Circle", "x_int");
  mgr.RegisterAgentVar<double>("Circle", "y_dbl");
  mgr.RegisterAgentVar<double>("Circle", "z_dbl");
  mgr.HintPopulationSize("Circle", (size_t)10);
  BOOST_CHECK_EQUAL(mgr.GetAgentCount(), (size_t)1);

  mgr.GetVector<int>("Circle", "x_int")->push_back(1);
  mgr.GetVector<double>("Circle", "y_dbl")->push_back(0.1);
}

BOOST_AUTO_TEST_CASE(test_task_queue) {
  exe::TaskManager& tm = exe::TaskManager::GetInstance();
  tm.CreateAgentTask("t1", "Circle", test_func);
  tm.CreateAgentTask("t2", "Circle", test_func);
  tm.CreateAgentTask("t3", "Circle", test_func);
  tm.CreateAgentTask("t4", "Circle", test_func);
  //tm.AddDependency("t3", "t1");
  //tm.AddDependency("t4", "t1");
  //tm.AddDependency("t4", "t2");
  //tm.AddDependency("t4", "t3");

  /*
  exe::Task::id_type t1 = tm.get_id("t1");  // test-only routine
  exe::Task::id_type t2 = tm.get_id("t2");  // test-only routine
  exe::Task::id_type t3 = tm.get_id("t3");  // test-only routine
  exe::Task::id_type t4 = tm.get_id("t4");  // test-only routine
  */


  exe::Scheduler s;
  exe::Scheduler::QueueId q = s.CreateQueue<exe::FIFOTaskQueue>(2);
  s.AssignType(q, exe::Task::AGENT_FUNCTION);
  s.RunIteration();

  tm.Reset();
}

BOOST_AUTO_TEST_CASE(reset_memory_manager_exemod) {
  mem::MemoryManager& mgr = mem::MemoryManager::GetInstance();
  mgr.Reset();  // reset again so as not to affect next test suite
  BOOST_CHECK_EQUAL(mgr.GetAgentCount(), (size_t)0);
}

/*
BOOST_AUTO_TEST_CASE(runexe_setup) {

  m::MemoryManager& mm = m::MemoryManager::GetInstance();
  e::TaskManager& tm = e::TaskManager::GetInstance();

  // Register agent
  mm.RegisterAgent("Circle");
  mm.RegisterAgentVar<int>("Circle", "x_int");
  mm.RegisterAgentVar<double>("Circle", "y_dbl");
  mm.RegisterAgentVar<double>("Circle", "z_dbl");
  mm.HintPopulationSize("Circle", 10);

  // Populate data
  std::vector<int>* x_ptr = mm.GetVector<int>("Circle", "x_int");
  std::vector<double>* y_ptr = mm.GetVector<double>("Circle", "y_dbl");
  std::vector<double>* z_ptr = mm.GetVector<double>("Circle", "z_dbl");
  for (int i = 0; i < 10; ++i) {
    x_ptr->push_back(i);
    y_ptr->push_back(i * 2.0);
    z_ptr->push_back(0.0);
  }

  // Register task
  e::Task& t = tm.CreateAgentTask("test", "Circle", test_func);
  t.AllowAccess("x_int");
  t.AllowAccess("y_dbl");
  t.AllowAccess("z_dbl", true);  // writeable
}

BOOST_AUTO_TEST_CASE(runexe_run) {
  flame::exe::ExecutionThread exe_thread;
  exe_thread.Run("test");

  e::TaskManager& tm = e::TaskManager::GetInstance();
  e::Task& task = tm.GetTask("test");
  m::MemoryIteratorPtr m_iter = task.GetMemoryIterator();
  BOOST_CHECK_EQUAL(m_iter->get_size(), 10);

  for (int i = 0; i < 10; ++i) {
    BOOST_CHECK_EQUAL(m_iter->AtEnd(), false);
    BOOST_CHECK_EQUAL(m_iter->Get<int>("x_int"), i);
    BOOST_CHECK_EQUAL(m_iter->Get<double>("y_dbl"), i * 2.0);
    BOOST_CHECK_EQUAL(m_iter->Get<double>("z_dbl"), i * i * 2.0);
    m_iter->Step();
  }
}

BOOST_AUTO_TEST_CASE(runexe_cleanup) {
  e::TaskManager::GetInstance().Reset();
  m::MemoryManager::GetInstance().Reset();
}

*/
BOOST_AUTO_TEST_SUITE_END()
