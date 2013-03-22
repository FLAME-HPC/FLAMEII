/*!
 * \file src/exe/task.cpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief DESCRIPTION
 */
#ifndef EXE__TASK_HPP_
#define EXE__TASK_HPP_
#include <string>
#include <utility>
#include <map>
#include <set>
#include "include/flame.h"
#include "mem/memory_manager.hpp"
#include "mem/memory_iterator.hpp"

// TODO(lsc): Consider boost::function to replace AgentFuncPtr

namespace flame { namespace exe {

class Task {
  friend class TaskManager;

  public:
    //! Enable access to a specific agent var
    void AllowAccess(const std::string& var_name, bool writeable = false);

    //! Returns a new instance of a MemoryIterator
    flame::mem::MemoryIteratorPtr GetMemoryIterator() const;

    //! Returns the name of the task
    std::string get_task_name() const;

    //! Returns the function pointer associated with this task
    AgentFuncPtr get_func_ptr() const;

  protected:
    // Tasks should only be created via Task Manager
    Task(std::string task_name, std::string agent_name, AgentFuncPtr func_ptr);

  private:
    std::string task_name_;  //! Name of task
    std::string agent_name_;  //! Name of associated agent
    AgentFuncPtr func_ptr_;  //! Function pointer associated with task
    flame::mem::AgentShadowPtr shadow_ptr_;  //! Pointer to AgentShadow
};

}}  // namespace flame::exe
#endif  // EXE__TASK_HPP_
