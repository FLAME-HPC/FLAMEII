/*!
 * \file flame2/exe/task.cpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Agent function task
 */
#include <utility>
#include <string>
#include "flame2/config.hpp"
#include "flame2/exceptions/all.hpp"
#include "flame2/exceptions/api.hpp"
#include "flame2/api/agent_api.hpp"
#include "flame2/mem/memory_manager.hpp"
#include "agent_task.hpp"
#include "task_splitter.hpp"

namespace flame { namespace exe {

namespace mem = flame::mem;
namespace mb = flame::mb;
namespace api = flame::api;

typedef std::pair<std::string, mem::VectorWrapperBase*>  VectorMapValue;

AgentTask::AgentTask(std::string task_name, std::string agent_name,
                     TaskFunction func)
    : agent_name_(agent_name), func_(func),
      is_split_(false), offset_(0), count_(0) {
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

AgentTask::AgentTask(const AgentTask& parent, size_t offset, size_t count)
    : is_split_(true), offset_(offset), count_(count) {
  func_ = parent.func_;
  task_id_ = parent.task_id_;
  mb_proxy_ = parent.mb_proxy_;
  task_name_ = parent.task_name_;
  agent_name_ = parent.agent_name_;
  shadow_ptr_ = parent.shadow_ptr_;
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
  mem::MemoryIteratorPtr m = GetMemoryIterator();
  api::AgentAPI agent(m, GetMessageBoardClient());

  while (!m->AtEnd()) {  // run function for each agent
    try {
      func_(agent);
    } catch(const flame::exceptions::flame_api_exception& E) {
      // throw new exception and tag on agent/task name
      throw flame::exceptions::flame_task_exception(agent_name_,
                                                get_transition_function_name(),
                                                E.what());
    }

    // TODO(lsc): check rc == 0 to handle agent death
    m->Step();
  }
}

TaskSplitterHandle AgentTask::SplitTask(size_t max_tasks,
                                          size_t min_task_size) {
  if (max_tasks < 2) {  // no splitting required
    return TaskSplitterHandle();  // return null handle
  }

  // calculate how many splits and split size
  size_t offset = (is_split_) ? offset_ : 0;
  size_t population = (is_split_) ? count_ : shadow_ptr_->get_size();
  if (population < (min_task_size * 2))  // too small to split
    return TaskSplitterHandle();  // return null handle

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
    t = Task::Handle(new AgentTask(*this, offset, s));
    vec.push_back(t);
    offset += s;
  }

  // Construct and return TaskSplitter
  return TaskSplitterHandle(new TaskSplitter(task_id_, vec));
}

}}  // namespace flame::exe
