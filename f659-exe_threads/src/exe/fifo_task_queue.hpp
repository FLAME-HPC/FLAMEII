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

    FIFOTaskQueue(size_t slots);
    ~FIFOTaskQueue();

    void Enqueue(Task::id_type task_id);
    void TaskDone(Task::id_type task_id);
    Task::id_type GetNextTask();

    bool empty() const;

  protected:
    size_t slots_;
    WorkerVector workers_;

  private:
    std::queue<Task::id_type> queue_;
};

}}  // namespace flame::exe
#endif  // EXE__FIFO_TASK_QUEUE_HPP_
