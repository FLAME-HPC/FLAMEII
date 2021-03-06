/*!
 * \file flame2/exe/splitting_fifo_task_queue.cpp
 * \author Shawn Chin
 * \date Oct 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Implementation of SplittingFIFOTaskQueue
 */
#include <boost/thread/mutex.hpp>
#include <boost/foreach.hpp>
#include "flame2/config.hpp"
#include "task_manager.hpp"
#include "splitting_fifo_task_queue.hpp"

namespace flame { namespace exe {

/*!
 * \brief Constructor
 *
 * Sets the max_split_ to the number of slots and
 * min_vector_size to DEFAULT_MIN_VECTOR_SIZE.
 *
 * Throws flame::exceptions::invalid_argument if an invalid value for slot is
 * given.
 */
SplittingFIFOTaskQueue::SplittingFIFOTaskQueue(size_t slots)
    : slots_(slots),
      max_splits_(slots),
      min_vector_size_(DEFAULT_MIN_VECTOR_SIZE) {
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
 * Enqueue a termination tasks for each worker thread and wait for them to
 * complete.
 */
SplittingFIFOTaskQueue::~SplittingFIFOTaskQueue() {
  for (size_t i = 0; i < slots_; ++i) {
    Enqueue(Task::GetTermTaskId());
  }
  BOOST_FOREACH(WorkerThread &thread, workers_) {
    thread.join();  // block till thread actually ends
  }
}

//! \brief Addes to set of task type than can be split.
void SplittingFIFOTaskQueue::SetSplittable(Task::TaskType task_type) {
  splittable_.insert(task_type);
}

//! \brief Sets the value of max_splits_
void SplittingFIFOTaskQueue::SetMaxTasksPerSplit(size_t max_splits) {
  if (max_splits < 1) {
    throw flame::exceptions::invalid_argument("max_splits must be > 0");
  }
  max_splits_ = max_splits;
}

//! \brief Returns the value of max_splits_
size_t SplittingFIFOTaskQueue::GetMaxTasksPerSplit(void) const {
  return max_splits_;
}

//! \brief Sets the value of min_vector_size_
void SplittingFIFOTaskQueue::SetMinVectorSize(size_t min_vector_size) {
  if (min_vector_size < 1) {
    throw flame::exceptions::invalid_argument("min_vector_size must be > 0");
  }
  min_vector_size_ = min_vector_size;
}

//! \brief Returns the value of min_vector_size_
size_t SplittingFIFOTaskQueue::GetMinVectorSize(void) const {
  return min_vector_size_;
}

//! \brief Returns true if the queue is empty
bool SplittingFIFOTaskQueue::empty() const {
  return queue_.empty();
}

/*!
 * \brief Adds a task to the queue
 *
 * This method is meant to be called by the Scheduler
 *
 * Appends the tasks to the FIFO queue. If the task is a splittable task,
 * attempt the split the task. If the split is successful, add the resulting
 * TaskSplitter instance to the split_map_ with the task_id as key.
 */
void SplittingFIFOTaskQueue::Enqueue(Task::id_type task_id) {
  boost::lock_guard<boost::mutex> lock(mutex_);
  queue_.push(task_id);
  ready_.notify_one();  // wake up one worker thread

  // TERM task id is virtual and has no corresponding task obj.
  if (Task::IsTermTask(task_id)) return;   // Nothing left to do.

  // Check if task is splittable
  Task& t = TaskManager::GetInstance().GetTask(task_id);
  if (splittable_.find(t.get_task_type()) == splittable_.end()) return;  // no

  // Attempt to split atoms^H^H^H^H^H task
  TaskSplitterHandle ts = t.SplitTask(max_splits_, min_vector_size_);
  if (ts) {  // successfully split. Add to split_map_
    SplitMap::iterator lb = split_map_.lower_bound(task_id);
    if (lb != split_map_.end() &&
        !(split_map_.key_comp()(task_id, lb->first))) {  // key exists
      throw flame::exceptions::logic_error("task id conflict");
    } else {
      // register subtasks. Wake ALL workers and return
      split_map_.insert(SplitMap::value_type(task_id, ts));
      // wake up more workers
      for (size_t i = 0; i < ts->GetNumTasks() -1; ++i) {
        ready_.notify_one();
      }
    }
  }
}

/*!
 * \brief Indicate that a task has been completed
 *
 * This method is meant to be called by a Worker Thread.
 *
 * If the task does not have an entry in split_map_, run the callback function
 * and return.
 *
 * If the task has an entry in split_map_, retrieve the task splitter instance
 * and indicate that a task segment has been completed. If there are still
 * pending or running subtasks, do nothing else.
 *
 * If all subtasks have been completed, remove the task from the split_map_ and
 * run the scheduler callback function to signal that the whole task has been
 * completed.
 */
void SplittingFIFOTaskQueue::TaskDone(Task::id_type task_id) {
  // mutex required since we're accessing split_map_
  boost::lock_guard<boost::mutex> lock(mutex_);

  // determine if this is a split task
  SplitMap::iterator it = split_map_.find(task_id);
  if (it != split_map_.end()) {  // it is
    bool all_completed = it->second->OneTaskDone();
    if (!all_completed) {
      return;  // still more to go.  callback should not go upsteam
    }
  }

  // all subtasks completed. Remove from map and trigger callback
  split_map_.erase(task_id);
  callback_(task_id);
}

/*!
 * \brief Returns the next available task.
 *
 * If there are none available, the calling thread will be blocked
 *
 * This method is meant to be called by a Worker Thread.
 *
 * If the task is split (has an entry in split_map_), only pop the task from
 * the FIFO queue when all subtasks have been assigned.
 */
Task::id_type SplittingFIFOTaskQueue::GetNextTask(void) {
  boost::unique_lock<boost::mutex> lock(mutex_);
  while (empty()) {
    ready_.wait(lock);
  }

  // Peek at next candidate
  Task::id_type task_id = queue_.front();

  // determine if this is a split task
  SplitMap::iterator it = split_map_.find(task_id);
  if (it != split_map_.end()) {  // it is
    bool none_remaining = it->second->OneTaskAssigned();
    if (none_remaining) {
      queue_.pop();  // all tasks assigned. dequeue.
    }
  } else {  // not a split task. dequeue as usual
    queue_.pop();
  }

  return task_id;
}

/*!
 * \brief Returns a task reference given a task id
 *
 * This is used by the worker thread to get at the actual task.
 *
 * If the task is a split task (has an entry in split_map_) we return a
 * reference to a subtask. Otherwise, request for the task from the
 * Task Manager.
 */
Task& SplittingFIFOTaskQueue::GetTaskById(Task::id_type task_id) {
  boost::unique_lock<boost::mutex> lock(mutex_);
  SplitMap::iterator it = split_map_.find(task_id);
  if (it != split_map_.end()) {  // it is
    return it->second->GetTask();
  } else {  // normal task
    return TaskManager::GetInstance().GetTask(task_id);
  }
}
}}  // namespace flame::exe
