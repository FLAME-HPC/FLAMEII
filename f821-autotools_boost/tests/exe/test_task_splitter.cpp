/*!
 * \file tests/exe/test_task_splitter.cpp
 * \author Shawn Chin
 * \date October 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Test suite for task splitter
 */
#define BOOST_TEST_DYN_LINK
#include <vector>
#include <boost/test/unit_test.hpp>
#include "flame2/exceptions/all.hpp"
#include "flame2/mem/memory_manager.hpp"
#include "flame2/exe/task_manager.hpp"
#include "flame2/exe/task_splitter.hpp"
#include "flame2/compat/C/flame2.h"

BOOST_AUTO_TEST_SUITE(ExeModule)

namespace e = flame::exceptions;
namespace exe = flame::exe;
namespace mem = flame::mem;

FLAME_AGENT_FUNC(dummy_func) {
  return FLAME_AGENT_ALIVE;
}

BOOST_AUTO_TEST_CASE(exe_test_task_split) {
  // Setup Agent Memory
  mem::MemoryManager& mgr_mem = mem::MemoryManager::GetInstance();
  mgr_mem.RegisterAgent("Circle");
  mgr_mem.RegisterAgentVar<int>("Circle", "x");

  // Setup Task
  exe::TaskManager& mgr_task = exe::TaskManager::GetInstance();
  exe::Task &t1 = mgr_task.CreateAgentTask("t1", "Circle", dummy_func);

  // Test split with no agents
  exe::TaskSplitterHandle ts = t1.SplitTask(1, 1);
  BOOST_CHECK(!ts);

  // Populate
  const int POPULATION_SIZE = 100;
  mgr_mem.HintPopulationSize("Circle", (size_t)POPULATION_SIZE);
  std::vector<int> *x = mgr_mem.GetVector<int>("Circle", "x");
  for (int i = 0; i < POPULATION_SIZE; ++i) {
    x->push_back(i);
  }
  mem::AgentShadowPtr shadow = mgr_mem.GetAgentShadow("Circle");
  BOOST_REQUIRE_EQUAL(shadow->get_size(), (size_t)POPULATION_SIZE);

  // split = 1, nothing to do
  ts = t1.SplitTask(1, 1);
  BOOST_CHECK(!ts);

  // Test split with population < 2 * min_vector_size (split violates min size)
  ts = t1.SplitTask(5, 199);
  BOOST_CHECK(!ts);

  // Test split with splits < max_splits
  ts = t1.SplitTask(5, 50);
  BOOST_CHECK(ts);
  BOOST_CHECK(!ts->IsComplete());
  BOOST_CHECK(!ts->NonePending());
  BOOST_CHECK_EQUAL(ts->GetNumTasks(), (size_t)2);

  // Test exact max splits (population = min_vector_size * max_splits)
  ts = t1.SplitTask(5, 20);
  BOOST_CHECK(ts);
  BOOST_CHECK(!ts->IsComplete());
  BOOST_CHECK(!ts->NonePending());
  BOOST_CHECK_EQUAL(ts->GetNumTasks(), (size_t)5);

  // Test max split with task size > min_task_size
  ts = t1.SplitTask(7, 2);
  BOOST_CHECK(ts);
  BOOST_CHECK(!ts->IsComplete());
  BOOST_CHECK(!ts->NonePending());
  BOOST_CHECK_EQUAL(ts->GetNumTasks(), (size_t)7);


  // test IsComplete, NonePending, OneTaskAssigned, OneTaskDone, GetTask
  ts = t1.SplitTask(4, 20);
  BOOST_CHECK_THROW(ts->OneTaskDone(), e::flame_exception);  // nothing pending
  BOOST_CHECK(!ts->OneTaskAssigned());
  BOOST_CHECK(!ts->IsComplete());
  BOOST_CHECK(!ts->NonePending());
  BOOST_CHECK(!ts->OneTaskAssigned());
  BOOST_CHECK(!ts->IsComplete());
  BOOST_CHECK(!ts->NonePending());
  BOOST_CHECK(!ts->OneTaskAssigned());
  BOOST_CHECK(!ts->IsComplete());
  BOOST_CHECK(!ts->NonePending());
  BOOST_CHECK(ts->OneTaskAssigned());
  BOOST_CHECK(!ts->IsComplete());
  BOOST_CHECK(ts->NonePending());

  BOOST_CHECK_THROW(ts->OneTaskAssigned(), e::flame_exception);
  BOOST_CHECK(!ts->OneTaskDone());
  BOOST_CHECK(!ts->IsComplete());
  BOOST_CHECK(ts->NonePending());
  BOOST_CHECK(!ts->OneTaskDone());
  BOOST_CHECK(!ts->IsComplete());
  BOOST_CHECK(ts->NonePending());
  BOOST_CHECK(!ts->OneTaskDone());
  BOOST_CHECK(!ts->IsComplete());
  BOOST_CHECK(ts->NonePending());
  BOOST_CHECK(ts->OneTaskDone());
  BOOST_CHECK(ts->IsComplete());
  BOOST_CHECK(ts->NonePending());
  BOOST_CHECK_THROW(ts->OneTaskDone(), e::flame_exception);  // nothing assigned
  BOOST_CHECK_THROW(ts->OneTaskAssigned(), e::flame_exception);

  mem::MemoryIteratorPtr miter;

  exe::Task& task = ts->GetTask();
  BOOST_CHECK_EQUAL(task.get_task_id(), t1.get_task_id());
  BOOST_CHECK_EQUAL(task.get_task_name(), t1.get_task_name());
  miter = task.GetMemoryIterator();
  BOOST_CHECK_EQUAL(miter->get_size(), (size_t)100);
  BOOST_CHECK_EQUAL(miter->get_count(), (size_t)25);
  BOOST_CHECK_EQUAL(miter->get_offset(), (size_t)0);

  exe::Task& task2  = ts->GetTask();
  BOOST_CHECK_EQUAL(task2.get_task_id(), t1.get_task_id());
  BOOST_CHECK_EQUAL(task2.get_task_name(), t1.get_task_name());
  miter = task2.GetMemoryIterator();
  BOOST_CHECK_EQUAL(miter->get_size(), (size_t)100);
  BOOST_CHECK_EQUAL(miter->get_count(), (size_t)25);
  BOOST_CHECK_EQUAL(miter->get_offset(), (size_t)25);

  exe::Task& task3  = ts->GetTask();
  BOOST_CHECK_EQUAL(task3.get_task_id(), t1.get_task_id());
  BOOST_CHECK_EQUAL(task3.get_task_name(), t1.get_task_name());
  miter = task3.GetMemoryIterator();
  BOOST_CHECK_EQUAL(miter->get_size(), (size_t)100);
  BOOST_CHECK_EQUAL(miter->get_count(), (size_t)25);
  BOOST_CHECK_EQUAL(miter->get_offset(), (size_t)50);

  exe::Task& task4  = ts->GetTask();
  BOOST_CHECK_EQUAL(task4.get_task_id(), t1.get_task_id());
  BOOST_CHECK_EQUAL(task4.get_task_name(), t1.get_task_name());
  miter = task4.GetMemoryIterator();
  BOOST_CHECK_EQUAL(miter->get_size(), (size_t)100);
  BOOST_CHECK_EQUAL(miter->get_count(), (size_t)25);
  BOOST_CHECK_EQUAL(miter->get_offset(), (size_t)75);
  BOOST_CHECK_THROW(ts->GetTask(), e::flame_exception);


  // check with uneven load
  ts = t1.SplitTask(3, 20);
  exe::Task& task1x  = ts->GetTask();
  miter = task1x.GetMemoryIterator();
  BOOST_CHECK_EQUAL(miter->get_size(), (size_t)100);
  BOOST_CHECK_EQUAL(miter->get_count(), (size_t)34);
  BOOST_CHECK_EQUAL(miter->get_offset(), (size_t)0);

  exe::Task& task2x  = ts->GetTask();
  miter = task2x.GetMemoryIterator();
  BOOST_CHECK_EQUAL(miter->get_size(), (size_t)100);
  BOOST_CHECK_EQUAL(miter->get_count(), (size_t)33);
  BOOST_CHECK_EQUAL(miter->get_offset(), (size_t)34);

  exe::Task& task3x  = ts->GetTask();
  miter = task3x.GetMemoryIterator();
  BOOST_CHECK_EQUAL(miter->get_size(), (size_t)100);
  BOOST_CHECK_EQUAL(miter->get_count(), (size_t)33);
  BOOST_CHECK_EQUAL(miter->get_offset(), (size_t)67);

  BOOST_CHECK_THROW(ts->GetTask(), e::flame_exception);  // no more


  // Splitting a split task.
  ts = task2x.SplitTask(2, 1);
  exe::Task& task2x1  = ts->GetTask();
  miter = task2x1.GetMemoryIterator();
  BOOST_CHECK_EQUAL(miter->get_size(), (size_t)100);
  BOOST_CHECK_EQUAL(miter->get_count(), (size_t)17);
  BOOST_CHECK_EQUAL(miter->get_offset(), (size_t)34);

  exe::Task& task2x2  = ts->GetTask();
  miter = task2x2.GetMemoryIterator();
  BOOST_CHECK_EQUAL(miter->get_size(), (size_t)100);
  BOOST_CHECK_EQUAL(miter->get_count(), (size_t)16);
  BOOST_CHECK_EQUAL(miter->get_offset(), (size_t)51);

  BOOST_CHECK_THROW(ts->GetTask(), e::flame_exception);  // no more

  mgr_mem.Reset();
  mgr_task.Reset();
}


BOOST_AUTO_TEST_SUITE_END()
