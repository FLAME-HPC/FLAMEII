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
#include <boost/test/unit_test.hpp>
#include "flame2/mem/memory_manager.hpp"
#include "flame2/exe/task_manager.hpp"
#include "flame2/exe/task_interface.hpp"
#include "flame2/exe/fifo_task_queue.hpp"
#include "flame2/exe/splitting_fifo_task_queue.hpp"
#include "flame2/exe/scheduler.hpp"
#include "flame2/compat/C/flame2.h"

BOOST_AUTO_TEST_SUITE(ExeModule)

namespace exe = flame::exe;
namespace mem = flame::mem;

const int AGENT_COUNT = 5000;

FLAME_AGENT_FUNC(func_Y_10X) {
  int x = flame_mem_get_int("x_int");
  flame_mem_set_double("y_dbl", (x * 10.0));
  return FLAME_AGENT_ALIVE;
}

FLAME_AGENT_FUNC(func_Z_Xp1) {
  int x = flame_mem_get_int("x_int");
  flame_mem_set_double("z_dbl", (x + 1.0));
  return FLAME_AGENT_ALIVE;
}

FLAME_AGENT_FUNC(func_Y_XpY) {
  int x = flame_mem_get_int("x_int");
  double y = flame_mem_get_double("y_dbl");
  flame_mem_set_double("y_dbl", (x + y));
  return FLAME_AGENT_ALIVE;
}


FLAME_AGENT_FUNC(func_X_YpZ) {
  double y = flame_mem_get_double("y_dbl");
  double z = flame_mem_get_double("z_dbl");
  flame_mem_set_int("x_int", static_cast<int>(y+z));
  return FLAME_AGENT_ALIVE;
}

BOOST_AUTO_TEST_CASE(initialise_memory_manager_exemod) {
  mem::MemoryManager& mgr = mem::MemoryManager::GetInstance();
  mgr.RegisterAgent("Circle");
  mgr.RegisterAgentVar<int>("Circle", "x_int");
  mgr.RegisterAgentVar<double>("Circle", "y_dbl");
  mgr.RegisterAgentVar<double>("Circle", "z_dbl");
  mgr.HintPopulationSize("Circle", (size_t)AGENT_COUNT);
  BOOST_CHECK_EQUAL(mgr.GetAgentCount(), (size_t)1);

  std::vector<int> *x = mgr.GetVector<int>("Circle", "x_int");
  std::vector<double> *y = mgr.GetVector<double>("Circle", "y_dbl");
  std::vector<double> *z = mgr.GetVector<double>("Circle", "z_dbl");

  for (int i = 0; i < AGENT_COUNT; ++i) {
    x->push_back(i);
    y->push_back(0.0);
    z->push_back(0.0);
  }

  mem::AgentShadowPtr shadow = mgr.GetAgentShadow("Circle");
  shadow->AllowAccess("x_int");
  BOOST_CHECK_EQUAL(shadow->get_size(), (size_t)AGENT_COUNT);
}

BOOST_AUTO_TEST_CASE(test_task_queue) {
  exe::TaskManager& tm = exe::TaskManager::GetInstance();
  // t1 : y = x * 10
  exe::Task &t1 = tm.CreateAgentTask("t1", "Circle", func_Y_10X);
  t1.AllowAccess("x_int");
  t1.AllowAccess("y_dbl", true);  // write access to y

  // t2 : z = x + 1
  exe::Task &t2 = tm.CreateAgentTask("t2", "Circle", func_Z_Xp1);
  t2.AllowAccess("x_int");
  t2.AllowAccess("z_dbl", true);  // write access to z

  // t3 : y = y + x
  exe::Task &t3 = tm.CreateAgentTask("t3", "Circle", func_Y_XpY);
  t3.AllowAccess("x_int");
  t3.AllowAccess("y_dbl", true);  // write access to y

  // t4 : x = y + z
  exe::Task &t4 = tm.CreateAgentTask("t4", "Circle", func_X_YpZ);
  t4.AllowAccess("z_dbl");
  t4.AllowAccess("y_dbl");
  t4.AllowAccess("x_int", true);  // write access to x

  // define task dependencies
  tm.AddDependency("t3", "t1");
  tm.AddDependency("t4", "t1");
  tm.AddDependency("t4", "t2");
  tm.AddDependency("t4", "t3");

  exe::Scheduler s;
  exe::Scheduler::QueueId q = s.CreateQueue<exe::SplittingFIFOTaskQueue>(4);
  s.AssignType(q, exe::Task::AGENT_FUNCTION);
  s.SetSplittable(exe::Task::AGENT_FUNCTION);
  s.RunIteration();


  // check content
  // assuming the functions are run the the correct order, the results
  // should be:
  // x = 12i + 1
  // y = 11i
  // z = i + i
  mem::MemoryManager& mgr = mem::MemoryManager::GetInstance();
  mem::AgentShadowPtr shadow = mgr.GetAgentShadow("Circle");
  shadow->AllowAccess("x_int");
  shadow->AllowAccess("y_dbl");
  shadow->AllowAccess("z_dbl");
  BOOST_CHECK_EQUAL(shadow->get_size(), (size_t)AGENT_COUNT);
  mem::MemoryIteratorPtr mptr = shadow->GetMemoryIterator();
  BOOST_CHECK(!mptr->AtEnd());
  for (int i = 0; i < AGENT_COUNT; i++) {
    BOOST_CHECK_EQUAL(mptr->Get<int>("x_int"), 12*i + 1);
    BOOST_CHECK_CLOSE(mptr->Get<double>("y_dbl"), 11*i, 0.00001);
    BOOST_CHECK_CLOSE(mptr->Get<double>("z_dbl"), i+1, 0.00001);
    mptr->Step();
  }
  BOOST_CHECK(mptr->AtEnd());


  // run another iteration. Results should now be
  // x = 144i + 13
  // y = 132i + 11
  // z = 12i + 2
  s.RunIteration();
  mptr->Rewind();
  for (int i = 0; i < AGENT_COUNT; i++) {
    BOOST_CHECK_EQUAL(mptr->Get<int>("x_int"), 144*i + 13);
    BOOST_CHECK_CLOSE(mptr->Get<double>("y_dbl"), 132*i + 11, 0.00001);
    BOOST_CHECK_CLOSE(mptr->Get<double>("z_dbl"), 12*i + 2, 0.00001);
    mptr->Step();
  }
  BOOST_CHECK(mptr->AtEnd());
}

BOOST_AUTO_TEST_CASE(reset_memory_manager_exemod) {
  mem::MemoryManager& mgr = mem::MemoryManager::GetInstance();
  mgr.Reset();  // reset again so as not to affect next test suite
  BOOST_CHECK_EQUAL(mgr.GetAgentCount(), (size_t)0);

  exe::TaskManager& tm = exe::TaskManager::GetInstance();
  tm.Reset();
}

BOOST_AUTO_TEST_SUITE_END()
