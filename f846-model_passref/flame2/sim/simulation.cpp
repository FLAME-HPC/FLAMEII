/*!
 * \file flame2/sim/simulation.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Simulation: holds and manages simulation information
 */
#include <string>
#include <cstdio>
#include "flame2/config.hpp"
#include "flame2/io/io_manager.hpp"
#include "flame2/exe/fifo_task_queue.hpp"
#include "flame2/exe/scheduler.hpp"
#include "flame2/exceptions/sim.hpp"
#include "simulation.hpp"

namespace flame { namespace sim {

typedef std::pair<std::string, std::string> AgentVar;
typedef std::map<std::string, std::set<AgentVar> > AgentMemory;
typedef std::set< std::pair<std::string, std::string> > StringPairSet;
typedef std::set<std::string> StringSet;

Simulation::Simulation(const flame::model::Model &model, std::string pop_file) {
  flame::io::IOManager& iomanager = flame::io::IOManager::GetInstance();

  // check model has been validated
  if (!model.isValidated()) throw flame::exceptions::flame_sim_exception(
      "Model added to a simulation has not been validated");

  // register model with memory and task manager
  registerModelWithMessageManager(model);
  registerModelWithMemoryManager(model);
  registerModelWithTaskManager(model);

  // remember to uncomment and fix
  //iomanager.readPop(pop_file, model, io::IOManager::xml);
}

void Simulation::start(size_t iterations, size_t num_cores) {
  exe::Scheduler s;
  exe::Scheduler::QueueId q = s.CreateQueue<exe::FIFOTaskQueue>(num_cores);
  s.AssignType(q, exe::Task::AGENT_FUNCTION);
  s.AssignType(q, exe::Task::MB_FUNCTION);
  s.AssignType(q, exe::Task::IO_FUNCTION);

  unsigned int ii;
  for (ii = 1; ii <= iterations; ++ii) {
#ifndef TESTBUILD
    printf("Iteration - %u\n", ii);
#endif
    s.RunIteration();
  }
}

void Simulation::registerModelWithMessageManager(const flame::model::Model &model) {
}

void Simulation::registerModelWithMemoryManager(const flame::model::Model &model) {
  AgentMemory::iterator agent;
  std::set<AgentVar>::iterator var;

  // get memory manager
  flame::mem::MemoryManager& memoryManager =
              flame::mem::MemoryManager::GetInstance();
  // get agent memory info
  AgentMemory agentMemory = model.getAgentMemoryInfo();

  for (agent = agentMemory.begin(); agent != agentMemory.end(); ++agent) {
    // printf("memoryManager.RegisterAgent %s\n", (*agent).first.c_str());
    // register agent with memory manager
    memoryManager.RegisterAgent((*agent).first);
    // register agent memory variables
    for (var = (*agent).second.begin(); var != (*agent).second.end(); ++var) {
      // register int variable
      if ((*var).first == "int")
        memoryManager.RegisterAgentVar<int>((*agent).first, (*var).second);
      // register double variable
      else if ((*var).first == "double")
        memoryManager.RegisterAgentVar<double>((*agent).first, (*var).second);
    }
    // population Size hint
    memoryManager.HintPopulationSize((*agent).first, 100);

  }
}

void Simulation::registerModelWithTaskManager(const flame::model::Model &model) {
  flame::exe::TaskManager& taskManager = exe::TaskManager::GetInstance();
  StringPairSet::iterator it;
  StringPairSet agentTasks = model.getAgentTasks();

  for (it = agentTasks.begin(); it != agentTasks.end(); ++it) {
    flame::exe::Task& task = taskManager.CreateAgentTask(
      (*it).first, (*it).second, model.getAgentFunctionPointer((*it).first));
//    StringSet rov = model.getReadOnlyVariables((*it).second, (*it).first);
//    StringSet rwv = model.getWriteVariables((*it).second, (*it).first);
//    StringSet out = model.getOutputMessages((*it).second, (*it).first);
//    StringSet in = model.getInputMessages((*it).second, (*it).first);
  }


  StringPairSet ioTasks = model.getIOTasks();
  StringPairSet messageboardTasks = model.getMessageBoardTasks();
}

#ifdef TESTBUILD
void Simulation::registerModelWithMemoryManagerTest(const flame::model::Model &model) {
  registerModelWithMemoryManager(model);
}

void Simulation::registerModelWithTaskManagerTest(const flame::model::Model &model) {
  registerModelWithTaskManager(model);
}
#endif

}}  // namespace flame::sim
