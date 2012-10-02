/*!
 * \file src/exe/fifo_task_queue.hpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Basic Task Queue that uses FIFO scheduling
 */
#ifndef EXE__FIFO_TASK_QUEUE_HPP_
#define EXE__FIFO_TASK_QUEUE_HPP_
#include <queue>
#include "worker_thread.hpp"
#include "task_queue_interface.hpp"

namespace flame { namespace exe {

class FIFOTaskQueue : public TaskQueue {
  public:
    typedef boost::ptr_vector<WorkerThread> WorkerVector;

    explicit FIFOTaskQueue(size_t slots);
    ~FIFOTaskQueue();

    //! \brief Adds a task to the queue
    //!
    //! This method is meant to be called by the Scheduler
    void Enqueue(Task::id_type task_id);

    //! \brief Indicate that a task has been completed
    //!
    //! This method is meant to be called by a Worker Thread
    void TaskDone(Task::id_type task_id);

    //! \brief Returns the next available task.
    //! If there are none available, the calling thread will be blocked
    //!
    //! This method is meant to be called by a Worker Thread
    Task::id_type GetNextTask();

    //! \brief Returns true if the queue is empty
    bool empty() const;

  protected:
    size_t slots_;  //! Number of processing slots (worker threads)
    WorkerVector workers_;  //! Collection of worker threads

  private:
    std::queue<Task::id_type> queue_;  //! FIFO task queue
};

}}  // namespace flame::exe
#endif  // EXE__FIFO_TASK_QUEUE_HPP_
