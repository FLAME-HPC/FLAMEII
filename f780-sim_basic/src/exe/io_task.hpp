/*!
 * \file src/exe/io_task.hpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Task that performs IO operations
 */
#ifndef EXE__IO_TASK_HPP_
#define EXE__IO_TASK_HPP_
#include <string>
#include "task_interface.hpp"

namespace flame { namespace exe {

class IOTask : public Task {
  friend class TaskManager;
  public:
    //! Allowed operation types
    enum Operation {
      OP_OUTPUT
    };

    //! Returns the task type
    TaskType get_task_type(void) const { return Task::IO_FUNCTION; }

    //! Returns a memory iterator for this task
    flame::mem::MemoryIteratorPtr GetMemoryIterator(void) const {
      throw flame::exceptions::logic_error("method not applicable");
    }

    //! Enable access to a specific agent var (not applicable)
    void AllowAccess(const std::string& var_name, bool writeable = false) {
      throw flame::exceptions::logic_error("method not applicable");
    }

    //! Returns a task splitter (not supported by MB task)
    TaskSplitterHandle SplitTask(size_t max_tasks, size_t min_task_size) {
      throw flame::exceptions::not_implemented("method not applicable");
    }

    //! Runs the task
    void Run(void);

  protected:
    // Tasks should only be created via Task Manager
    IOTask(std::string task_name, std::string agent_name,
           std::string var_name, Operation op);

  private:
    std::string agent_name_;
    std::string var_name_;
    Operation op_;
};

}}  // namespace flame::exe
#endif  // EXE__IO_TASK_HPP_
