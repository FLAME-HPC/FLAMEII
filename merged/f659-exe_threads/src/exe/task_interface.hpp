/*!
 * \file src/exe/task_interface.hpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Interface class for tasks than can be executed by workers
 */
#ifndef EXE__RUNNABLE_TASK_HPP_
#define EXE__RUNNABLE_TASK_HPP_
#include <limits>
#include <string>
#include "boost/function.hpp"
#include "mem/memory_iterator.hpp"

namespace flame { namespace exe {

typedef boost::function<int (void*)> TaskFunction;

class Task {
  public:
    typedef size_t id_type;

    //! Identifier for different task types
    enum TaskType {
      AGENT_FUNCTION,
      IO_FUNCTION,
      MB_FUNCTION
    };

    virtual ~Task() {}

    //! Runs the task
    virtual void Run() = 0;

    //! Returns the task type
    virtual TaskType get_task_type() const = 0;

    //! Returns a memory iterator for this task
    virtual flame::mem::MemoryIteratorPtr GetMemoryIterator() const = 0;

    //! Defines access control to agent memory variables
    virtual void AllowAccess(const std::string& var_name,
                             bool writeable = false) = 0;

    //! Returns the task id
    id_type get_task_id() const { return task_id_; }

    //! Sets the task id
    void set_task_id(id_type id) { task_id_ = id; }

    //! Returns the task name
    std::string get_task_name() { return task_name_; }

    //! Returns true if the given task id is a termination signal
    inline static bool IsTermTask(id_type task_id) {
      return (task_id == GetTermTaskId());
    }

    //! Returns the task id that represent the termination signal
    inline static id_type GetTermTaskId() {
      return std::numeric_limits<id_type>::max();
    }

  protected:
    id_type task_id_;
    std::string task_name_;
};



}}  // namespace flame::exe
#endif  // EXE__RUNNABLE_TASK_HPP_
