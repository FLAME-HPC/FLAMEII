/*!
 * \file flame2/exe/task_manager.cpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief DESCRIPTION
 */
#include <set>
#include <stack>
#include <string>
#include <stdexcept>
#include <boost/foreach.hpp>
#include "flame2/exceptions/all.hpp"
#include "task_manager.hpp"
#include "agent_task.hpp"

//! Internal routine which asserts that the manager is not finalised
static inline void check_not_finalised(bool finalised) {
  if (finalised) {
    throw flame::exceptions::logic_error(
                 "Finalise() called. Operation not allowed");
  }
}

//! Internal routine which asserts that the manager is finalised
static inline void check_finalised(bool finalised) {
  if (!finalised) {
    throw flame::exceptions::logic_error("Finalise() has not been called");
  }
}

namespace flame { namespace exe {

//! \brief Instantiates, registers and returns a new Agent Task
Task& TaskManager::CreateAgentTask(std::string task_name,
                                   std::string agent_name,
                                   TaskFunction func_ptr) {
  AgentTask* task_ptr = new AgentTask(task_name, agent_name, func_ptr);

  try {  // register new task with manager
    RegisterTask(task_name, task_ptr);
  } catch(const flame::exceptions::logic_error& E) {
    delete task_ptr;  // free memory if registration failed.
    throw E;  // rethrow exception
  }

  return *task_ptr;
}

//! \brief Instantiates, registers and returns a new MessageBoard Task
Task& TaskManager::CreateMessageBoardTask(std::string task_name,
                                         std::string msg_name,
                                         MessageBoardTask::Operation op) {
  MessageBoardTask* task_ptr = new MessageBoardTask(task_name, msg_name, op);
  try {  // register new task with manager
    RegisterTask(task_name, task_ptr);
  } catch(const flame::exceptions::logic_error& E) {
    delete task_ptr;  // free memory if registration failed.
    throw E;  // rethrow exception
  }

  return *task_ptr;
}
//! \brief Registers and returns a new IO Task
Task& TaskManager::CreateIOTask(std::string task_name,
                                std::string agent_name,
                                std::string var_name,
                                IOTask::Operation op) {
  IOTask* task_ptr = new IOTask(task_name, agent_name, var_name, op);
  try {  // register new task with manager
    RegisterTask(task_name, task_ptr);
  } catch(const flame::exceptions::logic_error& E) {
    delete task_ptr;  // free memory if registration failed.
    throw E;  // rethrow exception
  }

  return *task_ptr;
}

/*!
 * \brief Internal method to register a task within the manager
 *
 * The index within tasks_ vector is used as the task id. This allows us to have
 * int-based keys which are easier to pass around and look up.
 *
 * Entries for this task are also made in internal variables used for handing
 * task dependencies.
 *
 * Throws flame::exceptions::logic_error if the task manager is already
 * finalised, or if a task with that name has already been registered.
 *
 * Throws flame::exceptions::out_of_range if the number of tasks exceeds the
 * maximum size (unlikely to happen). The maximum value depends on the limits
 * of an array index.
 */
void TaskManager::RegisterTask(std::string task_name, Task* task_ptr) {
  if (finalised_) {
    throw flame::exceptions::logic_error("Finalise() called. No more updates");
  }
  boost::lock_guard<boost::mutex> lock(mutex_task_);

  // Check for tasks with same name
  TaskNameMap::iterator lb = name_map_.lower_bound(task_name);
  if (lb != name_map_.end() && !(name_map_.key_comp()(task_name, lb->first))) {
    throw flame::exceptions::logic_error("task with that name already exists");
  }
  // map task name to idx of new vector entry
  Task::id_type id = tasks_.size();  // use next index as id

#ifdef DEBUG
  if (Task::IsTermTask(id)) {
    throw flame::exceptions::out_of_range("Too many tasks");
  }
#endif

  name_map_.insert(lb, TaskNameMap::value_type(task_name, id));
  task_ptr->set_task_id(id);
  tasks_.push_back(task_ptr);

  // initialise entries for dependency management
  parents_.push_back(IdSet());
  children_.push_back(IdSet());
  roots_.insert(id);
  leaves_.insert(id);
}

//! Returns a task id given a task name
TaskManager::TaskId TaskManager::GetId(std::string task_name) const {
  try {
    return name_map_.at(task_name);
  }
  catch(const std::out_of_range& E) {
    throw flame::exceptions::invalid_argument("Unknown task");
  }
}

//! Returns a task reference given a task name
Task& TaskManager::GetTask(std::string task_name) {
  return GetTask(GetId(task_name));
}

//! Returns a task reference given a task id
Task& TaskManager::GetTask(TaskManager::TaskId task_id) {
  try {
    return tasks_.at(task_id);
  }
  catch(const std::exception& E) {
    throw flame::exceptions::invalid_argument("Invalid id");
  }
}

//! Adds a dependency between two registered tasks (by name)
void TaskManager::AddDependency(std::string task_name,
                                std::string dependency_name) {
  AddDependency(GetId(task_name), GetId(dependency_name));
}

//! Adds a dependency between two registered tasks (by id)
void TaskManager::AddDependency(TaskManager::TaskId task_id,
                                TaskManager::TaskId dependency_id) {
  if (!IsValidID(task_id) || !IsValidID(dependency_id)) {
    throw flame::exceptions::invalid_argument("Invalid id");
  }
  if (task_id == dependency_id) {
    throw flame::exceptions::logic_error("Task cannot depend on itself");
  }
  check_not_finalised(finalised_);
  boost::lock_guard<boost::mutex> lock(mutex_task_);

#ifdef DEBUG
  if (WillCauseCyclicDependency(task_id, dependency_id)) {
    throw flame::exceptions::logic_error("This will cause cyclic dependencies");
  }
#endif

  parents_[task_id].insert(dependency_id);
  children_[dependency_id].insert(task_id);
  roots_.erase(task_id);
  leaves_.erase(dependency_id);
}

//! Returns dependencies of the task as a set of ids
TaskManager::IdSet& TaskManager::GetDependencies(std::string task_name) {
  return GetDependencies(GetId(task_name));
}

//! Returns dependencies of the task as a set of ids
TaskManager::IdSet& TaskManager::GetDependencies(TaskManager::TaskId id) {
  try {
    return parents_.at(id);
  }
  catch(const std::out_of_range& E) {
    throw flame::exceptions::invalid_argument("Invalid id");
  }
}

#ifdef DEBUG
//! Returns true if a proposed dependency will result in a cyclic dependency
bool TaskManager::WillCauseCyclicDependency(TaskManager::TaskId task_id,
                                        TaskManager::TaskId target) {
  if (task_id == target) return true;

  TaskManager::TaskId current;
  TaskManager::IdSet visited;
  std::stack<TaskManager::TaskId> pending;

  // traverse upwards from target. If we do meet task_id then the proposed
  // dependency would lead to a cycle
  pending.push(target);
  while (!pending.empty()) {
    current = pending.top();
    pending.pop();
    if (current == task_id) return true;  // cycle detected

    visited.insert(current);

    BOOST_FOREACH(TaskManager::TaskId parent, parents_[current]) {
      if (visited.find(parent) == visited.end()) {  // if node not yet visited
        pending.push(parent);
      }
    }
  }
  return false;
}
#endif

//! Returns dependents of the task as a set of ids
TaskManager::IdSet& TaskManager::GetDependents(std::string task_name) {
  return GetDependents(GetId(task_name));
}

//! Returns dependents of the task as a set of ids
TaskManager::IdSet& TaskManager::GetDependents(TaskManager::TaskId id) {
  try {
    return children_.at(id);
  }
  catch(const std::out_of_range& E) {
    throw flame::exceptions::invalid_argument("Invalid id");
  }
}

//! Returns the number of registered tasks
size_t TaskManager::GetTaskCount() const {
#ifdef DEBUG
  if (tasks_.size() != children_.size() || tasks_.size() != parents_.size()) {
    throw flame::exceptions::flame_exception("inconsistent data sizes");
  }
#endif
  return tasks_.size();
}

//! Returns true if the given task id is valid
bool TaskManager::IsValidID(TaskManager::TaskId task_id) const {
  return (task_id < tasks_.size());
}

//! Indicates that new tasks can no longer be registered
void TaskManager::Finalise() {
  finalised_ = true;
  IterReset();
}

//! Returns true if the manager has been finalised
bool TaskManager::IsFinalised() const {
  return finalised_;
}

//! \brief Resets control data so a new iteration of tasks can begin
void TaskManager::IterReset() {
  check_finalised(finalised_);
  boost::lock_guard<boost::mutex> lock(mutex_task_);

  pending_deps_ = parents_;  // create copy of dependency tree
  assigned_tasks_.clear();
  ready_tasks_ = IdVector(roots_.begin(), roots_.end());  // tasks with no deps

  // reset and initialise
  pending_tasks_.clear();  // empty existing data
  for (size_t i = 0; i < tasks_.size(); ++i)  {
    if (roots_.find(i) == roots_.end()) {  // task not in ready queue
      pending_tasks_.insert(pending_tasks_.end(), i);
    }
  }
}

//! \brief Returns true if there are tasks ready for execution
bool TaskManager::IterTaskAvailable() const {
  check_finalised(finalised_);
  return (!ready_tasks_.empty());
}

//! \brief Returns true if all tasks have been executed
bool TaskManager::IterCompleted() const {
  check_finalised(finalised_);
  return (pending_tasks_.empty()
          && assigned_tasks_.empty()
          && ready_tasks_.empty());
}

//! \brief Returns the number of tasks ready for execution
size_t TaskManager::IterGetReadyCount() const {
  check_finalised(finalised_);
  return ready_tasks_.size();
}

//! \brief Returns the number of tasks that are still waiting for their
//! dependencies to be met
size_t TaskManager::IterGetPendingCount() const {
  check_finalised(finalised_);
  return pending_tasks_.size();
}

//! \brief Returns the number of tasks that have been assigned but not
//! completed.
size_t TaskManager::IterGetAssignedCount() const {
  check_finalised(finalised_);
  return assigned_tasks_.size();
}

/*!
 * \brief Pops and returns a task that is ready for execution
 *
 * Throws flame::exceptions::none_available if the queue is empty
 */
TaskManager::TaskId TaskManager::IterTaskPop() {
  check_finalised(finalised_);
  boost::lock_guard<boost::mutex> lock(mutex_task_);

  if (!IterTaskAvailable()) {
    throw flame::exceptions::none_available("No available tasks");
  }

  TaskManager::TaskId task_id = ready_tasks_.back();
  ready_tasks_.pop_back();
  assigned_tasks_.insert(task_id);
  return task_id;
}

//! \brief Indicates that a specific task has been completed
void TaskManager::IterTaskDone(TaskManager::TaskId task_id) {
  check_finalised(finalised_);
  boost::lock_guard<boost::mutex> lock(mutex_task_);

  IdSet::iterator it = assigned_tasks_.find(task_id);
  if (it == assigned_tasks_.end()) {
    throw flame::exceptions::invalid_argument("invalid task id");
  } else {
    assigned_tasks_.erase(it);
  }

  IdSet& dependents = children_.at(task_id);  // tasks that depend on task_id
  for (it = dependents.begin(); it != dependents.end(); ++it) {
    IdSet& d = pending_deps_.at(*it);  // get pending deps for each dependency
    d.erase(task_id);  // this dependency is now fulfilled
    if (d.empty()) {  // all dependencies met?
      ready_tasks_.push_back(*it);  // new task ready for execution
      pending_tasks_.erase(*it);
    }
  }
}


#ifdef TESTBUILD
void TaskManager::Reset() {
  boost::lock_guard<boost::mutex> lock(mutex_task_);

  tasks_.clear();
  name_map_.clear();
  roots_.clear();
  leaves_.clear();
  children_.clear();
  parents_.clear();
  finalised_ = false;
  assigned_tasks_.clear();
  ready_tasks_.clear();
  pending_tasks_.clear();
  pending_deps_.clear();
}
#endif

}}  // namespace flame::exe
