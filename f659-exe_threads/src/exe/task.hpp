/*!
 * \file src/exe/task.hpp
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
#include "mem/memory_manager.hpp"
#include "mem/memory_iterator.hpp"
#include "runnable_task.hpp"

namespace flame { namespace exe {

class Task : RunnableTask {
  friend class TaskManager;
  typedef size_t id_type;

  public:
    //! Enable access to a specific agent var
    void AllowAccess(const std::string& var_name, bool writeable = false);

    //! Returns a new instance of a MemoryIterator
    flame::mem::MemoryIteratorPtr GetMemoryIterator() const;

    //! Returns the name of the task
    std::string get_task_name() const;

    //! Returns the id of the task
    id_type get_task_id() const;

    //! Returns the function object associated with this task
    TaskFunction GetFunction() const;

    void TaskDone() {}

  protected:
    // Tasks should only be created via Task Manager
    Task(id_type task_id,
         std::string task_name,
         std::string agent_name,
         TaskFunction func_ptr);

  private:
    id_type task_id_;
    std::string task_name_;  //! Name of task
    std::string agent_name_;  //! Name of associated agent
    TaskFunction func_ptr_;  //! Function pointer associated with task
    flame::mem::AgentShadowPtr shadow_ptr_;  //! Pointer to AgentShadow
};

}}  // namespace flame::exe
#endif  // EXE__TASK_HPP_
