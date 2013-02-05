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
#include <set>
#include <map>
#include <utility>
#include <vector>
#include <cstdio>
#include "flame2/config.hpp"
#include "flame2/io/io_manager.hpp"
#include "flame2/exe/fifo_task_queue.hpp"
#include "flame2/exe/scheduler.hpp"
#include "flame2/exceptions/sim.hpp"
#include "simulation.hpp"

namespace flame { namespace sim {

namespace exe = flame::exe;

typedef std::set< std::pair<std::string, std::string> > StringPairSet;
typedef std::set<std::string> StringSet;

typedef size_t TaskId;
typedef std::set<TaskId> TaskIdSet;
typedef std::map<TaskId, TaskId> TaskIdMap;

typedef std::pair<std::string, std::string> Var;
typedef std::vector<Var> VarVec;
typedef std::map<std::string, VarVec> AgentMemory;

Simulation::Simulation(const m::Model &model, std::string pop_file) {
  flame::io::IOManager& iomanager = flame::io::IOManager::GetInstance();

  // check model has been validated
  if (!model.isValidated()) throw flame::exceptions::flame_sim_exception(
      "Model added to a simulation has not been validated");

  // register model with memory and task manager
  registerModelWithMessageManager(model);
  registerModelWithMemoryManager(model.getAgentMemoryInfo());
  registerModelWithTaskManager(model);

  // read pop data
  iomanager.readPop(pop_file, model.getAgentMemoryInfo(), io::IOManager::xml);
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

void Simulation::registerModelWithMessageManager(const m::Model &/*model*/) {
}

void Simulation::registerModelWithMemoryManager(
    const AgentMemory& agentMemory) {
  AgentMemory::const_iterator agent;
  VarVec::const_iterator var;

  // get memory manager
  flame::mem::MemoryManager& memoryManager =
              flame::mem::MemoryManager::GetInstance();

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

void registerAllowAccess(exe::Task * task,
    StringSet vars, bool writeable) {
  StringSet::iterator sit;

  // For each variable name
  for (sit = vars.begin(); sit != vars.end(); ++sit)
    task->AllowAccess((*sit), writeable);
}

void registerAllowMessage(exe::Task * task,
    StringSet messages, bool post) {
  StringSet::iterator sit;

  // For each message
  for (sit = messages.begin();
      sit != messages.end(); ++sit) {
    // Update task with appropriate access
    // If output message then allow post
    if (post) task->AllowMessagePost((*sit));
    // If input message then allow read
    else
      task->AllowMessageRead((*sit));
  }
}

void Simulation::registerAgentTasksWithTaskManager(const m::Model &model) {
  exe::TaskManager& taskManager = exe::TaskManager::GetInstance();
  TaskIdSet::iterator it;

  TaskIdSet agentTasks = model.getAgentTasks();
  for (it = agentTasks.begin(); it != agentTasks.end(); ++it) {
    TaskId id = (*it);
    flame::exe::Task& task = taskManager.CreateAgentTask(
      model.getTaskName(id),
      model.getTaskAgentName(id),
      model.getAgentFunctionPointer(model.getTaskFunctionName(id)));
    registerAllowAccess(&task, model.getTaskReadOnlyVariables(id), false);
    registerAllowAccess(&task, model.getTaskWriteVariables(id), true);
    registerAllowMessage(&task, model.getTaskOutputMessages(id), true);
    registerAllowMessage(&task, model.getTaskInputMessages(id), false);
  }
}

void Simulation::registerIOTasksWithTaskManager(const m::Model &model) {
  exe::TaskManager& taskManager = exe::TaskManager::GetInstance();
  TaskIdSet::iterator it;

  // get model io tasks
  TaskIdSet ioTasks = model.getAgentIOTasks();
  for (it = ioTasks.begin(); it != ioTasks.end(); ++it) {
    TaskId id = (*it);
    // get task agent name
    std::string agent_name = model.getTaskAgentName(id);
    StringSet::iterator sit;
    // get task writing variables
    StringSet vars = model.getTaskWriteVariables(id);
    // for each variable create a new IO task
    for (sit = vars.begin(); sit != vars.end(); ++sit) {
      std::string var = *sit;
      std::string taskName = "AD_";
      taskName.append(agent_name);
      taskName.append("_");
      taskName.append(var);
      taskManager.CreateIOTask(taskName, agent_name, var,
          flame::exe::IOTask::OP_OUTPUT);
    }
  }

  TaskId initIOTask = model.getInitIOTask();
  taskManager.CreateIOTask(model.getTaskName(initIOTask), "", "",
          flame::exe::IOTask::OP_INIT);
  TaskId finIOTask = model.getFinIOTask();
  taskManager.CreateIOTask(model.getTaskName(finIOTask), "", "",
          flame::exe::IOTask::OP_FIN);
}

void Simulation::registerMBTasksWithTaskManager(const m::Model &model) {
  exe::TaskManager& taskManager = exe::TaskManager::GetInstance();
  TaskIdSet::iterator it;

  // get model MB sync tasks
  TaskIdSet messageboardSyncTasks = model.getMessageBoardSyncTasks();
  for (it = messageboardSyncTasks.begin();
      it != messageboardSyncTasks.end(); ++it) {
    TaskId id = (*it);
    std::string message_name = model.getTaskFunctionName(id);
    std::string task_name = model.getTaskName(id);
    // register sync task
    taskManager.CreateMessageBoardTask(task_name, message_name,
            exe::MessageBoardTask::OP_SYNC);
  }

  // get model MB clear tasks
  TaskIdSet messageboardClearTasks = model.getMessageBoardClearTasks();
  for (it = messageboardClearTasks.begin();
      it != messageboardClearTasks.end(); ++it) {
    TaskId id = (*it);
    std::string message_name = model.getTaskFunctionName(id);
    std::string task_name = model.getTaskName(id);
    // register clear task
    taskManager.CreateMessageBoardTask(task_name, message_name,
            exe::MessageBoardTask::OP_CLEAR);
  }
}

void Simulation::registerModelWithTaskManager(const m::Model &model) {
  exe::TaskManager& taskManager = exe::TaskManager::GetInstance();

  // register agent tasks
  registerAgentTasksWithTaskManager(model);
  // register IO tasks
  registerIOTasksWithTaskManager(model);
  // register MB tasks
  registerMBTasksWithTaskManager(model);

  // register task dependencies
  TaskIdMap dependencies = model.getTaskDependencies();
  TaskIdMap::iterator mit;
  for (mit = dependencies.begin(); mit != dependencies.end(); ++mit)
    taskManager.AddDependency(model.getTaskName((*mit).first),
        model.getTaskName((*mit).second));

  // once finalised, tasks and dependencies can no longer be added
  taskManager.Finalise();
}

#ifdef TESTBUILD
void Simulation::registerModelWithMemoryManagerTest(
    const AgentMemory& agentMemory) {
  registerModelWithMemoryManager(agentMemory);
}

void Simulation::registerModelWithTaskManagerTest(const m::Model &model) {
  registerModelWithTaskManager(model);
}
#endif

}}  // namespace flame::sim
