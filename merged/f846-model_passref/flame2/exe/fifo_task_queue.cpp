/*!
 * \file flame2/exe/fifo_task_queue.cpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Basic Task Queue that uses FIFO scheduling
 */
#include <boost/thread/mutex.hpp>
#include <boost/foreach.hpp>
#include "flame2/config.hpp"
#include "flame2/exceptions/all.hpp"
#include "task_manager.hpp"
#include "task_interface.hpp"
#include "fifo_task_queue.hpp"

namespace flame { namespace exe {

/*!
 * \brief Constructor
 * \param[in] slots Number of slots
 *
 * Populates the vector of worker threads and initialses the threads.
 *
 * Throws flame::exceptions::invalid_argument if an invalid value is given
 * for slots.
 */
FIFOTaskQueue::FIFOTaskQueue(size_t slots) : slots_(slots) {
  if (slots < 1) {
    throw flame::exceptions::invalid_argument("slots must be > 0");
  }

  // initialise workers
  workers_.reserve(slots);
  for (size_t i = 0; i < slots; ++i) {
    WorkerThread *t = new WorkerThread(this);
    t->Init();
    workers_.push_back(t);
  }
}

/*!
 * \brief Destructor
 *
 * Enqueue the termination task (signal worker threads to wrap up) and waits
 * for all worker threads to complete before destroying this object.
 */
FIFOTaskQueue::~FIFOTaskQueue() {
  for (size_t i = 0; i < slots_; ++i) {
    Enqueue(Task::GetTermTaskId());
  }
  BOOST_FOREACH(WorkerThread &thread, workers_) {
    thread.join();  // block till thread actually ends
  }
}

//! \brief Returns true if the queue is empty
bool FIFOTaskQueue::empty() const {
  return queue_.empty();
}

/*!
 * \brief Adds a task to the queue
 *
 * This method is meant to be called by the Scheduler
 */
void FIFOTaskQueue::Enqueue(Task::id_type task_id) {
  boost::lock_guard<boost::mutex> lock(mutex_);
  queue_.push(task_id);
  ready_.notify_one();
}

/*!
 * \brief Process a completed task
 *
 * This method is meant to be called by a worker thread.
 *
 * This triggers the callback function of the parent scheduler.
 */
void FIFOTaskQueue::TaskDone(Task::id_type task_id) {
  callback_(task_id);
}

/*!
 * \brief Returns the next available task.
 *
 * If there are none available, the calling thread will be blocked
 *
 * This method is meant to be called by a Worker Thread
 */
Task::id_type FIFOTaskQueue::GetNextTask() {
  boost::unique_lock<boost::mutex> lock(mutex_);
  while (queue_.empty()) {
    ready_.wait(lock);
  }

  Task::id_type task_id = queue_.front();
  queue_.pop();
  return task_id;
}

}}  // namespace flame::exe
