/*!
 * \file flame2/model/model_task.cpp
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
#include "model_task.hpp"

namespace flame { namespace model {

/*!
 * \brief Initialises Task
 *
 * Initialises Task and sets level to be zero.
 */
ModelTask::ModelTask(std::string parentName, std::string name, TaskType type)
  : parentName_(parentName), name_(name), taskType_(type), priorityLevel_(10),
    level_(0), hasCondition_(false),
    startTask_(false), endTask_(false) {
  // set priority level depending on task type
  if (type == ModelTask::xmessage_sync) priorityLevel_ = 10;
  if (type == ModelTask::xmessage_clear) priorityLevel_ = 1;
  if (type == ModelTask::xfunction) priorityLevel_ = 5;
  if (type == ModelTask::io_pop_write) priorityLevel_ = 0;
}

void ModelTask::setStartTask(bool b) {
  startTask_ = b;
}

bool ModelTask::startTask() {
  return startTask_;
}

void ModelTask::setEndTask(bool b) {
  endTask_ = b;
}

bool ModelTask::endTask() {
  return endTask_;
}

std::string ModelTask::getTaskName() {
  std::string name;

  // if agent function
  if (taskType_ == ModelTask::xfunction) {
    name.append("AF_");
    name.append(parentName_);
  // if agent condition
  } else if (taskType_ == ModelTask::xcondition) {
    name.append("AC_");
    name.append(parentName_);
  // if agent data output
  } else if (taskType_ == ModelTask::io_pop_write) {
    name.append("AD_");
    name.append(parentName_);
  // if model data output
  } else if (taskType_ == ModelTask::start_model ||
      taskType_ == ModelTask::finish_model) {
    name.append("MD_");
    name.append(parentName_);
  // if message sync
  } else if (taskType_ == ModelTask::xmessage_sync) {
    name.append("MS");
  // if message clear
  } else if (taskType_ == ModelTask::xmessage_clear) {
    name.append("MC");
  }
  // add task name
  name.append("_");
  name.append(name_);

  return name;
}

void ModelTask::setParentName(std::string parentName) {
  parentName_ = parentName;
}

std::string ModelTask::getParentName() const {
  return parentName_;
}

void ModelTask::setName(std::string name) {
  name_ = name;
}

std::string ModelTask::getName() const {
  return name_;
}

void ModelTask::setTaskType(TaskType type) {
  taskType_ = type;
}

ModelTask::TaskType ModelTask::getTaskType() {
  return taskType_;
}

void ModelTask::setLevel(size_t level) {
  level_ = level;
}

size_t ModelTask::getLevel() {
  return level_;
}

void ModelTask::setPriorityLevel(size_t l) {
  priorityLevel_ = l;
}

size_t ModelTask::getPriorityLevel() {
  return priorityLevel_;
}

void ModelTask::setHasCondition(bool hasCond) {
  hasCondition_ = hasCond;
}

bool ModelTask::hasCondition() {
  return hasCondition_;
}

void ModelTask::addReadWriteVariable(std::string name) {
  // add var name to read and variable lists
  readVariables_.insert(name);
  writeVariables_.insert(name);
}

void ModelTask::addReadOnlyVariable(std::string name) {
  // add var name to read only and variable lists
  readOnlyVariables_.insert(name);
  readVariables_.insert(name);
}

std::set<std::string>* ModelTask::getReadOnlyVariables() {
  return &readOnlyVariables_;
}

void ModelTask::addReadVariable(std::string name) {
  readVariables_.insert(name);
}

std::set<std::string>* ModelTask::getReadVariables() {
  return &readVariables_;
}

void ModelTask::addWriteVariable(std::string name) {
  writeVariables_.insert(name);
}

std::set<std::string>* ModelTask::getWriteVariables() {
  return &writeVariables_;
}

VarMapToVertices * ModelTask::getLastWrites() {
  return &lastWrites_;
}

VarMapToVertices * ModelTask::getLastReads() {
  return &lastReads_;
}

std::set<size_t> * ModelTask::getLastConditions() {
  return &lastConditions_;
}

void ModelTask::addOutputMessage(std::string name) {
  outputMessages_.insert(name);
}

std::set<std::string>* ModelTask::getOutputMessages() {
  return &outputMessages_;
}

void ModelTask::addInputMessage(std::string name) {
  inputMessages_.insert(name);
}

std::set<std::string>* ModelTask::getInputMessages() {
  return &inputMessages_;
}

std::set<std::string> ModelTask::getReadOnlyVariablesConst() const {
  return readOnlyVariables_;
}

std::set<std::string> ModelTask::getWriteVariablesConst() const {
  return writeVariables_;
}

std::set<std::string> ModelTask::getOutputMessagesConst() const {
  return outputMessages_;
}

std::set<std::string> ModelTask::getInputMessagesConst() const {
  return inputMessages_;
}

}}  // namespace flame::model
