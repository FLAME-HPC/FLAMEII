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
#include <set>
#include "./task.hpp"

namespace flame { namespace model {

/*!
 * \brief Initialises Task
 *
 * Initialises Task and sets level to be zero.
 */
Task::Task(std::string name, TaskType type)
    : name_(name), taskType_(type) {
    level_ = 0;
    hasCondition_ = false;
}

void Task::setTaskID(size_t id) {
    taskID_ = id;
}

size_t Task::getTaskID() {
    return taskID_;
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

void Task::setPriorityLevel(size_t l) {
    priorityLevel_ = l;
}

size_t Task::getPriorityLevel() {
    return priorityLevel_;
}

void Task::setHasCondition(bool hasCondition) {
    hasCondition_ = hasCondition;
}

bool Task::hasCondition() {
    return hasCondition_;
}

void Task::addReadVariable(std::string name) {
    readVariables_.insert(name);
}

std::set<std::string> Task::getReadVariables() {
    return readVariables_;
}

void Task::addWriteVariable(std::string name) {
    writeVariables_.insert(name);
}

std::set<std::string> Task::getWriteVariables() {
    return writeVariables_;
}

}}  // namespace flame::model
