/*!
 * \file flame2/model/task.cpp
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
#include "flame2/config.hpp"
#include "task.hpp"

namespace flame { namespace model {

/*!
 * \brief Initialises Task
 *
 * Initialises Task and sets level to be zero.
 */
Task::Task(std::string parentName, std::string name, TaskType type)
  : parentName_(parentName), name_(name), taskType_(type), priorityLevel_(10),
    level_(0), hasCondition_(false),
    startTask_(false), endTask_(false) {
  if (type == Task::xmessage_sync) priorityLevel_ = 10;
  if (type == Task::xmessage_clear) priorityLevel_ = 1;
  if (type == Task::xfunction) priorityLevel_ = 5;
  if (type == Task::io_pop_write) priorityLevel_ = 0;
}

void Task::setStartTask(bool b) {
  startTask_ = b;
}

bool Task::startTask() {
  return startTask_;
}

void Task::setEndTask(bool b) {
  endTask_ = b;
}

bool Task::endTask() {
  return endTask_;
}

std::string Task::getTaskName() {
  std::string name;

  // If agent function
  if (taskType_ == Task::xfunction) {
    name.append("AF_");
    name.append(parentName_);
    // If agent condition
  } else if (taskType_ == Task::xcondition) {
    name.append("AC_");
    name.append(parentName_);
    // If agent data output
  } else if (taskType_ == Task::io_pop_write) {
    name.append("AD_");
    name.append(parentName_);
    // If model data output
  } else if (taskType_ == Task::start_model ||
      taskType_ == Task::finish_model) {
    name.append("MD_");
    name.append(parentName_);
    // If message sync
  } else if (taskType_ == Task::xmessage_sync) {
    name.append("MS");
    // If message clear
  } else if (taskType_ == Task::xmessage_clear) {
    name.append("MC");
  }
  name.append("_");
  name.append(name_);

  return name;
}

void Task::setParentName(std::string parentName) {
  parentName_ = parentName;
}

std::string Task::getParentName() const {
  return parentName_;
}

void Task::setName(std::string name) {
  name_ = name;
}

std::string Task::getName() const {
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

void Task::setHasCondition(bool hasCond) {
  hasCondition_ = hasCond;
}

bool Task::hasCondition() {
  return hasCondition_;
}

void Task::addReadWriteVariable(std::string name) {
  readVariables_.insert(name);
  writeVariables_.insert(name);
}

void Task::addReadOnlyVariable(std::string name) {
  readOnlyVariables_.insert(name);
  readVariables_.insert(name);
}

std::set<std::string>* Task::getReadOnlyVariables() {
  return &readOnlyVariables_;
}

void Task::addReadVariable(std::string name) {
  readVariables_.insert(name);
}

std::set<std::string>* Task::getReadVariables() {
  return &readVariables_;
}

void Task::addWriteVariable(std::string name) {
  writeVariables_.insert(name);
}

std::set<std::string>* Task::getWriteVariables() {
  return &writeVariables_;
}

VarMapToVertices * Task::getLastWrites() {
  return &lastWrites_;
}

VarMapToVertices * Task::getLastReads() {
  return &lastReads_;
}

std::set<size_t> * Task::getLastConditions() {
  return &lastConditions_;
}

void Task::addOutputMessage(std::string name) {
  outputMessages_.insert(name);
}

std::set<std::string>* Task::getOutputMessages() {
  return &outputMessages_;
}

void Task::addInputMessage(std::string name) {
  inputMessages_.insert(name);
}

std::set<std::string>* Task::getInputMessages() {
  return &inputMessages_;
}

std::set<std::string> Task::getReadOnlyVariablesConst() const {
  return readOnlyVariables_;
}

std::set<std::string> Task::getWriteVariablesConst() const {
  return writeVariables_;
}

std::set<std::string> Task::getOutputMessagesConst() const {
  return outputMessages_;
}

std::set<std::string> Task::getInputMessagesConst() const {
  return inputMessages_;
}

}}  // namespace flame::model
