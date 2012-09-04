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

/*!
 * \brief Initialises Task
 *
 * Initialises Task and sets level to be zero.
 */
Task::Task() {
    level_ = 0;
    function_ = 0;
}

Task::~Task() {
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

std::string Task::getFullName() {
    /* Return full name made from name and parent name */
    std::string fullName = parentName_ + "." + name_;
    return fullName;
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

void Task::setFunction(XFunction * f) {
    function_ = f;
}

XFunction * Task::getFunction() {
    return function_;
}

}}  // namespace flame::model
