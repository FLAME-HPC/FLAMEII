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
#include "boost/function.hpp"
#include "mem/memory_iterator.hpp"

namespace flame { namespace exe {

typedef boost::function<int (void*)> TaskFunction;

class Task {

  public:
    typedef size_t id_type;

    enum TaskType {
      AGENT_FUNCTION,
      IO_FUNCTION,
      MB_FUNCTION
    };

    virtual ~Task() {}
    virtual TaskFunction GetFunction() const = 0;

    virtual TaskType get_task_type() const = 0;
    virtual flame::mem::MemoryIteratorPtr GetMemoryIterator() const = 0;
    virtual void AllowAccess(const std::string& var_name,
                             bool writeable = false) = 0;
    virtual void TaskDone() = 0;

    id_type get_task_id() const { return task_id_; }
    void set_task_id(id_type id) { task_id_ = id; }
    std::string get_task_name() { return task_name_; }

  protected:
    id_type task_id_;
    std::string task_name_;
};

}}  // namespace flame::exe
#endif  // EXE__RUNNABLE_TASK_HPP_
