/*!
 * \file src/exe/fifo_task_queue.cpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Basic Task Queue that uses FIFO scheduling
 */
#include "boost/thread/mutex.hpp"
#include "exceptions/all.hpp"
#include "task_manager.hpp"
#include "task_interface.hpp"
#include "fifo_task_queue.hpp"

namespace flame { namespace exe {

FIFOTaskQueue::FIFOTaskQueue(Task::TaskType task_type, size_t num_threads)
    : type_(task_type) {}

Task::TaskType FIFOTaskQueue::GetType() {
  return type_;
}

void FIFOTaskQueue::PushTask(Task::id_type task_id) {
#ifdef DEBUG
  TaskManager& tm = TaskManager::GetInstance();
  Task& task = tm.GetTask(task_id);  // may throw invalid_argument
  if (task.get_task_type() != type_) {
    throw flame::exceptions::invalid_type("incompatible task type");
  }
#endif
  boost::unique_lock<boost::mutex> lock(mutex_);
  queue_.push(task_id);
}

}}  // namespace flame::exe
