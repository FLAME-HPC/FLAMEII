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

void Simulation::registerAgentTaskWithTaskManager(
    m::Task * task, const m::Model &model) {
  exe::TaskManager& taskManager = exe::TaskManager::GetInstance();

  flame::exe::Task& exetask = taskManager.CreateAgentTask(
    task->getTaskName(),
    task->getParentName(),
    model.getAgentFunctionPointer(task->getName()));
  registerAllowAccess(&exetask, task->getReadOnlyVariablesConst(), false);
  registerAllowAccess(&exetask, task->getWriteVariablesConst(), true);
  registerAllowMessage(&exetask, task->getOutputMessagesConst(), true);
  registerAllowMessage(&exetask, task->getInputMessagesConst(), false);
}

void Simulation::registerIOTaskWithTaskManager(m::Task * task) {
  exe::TaskManager& taskManager = exe::TaskManager::GetInstance();

  // get task agent name
  std::string agent_name = task->getParentName();
  StringSet::iterator sit;
  // get task writing variables
  StringSet vars = task->getWriteVariablesConst();
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

void Simulation::registerMBTaskWithTaskManager(m::Task * task, int taskType) {
  exe::TaskManager& taskManager = exe::TaskManager::GetInstance();

  std::string message_name = task->getName();
  std::string task_name = task->getTaskName();
  // register sync task
  if (taskType == 0)
    taskManager.CreateMessageBoardTask(task_name, message_name,
          exe::MessageBoardTask::OP_SYNC);
  // register clear task
  if (taskType == 1)
  taskManager.CreateMessageBoardTask(task_name, message_name,
          exe::MessageBoardTask::OP_CLEAR);
}

void Simulation::registerModelWithTaskManager(const m::Model &model) {
  exe::TaskManager& taskManager = exe::TaskManager::GetInstance();
  size_t ii;

  const m::TaskList * tasklist = model.getTaskList();
  for (ii = 0; ii < tasklist->getTaskCount(); ++ii) {
    m::Task * task = tasklist->getTask(ii);
    m::Task::TaskType type = task->getTaskType();
    // If agent task
    if (type == m::Task::xfunction || type == m::Task::xcondition)
      registerAgentTaskWithTaskManager(task, model);
    // if data tast
    if (type == m::Task::io_pop_write) registerIOTaskWithTaskManager(task);
    // if init io task
    if (type == m::Task::start_model)
      taskManager.CreateIOTask(task->getTaskName(), "", "",
                flame::exe::IOTask::OP_INIT);
    // if final io task
    if (type == m::Task::finish_model)
      taskManager.CreateIOTask(task->getTaskName(), "", "",
                flame::exe::IOTask::OP_FIN);
    // if message sync task
    if (type == m::Task::xmessage_sync) registerMBTaskWithTaskManager(task, 0);
    // if message clear task
    if (type == m::Task::xmessage_clear)
      registerMBTaskWithTaskManager(task, 1);
  }

  // register task dependencies
  TaskIdMap dependencies = model.getTaskDependencies();
  TaskIdMap::iterator mit;
  for (mit = dependencies.begin(); mit != dependencies.end(); ++mit)
    taskManager.AddDependency(
        tasklist->getTask((*mit).first)->getTaskName(),
        tasklist->getTask((*mit).second)->getTaskName());

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
