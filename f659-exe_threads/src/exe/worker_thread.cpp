/*!
 * \file src/exe/worker_thread.cpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Basic worker thread
 */
#include "boost/thread/mutex.hpp"
#include "task_queue_interface.hpp"
#include "worker_thread.hpp"
#include "task_manager.hpp"
namespace flame { namespace exe {

WorkerThread::WorkerThread(TaskQueue* taskqueue_ptr) : tq_(taskqueue_ptr) {}

void WorkerThread::Init() {
  thread_ = boost::thread(&WorkerThread::ProcessQueue, this);
}

void WorkerThread::join() {
  thread_.join();
}

void WorkerThread::ProcessQueue() {
  Task::id_type task_id = tq_->GetNextTask();  // calls wait() if queue empty

  while(!Task::IsTermTask(task_id)) {
    RunTask(task_id);
    tq_->TaskDone(task_id);  // register completed task
    task_id = tq_->GetNextTask();  // calls wait() if queue empty
  }
}

void WorkerThread::RunTask(Task::id_type task_id) {
  // TODO(...)
}

}}  // namespace flame::exe
