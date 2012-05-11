/*!
 * \file src/exe/task.cpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief DESCRIPTION
 */
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
}

void Task::AllowAccess(std::string var_name, bool writeable) {
  mem::MemoryManager& mgr = mem::MemoryManager::GetInstance();
  mem::VectorWrapperBase* vec_ptr = mgr.GetVectorWrapper(agent_name_, var_name);

  std::pair<VectorMap::iterator, bool> ret;
  ret = vec_map_.insert(VectorMapValue(var_name, vec_ptr));
  if (!ret.second) {  // if replacement instead of insertion
    throw flame::exceptions::logic_error("variable already registered");
  }

  if (writeable) {
    rw_set_.insert(var_name);
  }
}

std::string Task::get_task_name() {
  return task_name_;
}

}}  // namespace flame::exe
