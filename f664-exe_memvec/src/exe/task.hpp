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

namespace flame { namespace exe {

// forward declaration to allow reference without including headers
class VectorWrapperBase;
class TaskManager;

typedef std::map<std::string, flame::mem::VectorWrapperBase*>  VectorMap;

class Task {
  friend class TaskManager;

  public:
    flame::mem::MemoryIteratorPtr get_memory_iterator() const;
    std::string get_task_name() const;

  protected:
    // Tasks should only be created via Task Manager
    Task(std::string task_name, std::string agent_name, AgentFuncPtr func_ptr);

  private:
    std::string task_name_;
    std::string agent_name_;
    AgentFuncPtr func_ptr_;
    flame::mem::MemoryIteratorPtr memory_iterator_;
};

}}  // namespace flame::exe
#endif  // EXE__TASK_HPP_
