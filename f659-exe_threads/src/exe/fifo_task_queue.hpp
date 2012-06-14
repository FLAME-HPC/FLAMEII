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
#include "task_queue_interface.hpp"

namespace flame { namespace exe {

class FIFOTaskQueue
{
  public:
    FIFOTaskQueue(Task::TaskType task_type, size_t num_threads);
    Task::TaskType GetType();
    void PushTask(Task::id_type task_id);
  protected:
    size_t num_threads_;
    Task::TaskType type_;
    boost::mutex mutex_;
  private:
    std::queue<Task::id_type> queue_;
};

}}  // namespace flame::exe
#endif  // EXE__FIFO_TASK_QUEUE_HPP_
