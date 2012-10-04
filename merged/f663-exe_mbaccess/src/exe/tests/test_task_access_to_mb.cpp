/*!
 * \file src/exe/tests/test_task_access_to_mb.cpp
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
#include "mb/message_board_manager.hpp"
#include "exe/task_manager.hpp"
#include "../scheduler.hpp"
#include "../fifo_task_queue.hpp"
#include "include/flame.h"

BOOST_AUTO_TEST_SUITE(ExeModule)

namespace exe = flame::exe;
namespace mem = flame::mem;
namespace mb = flame::mb;

const int AGENT_COUNT = 100;

FLAME_AGENT_FUNC(func_post_message) {
  location_message msg;
  msg.x = flame_mem_get_double("x");
  msg.y = flame_mem_get_double("y");
  msg.z = flame_mem_get_double("z");
  msg.id = flame_mem_get_int("id");

  flame_msg_post("location", &msg);
  return FLAME_AGENT_ALIVE;
}

FLAME_AGENT_FUNC(func_read_message) {
  int checksum = 0;
  flame_msg_iterator iter;
  location_message msg;

  iter = flame_msg_get_iterator("location");
  for (; !flame_msg_iterator_end(iter); flame_msg_iterator_next(iter)) {
    flame_msg_iterator_get_message(iter, &msg);
    checksum += msg.id;
  }
  flame_msg_iterator_free(iter);

  flame_mem_set_int("checksum", checksum);
  return FLAME_AGENT_ALIVE;
}

BOOST_AUTO_TEST_CASE(exe_test_msg_post) {
  // Define agents and population
  mem::MemoryManager& mem_mgr = mem::MemoryManager::GetInstance();
  mem_mgr.RegisterAgent("Circle");
  mem_mgr.RegisterAgentVar<double>("Circle", "x");
  mem_mgr.RegisterAgentVar<double>("Circle", "y");
  mem_mgr.RegisterAgentVar<double>("Circle", "z");
  mem_mgr.RegisterAgentVar<int>("Circle", "id");
  mem_mgr.RegisterAgentVar<int>("Circle", "checksum");
  mem_mgr.HintPopulationSize("Circle", (size_t)AGENT_COUNT);

  std::vector<double> *x = mem_mgr.GetVector<double>("Circle", "x");
  std::vector<double> *y = mem_mgr.GetVector<double>("Circle", "y");
  std::vector<double> *z = mem_mgr.GetVector<double>("Circle", "z");
  std::vector<int> *id = mem_mgr.GetVector<int>("Circle", "id");
  std::vector<int> *cs = mem_mgr.GetVector<int>("Circle", "checksum");

  for (int i = 0; i < AGENT_COUNT; ++i) {
    x->push_back(i * 1.0);
    y->push_back(i * 10.0);
    z->push_back(i * 100.0);
    id->push_back(i);
    cs->push_back(0);
  }

  // Register Message Board
  mb::MessageBoardManager& mb_mgr = mb::MessageBoardManager::GetInstance();
  mb_mgr.RegisterMessage("location");
  mb_mgr.RegisterMessageVar<location_message>("location",
                                              FLAME_MESSAGE_VARNAME);
  // --------------- DEPLOY HACK -------------
  flame_mb_api_hack_initialise();  // assign "location" to type location_message

  // Register Task
  exe::TaskManager& tm = exe::TaskManager::GetInstance();
  exe::Task &t1 = tm.CreateAgentTask("post", "Circle", func_post_message);
  t1.AllowAccess("x");
  t1.AllowAccess("y");
  t1.AllowAccess("z");
  t1.AllowAccess("id");
  t1.AllowMessagePost("location");

  exe::Task &ts = tm.CreateMessageBoardTask("sync", "location",
                                            exe::MessageBoardTask::OP_SYNC);
  exe::Task &tc = tm.CreateMessageBoardTask("clear", "location",
                                            exe::MessageBoardTask::OP_CLEAR);

  exe::Task &t2 = tm.CreateAgentTask("read", "Circle", func_read_message);
  t2.AllowAccess("checksum", true);
  t2.AllowMessageRead("location");

  // define task dependencies
  tm.AddDependency("sync", "post");
  tm.AddDependency("read", "sync");
  tm.AddDependency("clear", "read");

  // Run
  exe::Scheduler s;
  exe::Scheduler::QueueId q = s.CreateQueue<exe::FIFOTaskQueue>(4);
  s.AssignType(q, exe::Task::AGENT_FUNCTION);
  s.AssignType(q, exe::Task::MB_FUNCTION);
  s.RunIteration();

  // Check checksum for each agent. This tells use that all agents
  // did post their id as a message and later read in all the messages
  int sum = 0;
  for (int i = 0; i < AGENT_COUNT; ++i) {
    sum += i;
  }
  for (int i = 0; i < AGENT_COUNT; ++i) {
    BOOST_CHECK_EQUAL(cs->at(i), sum);
  }

  // Board should have been cleared in the end
  BOOST_CHECK_EQUAL(mb_mgr.GetCount("location"), (size_t)0);

  mem_mgr.Reset();
  mb_mgr.Reset();
  tm.Reset();
}

BOOST_AUTO_TEST_SUITE_END()
