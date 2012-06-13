/*!
 * \file src/exe/task_manager.cpp
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
#include "boost/foreach.hpp"
#include "task_manager.hpp"
#include "exceptions/all.hpp"

namespace flame { namespace exe {

Task& TaskManager::CreateTask(std::string task_name,
                              std::string agent_name,
                              TaskFunction func_ptr) {
  // Check for tasks with same name
  TaskNameMap::iterator lb = name_map_.lower_bound(task_name);
  if (lb != name_map_.end() && lb->first != task_name) {  // name exists
    throw flame::exceptions::logic_error("task with that name already exists");
  }
  // map task name to idx of new vector entry
  Task::id_type id = tasks_.size();  // use next index as id
  name_map_.insert(lb, TaskNameMap::value_type(task_name, id));
  Task *t = new Task(id, task_name, agent_name, func_ptr);
  tasks_.push_back(t);

  // initialise entries for dependency management
  parents_.push_back(IdSet());
  children_.push_back(IdSet());
  roots_.insert(id);
  leaves_.insert(id);

  return *t;
}


RunnableTask::id_type TaskManager::GetId(std::string task_name){
  try {
    return name_map_.at(task_name);
  }
  catch(const std::out_of_range& E) {
    throw flame::exceptions::invalid_argument("Unknown task");
  }
}


Task& TaskManager::GetTask(std::string task_name) {
  return GetTask(GetId(task_name));
}


Task& TaskManager::GetTask(RunnableTask::id_type task_id) {
  try {
    return tasks_.at(task_id);
  }
  catch(const std::exception& E) {
    throw flame::exceptions::invalid_argument("Invalid id");
  }
}


void TaskManager::AddDependency(std::string task_name,
                                std::string dependency_name) {
  AddDependency(GetId(task_name), GetId(dependency_name));
}


void TaskManager::AddDependency(RunnableTask::id_type task_id,
                                RunnableTask::id_type dependency_id) {
  if (!IsValidID(task_id) || !IsValidID(dependency_id)) {
    throw flame::exceptions::invalid_argument("Invalid id");
  }
  if (task_id == dependency_id) {
    throw flame::exceptions::logic_error("Task cannot depend on itself");
  }

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


TaskManager::IdSet& TaskManager::GetDependencies(std::string task_name) {
  return GetDependencies(GetId(task_name));
}


TaskManager::IdSet& TaskManager::GetDependencies(RunnableTask::id_type id) {
  try {
    return parents_.at(id);
  }
  catch (const std::out_of_range& E) {
    throw flame::exceptions::invalid_argument("Invalid id");
  }
}

#ifdef DEBUG
bool TaskManager::WillCauseCyclicDependency(RunnableTask::id_type task_id,
                                        RunnableTask::id_type target) {
  if (task_id == target) return true;

  RunnableTask::id_type current;
  std::set<RunnableTask::id_type> visited;
  std::stack<RunnableTask::id_type> pending;

  // traverse upwards from target. If we do meet task_id then the proposed
  // dependency would lead to a cycle
  pending.push(target);
  while (!pending.empty()) {
    current = pending.top();
    pending.pop();
    if (current == task_id) return true;  // cycle detected

    visited.insert(current);

    BOOST_FOREACH(RunnableTask::id_type parent, parents_[current]) {
      if (visited.find(parent) == visited.end()) {  // if node not yet visited
        pending.push(parent);
      }
    }
  }
  return false;
}
#endif

TaskManager::IdSet& TaskManager::GetDependents(std::string task_name) {
  return GetDependents(GetId(task_name));
}


TaskManager::IdSet& TaskManager::GetDependents(RunnableTask::id_type id) {
  try {
    return children_.at(id);
  }
  catch (const std::out_of_range& E) {
    throw flame::exceptions::invalid_argument("Invalid id");
  }
}

size_t TaskManager::get_task_count() {
#ifdef DEBUG
  if (tasks_.size() != children_.size() || tasks_.size() != parents_.size()) {
    throw flame::exceptions::flame_exception("inconsistent data sizes");
  }
#endif
  return tasks_.size();
}


bool TaskManager::IsValidID(RunnableTask::id_type task_id) {
  return (task_id < tasks_.size());
}


#ifdef TESTBUILD
void TaskManager::Reset() {
  tasks_.clear();
  name_map_.clear();
  roots_.clear();
  leaves_.clear();
  children_.clear();
  parents_.clear();
}
#endif

}}  // namespace flame::exe
