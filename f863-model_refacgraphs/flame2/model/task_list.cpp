/*!
 * \file flame2/model/task_list.cpp
 * \author Simon Coakley
 * \date 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief TaskList: used to hold tasks
 */
#include "flame2/config.hpp"
#include "flame2/exceptions/model.hpp"
#include "task_list.hpp"

namespace flame { namespace model {

TaskList::TaskList() {
  vertex2task_ = new std::vector<TaskPtr>;
}

TaskList::~TaskList() {
  // Free vertex task mapping
  delete vertex2task_;
}

void TaskList::addTask(TaskPtr ptr) {
  vertex2task_->push_back(ptr);
}

void TaskList::removeTask(size_t index) {
  vertex2task_->erase(vertex2task_->begin() + index);
}

size_t TaskList::getIndex(Task * t) const {
  size_t ii;
  // Find index of task
  for (ii = 0; ii < vertex2task_->size(); ++ii)
    if (vertex2task_->at(ii).get() == t) return ii;
  return 0;
}

Task * TaskList::getTask(size_t index) const {
  if (index >= static_cast<size_t>(vertex2task_->size()))
    throw flame::exceptions::flame_model_exception(
      "Task id does not exist");

  // Return task at index v
  return vertex2task_->at(index).get();
}

}}  // namespace flame::model
