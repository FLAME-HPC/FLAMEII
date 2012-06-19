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
#include "boost/foreach.hpp"
#include "exceptions/all.hpp"
#include "task_manager.hpp"
#include "task_interface.hpp"
#include "fifo_task_queue.hpp"

namespace flame { namespace exe {

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

FIFOTaskQueue::~FIFOTaskQueue() {
  for (size_t i = 0; i < slots_; ++i) {
    Enqueue(Task::GetTermTaskId());
  }
  BOOST_FOREACH(WorkerThread &thread, workers_) {
    thread.join();  // block till thread actually ends
  }
}

bool FIFOTaskQueue::empty() const {
  return queue_.empty();
}

void FIFOTaskQueue::Enqueue(Task::id_type task_id) {
  boost::lock_guard<boost::mutex> lock(mutex_);
  queue_.push(task_id);
  ready_.notify_one();
}

void FIFOTaskQueue::TaskDone(Task::id_type task_id) {
  callback_(task_id);
}

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
