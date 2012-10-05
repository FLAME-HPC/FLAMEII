/*!
 * \file src/exe/task.cpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Agent function task
 */
#include <utility>
#include <string>
#include "agent_task.hpp"
#include "exceptions/all.hpp"
#include "mem/memory_manager.hpp"
#include "task_splitter.hpp"

namespace flame { namespace exe {

namespace mem = flame::mem;
namespace mb = flame::mb;
typedef std::pair<std::string, mem::VectorWrapperBase*>  VectorMapValue;

AgentTask::AgentTask(std::string task_name, std::string agent_name,
                     TaskFunction func) : is_split_(false) {
  _init(task_name, agent_name, func);
}

//! Constructor used internally to produce split task
AgentTask::AgentTask(std::string task_name,
                     std::string agent_name,
                     TaskFunction func,
                     size_t offset, size_t count) : is_split_(true) {
  _init(task_name, agent_name, func);
  offset_ = offset;
  count_ = count;
}

void AgentTask::_init(std::string task_name, std::string agent_name,
                      TaskFunction func) {
  offset_ = 0;
  count_ = 0;
  func_ = func;
  agent_name_ = agent_name;
  task_name_ = task_name;
  mem::MemoryManager& mm = mem::MemoryManager::GetInstance();
  if (!mm.IsRegisteredAgent(agent_name)) {
    throw flame::exceptions::invalid_agent("Invalid agent");
  }
  if (!func) {
    throw flame::exceptions::invalid_argument("NULL function provided");
  }

  shadow_ptr_ = mm.GetAgentShadow(agent_name);
}

flame::mem::MemoryIteratorPtr AgentTask::GetMemoryIterator() const {
  if (is_split_) {
    return shadow_ptr_->GetMemoryIterator(offset_, count_);
  } else {
    return shadow_ptr_->GetMemoryIterator();
  }
}

void AgentTask::AllowAccess(const std::string& var_name, bool writeable) {
  shadow_ptr_->AllowAccess(var_name, writeable);
}

void AgentTask::Run() {
  mem::MemoryIteratorPtr m_ptr = GetMemoryIterator();
  MessageBoardClient mb_client = GetMessageBoardClient();

  m_ptr->Rewind();
  while (!m_ptr->AtEnd()) {  // run function for each agent
    func_(static_cast<void*>(m_ptr.get()), static_cast<void*>(mb_client.get()));
    // TODO(lsc): check rc == 0 to handle agent death
    m_ptr->Step();
  }
}

//! Returns a task splitter which allows task to be exected in segments
TaskSplitterHandle AgentTask::SplitTask(size_t max_tasks,
                                          size_t min_task_size) {
  if (max_tasks < 2) {  // no splitting required
    return TaskSplitterHandle();  // return null handle
  }

  // calculate how many splits and split size
  size_t offset = (is_split_) ? offset_ : 0;
  size_t population = (is_split_) ? count_ : shadow_ptr_->get_size();
  if (population < (min_task_size * 2)) {  // too small to split
    return TaskSplitterHandle();  // return null handle
  }

  size_t num_splits, size_per_task, remainder;
  if (population >= (min_task_size * max_tasks)) {
    num_splits = max_tasks;
  } else {
    num_splits = population / min_task_size;
  }
  size_per_task = population / num_splits;
  remainder = population % num_splits;

  // Create TaskSplitter::TaskVector and populate using internal constructor
  Task::Handle t;
  TaskSplitter::TaskVector vec;
  vec.reserve(num_splits);
  size_t s;
  for (size_t i = 0; i < num_splits; ++i) {
    s = ((i < remainder) ? 1 : 0) + size_per_task;
    t = Task::Handle(new AgentTask(task_name_, agent_name_, func_, offset, s));
    t->set_task_id(task_id_);
    vec.push_back(t);
    offset += s;
  }

  // Construct and return TaskSplitter
  return TaskSplitterHandle(new TaskSplitter(task_id_, vec));
}

}}  // namespace flame::exe
