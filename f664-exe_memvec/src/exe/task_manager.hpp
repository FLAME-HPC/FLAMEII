/*!
 * \file src/exe/task_manager.cpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief DESCRIPTION
 */
#ifndef EXE__TASK_MANAGER_HPP_
#define EXE__TASK_MANAGER_HPP_
#include <string>
#include "boost/ptr_container/ptr_map.hpp"
#include "include/flame.h"
//#include "mem/memory_manager.hpp"
#include "task.hpp"

namespace flame { namespace exe {

typedef boost::ptr_map<std::string, Task> TaskMap;

class TaskManager {
  public:
    Task& CreateTask(std::string task_name,
                     std::string agent_name,
                     AgentFuncPtr func_ptr);

    Task& GetTask(std::string task_name);
  private:
    TaskMap task_map_;
};

}}  // namespace flame::exe
#endif // EXE__TASK_MANAGER_HPP_
