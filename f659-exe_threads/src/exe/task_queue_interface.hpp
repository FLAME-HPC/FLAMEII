/*!
 * \file src/exe/task_queue_interface.hpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Interface class for Task Queues
 */
#ifndef EXE__TASK_QUEUE_INTERFACE_HPP_
#define EXE__TASK_QUEUE_INTERFACE_HPP_
#include "boost/thread/mutex.hpp"
#include "boost/thread/condition_variable.hpp"
#include "task_interface.hpp"

namespace flame { namespace exe {

typedef boost::function<void (Task::id_type)> TaskQueueCallback;

class TaskQueue {
  friend class WorkerThread;
  public:
    virtual ~TaskQueue() {}
    virtual void Enqueue(Task::id_type task_id) = 0;
    virtual void TaskDone(Task::id_type task_id) = 0;
    virtual Task::id_type GetNextTask() = 0;
    virtual bool empty() const = 0;
    void SetCallback(TaskQueueCallback func) {
      callback_ = func;
    }
  protected:
    boost::mutex mutex_;
    boost::condition_variable ready_;
    TaskQueueCallback callback_;
};

}}  // namespace flame::exe
#endif  // EXE__TASK_QUEUE_INTERFACE_HPP_
