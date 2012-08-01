/*!
 * \file FILENAME
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief DESCRIPTION
 */
// Note: for some versions of Boost, valgrind --leak-check=full may claim
// than "8 bytes in block" still reachable. This is not so and is safe to
// ignore.
// See: http://stackoverflow.com/questions/6321602/boost-thread-leakage-c
#include <stdexcept>
#include "boost/thread/mutex.hpp"
#include "boost/thread/condition_variable.hpp"
#include "boost/foreach.hpp"
#include "exceptions/all.hpp"
#include "task_manager.hpp"
#include "scheduler.hpp"

namespace flame { namespace exe {

void Scheduler::AssignType(QueueId qid, Task::TaskType type) {
  if (!IsValidQueueId(qid)) {
    throw flame::exceptions::invalid_argument("invalid queue id");
  }

  RouteMap::iterator lb = route_.lower_bound(type);
  if (lb != route_.end() && type == lb->first) {
    throw flame::exceptions::invalid_argument("type already assigned");
  } else {
    route_.insert(lb, RouteMap::value_type(type, qid));
  }
}

bool Scheduler::IsValidQueueId(QueueId id) {
  return (id < queues_.size());
}


void Scheduler::TaskDoneCallback(Task::id_type task_id) {
  boost::unique_lock<boost::mutex> lock(doneq_mutex_);
  doneq_.push_back(task_id);
  doneq_cond_.notify_one();
}

void Scheduler::RunIteration() {
  TaskManager &tm = TaskManager::GetInstance();
  tm.Finalise();

  // sanity check. avoid deadlock if no ready tasks
  if (!tm.IterTaskAvailable()) {
    throw flame::exceptions::flame_exe_exception("No runnable tasks");
  }

  while (!tm.IterCompleted()) {  // repeat till all tasks completed
    while (tm.IterTaskAvailable()) {  // schedule ready tasks
      EnqueueTask(tm.IterTaskPop());
    }

    { // deal with tasks that have been completed by workers
      boost::unique_lock<boost::mutex> lock(doneq_mutex_);
      if (doneq_.empty()) {
        doneq_cond_.wait(lock);
      }
      while (!doneq_.empty()) {
        tm.IterTaskDone(doneq_.back());
        doneq_.pop_back();
      }
    }  // lock freed on exiting block scope
  }

  tm.IterReset();  // prepare for next iteration
}

void Scheduler::EnqueueTask(Task::id_type task_id) {
  TaskManager &tm = TaskManager::GetInstance();
  Task& task = tm.GetTask(task_id);

  try {
    QueueId qid = route_.at(task.get_task_type());  // identify queue
    queues_.at(qid).Enqueue(task.get_task_id());
  } catch(const std::out_of_range& E) {
    throw flame::exceptions::invalid_type("unassigned task type");
  }
}

}}  // namespace flame::exe
