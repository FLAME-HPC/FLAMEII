/*!
 * \file src/exe/task_manager.cpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief DESCRIPTION
 */
#include <utility>
#include <string>
#include "task_manager.hpp"
#include "exceptions/all.hpp"

namespace flame { namespace exe {

Task& TaskManager::CreateTask(std::string task_name,
                              std::string agent_name,
                              AgentFuncPtr func_ptr) {
  std::pair<TaskMap::iterator, bool> ret;
  Task* t = new Task(task_name, agent_name, func_ptr);
  ret = task_map_.insert(task_name, t);
  if (!ret.second) {  // if replacement instead of insertion
    throw flame::exceptions::logic_error("task with that name already exists");
  }
  return *t;
}

Task& TaskManager::GetTask(std::string task_name) {
  try {
    return task_map_.at(task_name);
  }
  catch(const boost::bad_ptr_container_operation& E) {
    throw flame::exceptions::invalid_argument("Unknown task");
  }
}

#ifdef TESTBUILD
void TaskManager::Reset() {
  task_map_.clear();
}
#endif

}}  // namespace flame::exe
