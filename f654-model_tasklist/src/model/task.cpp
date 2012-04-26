/*!
 * \file src/model/task.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Task: holds task information
 */
#include <cstdio>
#include <string>
#include <vector>
#include "./task.hpp"

namespace flame { namespace model {

Task::Task() {
    level_ = 0;
}

Task::~Task() {
    /* Delete dependencies */
    Dependency * dependency;
    while (!parents_.empty()) {
        dependency = parents_.back();
        delete dependency;
        parents_.pop_back();
    }
}

void Task::setTaskID(size_t id) {
    taskID_ = id;
}

size_t Task::getTaskID() {
    return taskID_;
}

void Task::setParentName(std::string name) {
    parentName_ = name;
}

std::string Task::getParentName() {
    return parentName_;
}

void Task::setName(std::string name) {
    name_ = name;
}

std::string Task::getName() {
    return name_;
}

void Task::setTaskType(TaskType type) {
    taskType_ = type;
}

Task::TaskType Task::getTaskType() {
    return taskType_;
}

void Task::setLevel(size_t level) {
    level_ = level;
}

size_t Task::getLevel() {
    return level_;
}

void Task::addParent(std::string name,
            Dependency::DependencyType type, Task * task) {
    Dependency * d = new Dependency;
    d->setName(name);
    d->setDependencyType(type);
    d->setTask(task);
    parents_.push_back(d);
}

void Task::addDependency(Dependency * d) {
    parents_.push_back(d);
}

std::vector<Dependency*> Task::getParents() {
    return parents_;
}

void Task::setPriorityLevel(size_t l) {
    priorityLevel_ = l;
}

size_t Task::getPriorityLevel() {
    return priorityLevel_;
}

}}  // namespace flame::model
