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

TaskIdSet TaskList::getAgentTasks() const {
  size_t ii;
  TaskIdSet tasks;

  // For each vertex
  for (ii = 0; ii <= vertex2task_->size(); ++ii)
    // If agent task
    if (vertex2task_->at(ii)->getTaskType() == Task::xfunction ||
        vertex2task_->at(ii)->getTaskType() == Task::xcondition)
      tasks.insert(ii);

  return tasks;
}

TaskIdSet TaskList::getAgentIOTasks() const {
  size_t ii;
  TaskIdSet tasks;

  // For each vertex
  for (ii = 0; ii <= vertex2task_->size(); ++ii)
    // If data task
    if (vertex2task_->at(ii)->getTaskType() == Task::io_pop_write)
      tasks.insert(ii);

  return tasks;
}

TaskId TaskList::getInitIOTask() const {
  size_t ii;

  // For each vertex
  for (ii = 0; ii <= vertex2task_->size(); ++ii)
    // If start data task
    if (vertex2task_->at(ii)->getTaskType() == Task::start_model)
      return ii;

  throw flame::exceptions::flame_model_exception(
        "Init IO Task does not exist");
}

TaskId TaskList::getFinIOTask() const {
  size_t ii;

  // For each vertex
  for (ii = 0; ii <= vertex2task_->size(); ++ii)
    // If finish data task
    if (vertex2task_->at(ii)->getTaskType() == Task::finish_model)
      return ii;

  throw flame::exceptions::flame_model_exception(
        "Init IO Task does not exist");
}

TaskIdSet TaskList::getMessageBoardSyncTasks() const {
  size_t ii;
  TaskIdSet tasks;

  // For each vertex
  for (ii = 0; ii <= vertex2task_->size(); ++ii)
    // if message sync task
    if (vertex2task_->at(ii)->getTaskType() == Task::xmessage_sync)
      tasks.insert(ii);

  return tasks;
}

TaskIdSet TaskList::getMessageBoardClearTasks() const {
  size_t ii;
  TaskIdSet tasks;

  // For each vertex
  for (ii = 0; ii <= vertex2task_->size(); ++ii)
    // if message clear task
    if (vertex2task_->at(ii)->getTaskType() == Task::xmessage_clear)
      tasks.insert(ii);

  return tasks;
}

std::string TaskList::getTaskName(TaskId id) const {
  return getTask(id)->getTaskName();
}

std::string TaskList::getTaskAgentName(TaskId id) const {
  return getTask(id)->getParentName();
}

std::string TaskList::getTaskFunctionName(TaskId id) const {
  return getTask(id)->getName();
}

StringSet TaskList::getTaskReadOnlyVariables(TaskId id) const {
  return getTask(id)->getReadOnlyVariablesConst();
}

StringSet TaskList::getTaskWriteVariables(TaskId id) const {
  return getTask(id)->getWriteVariablesConst();
}

StringSet TaskList::getTaskOutputMessages(TaskId id) const {
  return getTask(id)->getOutputMessagesConst();
}

StringSet TaskList::getTaskInputMessages(TaskId id) const {
  return getTask(id)->getInputMessagesConst();
}

}}  // namespace flame::model
