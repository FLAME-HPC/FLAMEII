/*!
 * \file flame2/exe/task_queue_interface.hpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Interface class for Task Queues
 */
#ifndef EXE__TASK_QUEUE_INTERFACE_HPP_
#define EXE__TASK_QUEUE_INTERFACE_HPP_
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include "task_manager.hpp"
#include "task_interface.hpp"

namespace flame { namespace exe {

//! Datatype for Task Queue callback function
typedef boost::function<void (Task::id_type)> TaskQueueCallback;

//! Abstract class for Task Queues
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

    //! Specify maximum splits per task
    virtual void SetMaxTasksPerSplit(size_t max_tasks_per_split) = 0;

    //! Returns maximum splits per task
    virtual size_t GetMaxTasksPerSplit(void) const = 0;

    //! Specify minimum vector size after split
    virtual void SetMinVectorSize(size_t min_vector_size) = 0;

    //! Returns minimum vector size after split
    virtual size_t GetMinVectorSize(void) const = 0;

    /*! 
     * \brief Returns a task reference given a task id
     * \param task_id task id
     * \return reference to Task instance
     *
     * This usually forwards the call to the TaskManager but it gives the queue
     * an opportunity to intercept the call
     */
    virtual Task& GetTaskById(Task::id_type task_id) {
      return TaskManager::GetInstance().GetTask(task_id);
    }

    //! Sets the callback function to be called when a task is completed
    void SetCallback(TaskQueueCallback func) {
      callback_ = func;
    }

  protected:
    boost::mutex mutex_;  //!< Mutex object 
    boost::condition_variable ready_;  //!< Condition variable
    TaskQueueCallback callback_;  //!< callback function for task queue
};

}}  // namespace flame::exe
#endif  // EXE__TASK_QUEUE_INTERFACE_HPP_

