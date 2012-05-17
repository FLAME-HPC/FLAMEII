/*!
 * \file src/exe/task.cpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief DESCRIPTION
 */
#include <utility>
#include <string>
#include "task.hpp"
#include "exceptions/all.hpp"
#include "mem/memory_manager.hpp"

namespace flame { namespace exe {

namespace mem = flame::mem;
typedef std::pair<std::string, mem::VectorWrapperBase*>  VectorMapValue;

Task::Task(std::string task_name, std::string agent_name, AgentFuncPtr func_ptr)
        : task_name_(task_name), agent_name_(agent_name), func_ptr_(func_ptr) {
  mem::MemoryManager& mm = mem::MemoryManager::GetInstance();
  if (!mm.IsRegisteredAgent(agent_name)) {
    throw flame::exceptions::invalid_agent("Invalid agent");
  }
  if (func_ptr == NULL) {
    throw flame::exceptions::invalid_argument("Function pointer is NULL");
  }

  shadow_ptr_ = mm.GetAgentShadow(agent_name);
}

flame::mem::MemoryIteratorPtr Task::GetMemoryIterator() const {
  return shadow_ptr_->GetMemoryIterator();
}

void Task::AllowAccess(const std::string& var_name, bool writeable) {
  shadow_ptr_->AllowAccess(var_name, writeable);
}

std::string Task::get_task_name() const {
  return task_name_;
}

AgentFuncPtr Task::get_func_ptr() const {
  return func_ptr_;
}

}}  // namespace flame::exe
