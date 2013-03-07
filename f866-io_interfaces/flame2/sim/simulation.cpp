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

typedef std::pair<std::string, std::string> Var;
typedef std::vector<Var> VarVec;

Simulation::Simulation(const m::Model &model, std::string pop_file) {
  flame::io::IOManager& iomanager = flame::io::IOManager::GetInstance();

  // check model has been validated
  if (!model.isValidated()) throw flame::exceptions::flame_sim_exception(
      "Model added to a simulation has not been validated");

  // register model with memory and task manager
  registerModelWithMemoryManager(model.getAgentMemoryInfo());
  registerModelWithTaskManager(model);

  iomanager.registerIOPlugins();
  iomanager.setInputType("xml");
  iomanager.setOutputType("xml");
  // tell io manager the agent memory info
  iomanager.setAgentMemoryInfo(model.getAgentMemoryInfo());
  // read pop data
  iomanager.readPop(pop_file);
}

void Simulation::start(size_t iterations, size_t num_cores) {
  // create a scheduler
  exe::Scheduler s;
  // add a FIFO queue to the scheduler with given num cores
  exe::Scheduler::QueueId q = s.CreateQueue<exe::FIFOTaskQueue>(num_cores);
  // add all task types to be handled by the queue
  s.AssignType(q, exe::Task::AGENT_FUNCTION);
  s.AssignType(q, exe::Task::MB_FUNCTION);
  s.AssignType(q, exe::Task::IO_FUNCTION);

  // run scheduler iteration for num iterations
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

  // for each variable name
  for (sit = vars.begin(); sit != vars.end(); ++sit)
    task->AllowAccess((*sit), writeable);
}

void registerAllowMessage(exe::Task * task,
    StringSet messages, bool post) {
  StringSet::iterator sit;

  // for each message
  for (sit = messages.begin();
      sit != messages.end(); ++sit) {
    // update task with appropriate access
    // if output message then allow post
    if (post) task->AllowMessagePost((*sit));
    // if input message then allow read
    else
      task->AllowMessageRead((*sit));
  }
}

void Simulation::registerAgentTaskWithTaskManager(
    m::ModelTask * task, const m::Model &model) {
  exe::TaskManager& taskManager = exe::TaskManager::GetInstance();

  // create agent task
  flame::exe::Task& exetask = taskManager.CreateAgentTask(
    task->getTaskName(),
    task->getParentName(),
    model.getAgentFunctionPointer(task->getName()));
  // register read only and write variables
  registerAllowAccess(&exetask, task->getReadOnlyVariablesConst(), false);
  registerAllowAccess(&exetask, task->getWriteVariablesConst(), true);
  // register message output and input
  registerAllowMessage(&exetask, task->getOutputMessagesConst(), true);
  registerAllowMessage(&exetask, task->getInputMessagesConst(), false);
}

void Simulation::registerMBTaskWithTaskManager(
    m::ModelTask * task, int taskType) {
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
  exe::TaskManager& taskMgr = exe::TaskManager::GetInstance();
  size_t ii;
  m::TaskIdMap::iterator mit;
  const m::TaskList * tasklist = model.getTaskList();

  // for each task
  for (ii = 0; ii < tasklist->getTaskCount(); ++ii) {
    m::ModelTask * task = tasklist->getTask(ii);
    m::ModelTask::TaskType type = task->getTaskType();
    // if agent task
    if (type == m::ModelTask::xfunction || type == m::ModelTask::xcondition)
      registerAgentTaskWithTaskManager(task, model);
    // if data tast
    if (type == m::ModelTask::io_pop_write)
      taskMgr.CreateIOTask(task->getTaskName(), task->getParentName(),
          task->getName(), exe::IOTask::OP_OUTPUT);
    // if init io task
    if (type == m::ModelTask::start_model)
      taskMgr.CreateIOTask(task->getTaskName(), "", "", exe::IOTask::OP_INIT);
    // if final io task
    if (type == m::ModelTask::finish_model)
      taskMgr.CreateIOTask(task->getTaskName(), "", "", exe::IOTask::OP_FIN);
    // if message sync task
    if (type == m::ModelTask::xmessage_sync)
      registerMBTaskWithTaskManager(task, 0);
    // if message clear task
    if (type == m::ModelTask::xmessage_clear)
      registerMBTaskWithTaskManager(task, 1);
  }

  // register task dependencies
  m::TaskIdMap dependencies = model.getTaskDependencies();
  for (mit = dependencies.begin(); mit != dependencies.end(); ++mit)
    taskMgr.AddDependency(tasklist->getTask((*mit).second)->getTaskName(),
        tasklist->getTask((*mit).first)->getTaskName());

  // once finalised, tasks and dependencies can no longer be added
  taskMgr.Finalise();
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
