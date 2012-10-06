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
#include "task_manager.hpp"
#include "task_interface.hpp"

namespace flame { namespace exe {

typedef boost::function<void (Task::id_type)> TaskQueueCallback;

class TaskQueue {
  friend class WorkerThread;
  public:
    virtual ~TaskQueue() {}

    //! Adds a task to the queue
    virtual void Enqueue(Task::id_type task_id) = 0;

    //! Indicates that a task has been completed
    virtual void TaskDone(Task::id_type task_id) = 0;

    //! Waits for and returns the next available task
    virtual Task::id_type GetNextTask() = 0;

    //! Returns true if the queue is empty
    virtual bool empty() const = 0;

    //! Assign task type that can be split
    virtual void SetSplittable(Task::TaskType task_type) = 0;

    //! Returns a task reference given a task id
    //! This usually forward the call to the TaskManager but it gives the queue
    //! an opportunity to intercept the call
    virtual Task& GetTaskById(Task::id_type task_id) {
      return TaskManager::GetInstance().GetTask(task_id);
    }

    //! Sets the callback function to be called when a task is completed
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
