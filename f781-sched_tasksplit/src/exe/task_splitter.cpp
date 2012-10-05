/*!
 * \file FILENAME
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief DESCRIPTION
 */
#include "task_splitter.hpp"

namespace flame { namespace exe {
TaskSplitter::TaskSplitter(Task::id_type id, const TaskVector& tasks)
    : id_(id), running_(0), tasks_(tasks) {
  pending_ = tasks_.size();
  next_ = pending_;
}

bool TaskSplitter::IsComplete(void) const {
  return (running_ == 0 && pending_ == 0);
}

//! Returns true if all tasks have been assigned (may be still running tasks)
bool TaskSplitter::NonePending(void) const {
  // cannot rely on tasks_.size() since tasks_ are not popped on assignment
  // to keep the object alive (held by shared pointer)
  return pending_ == 0;
}

//! Decrements running_ and returns true if IsComplete()
bool TaskSplitter::OneTaskDone(void) {
#ifdef DEBUG
  if (running_ < 1) {
    throw flame::exceptions::flame_exception("None running");
  }
#endif
  --running_;
  return IsComplete();
}

//! Decrements pending_ and returns true if NonePending()
bool TaskSplitter::OneTaskAssigned(void) {
#ifdef DEBUG
  if (running_ < 1) {
    throw flame::exceptions::flame_exception("None running");
  }
#endif
  --pending_;
  return NonePending();
}

//! Returns reference to the next task and decrements next_
Task& TaskSplitter::GetTask() {
#ifdef DEBUG
  if (next_ < 1) {
    throw flame::exceptions::flame_exception("None running");
  }
#endif
  Task* t_ptr = tasks_[next_ - 1].get();
  --next_;
  return *t_ptr;
}
}}  // namespace flame::exe
