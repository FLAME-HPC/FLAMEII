/*!
 * \file flame2/model/xmodel.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XModel: holds model information
 */
#include <cstdio>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <utility>
#include "flame2/config.hpp"
#include "flame2/exceptions/model.hpp"
#include "xmodel.hpp"

namespace flame { namespace model {

XModel::XModel() {
  /* Initialise list of data types */
  addAllowedDataType("int");
  addAllowedDataType("float");
  addAllowedDataType("double");
  addAllowedDataType("char"); /* Allow? */
}

/*!
 * \brief Print a model to stdout
 *
 * Print a whole model out to standard out.
 */
void XModel::print() {
  boost::ptr_vector<XVariable>::iterator c_it;
  boost::ptr_vector<XTimeUnit>::iterator tu_it;
  boost::ptr_vector<XADT>::iterator adt_it;
  boost::ptr_vector<XMessage>::iterator m_it;
  boost::ptr_vector<XMachine>::iterator a_it;
  std::vector<std::string>::iterator s_it;

  std::printf("Model Name: %s\n", name_.c_str());
  std::printf("Constants:\n");
  for (c_it = constants_.begin(); c_it != constants_.end(); ++c_it)
    (*c_it).print();
  std::printf("Data types:\n");
  for (adt_it = adts_.begin(); adt_it != adts_.end(); ++adt_it)
    (*adt_it).print();
  std::printf("Time units:\n");
  for (tu_it = timeUnits_.begin(); tu_it != timeUnits_.end(); ++tu_it)
    (*tu_it).print();
  std::printf("Function files:\n");
  for (s_it = functionFiles_.begin(); s_it != functionFiles_.end(); ++s_it)
    std::printf("\t%s\n", (*s_it).c_str());
  std::printf("Agents:\n");
  for (a_it = agents_.begin(); a_it != agents_.end(); ++a_it)
    (*a_it).print();
  std::printf("Messages:\n");
  for (m_it = messages_.begin(); m_it != messages_.end(); ++m_it)
    (*m_it).print();
}

int XModel::validate() {
  int rc;

  // try and validate model
  XModelValidate validator(this);
  rc = validator.validate();

  // if fail then return code
  if (rc != 0) return rc;

  // if success then generate graph
  generateGraph();

  return 0;
}

void XModel::generateGraph() {
  boost::ptr_vector<XMachine>::iterator agent;
  std::set<DependencyGraph*> graphs;

  dependencyGraph_.setName(name_);

  // ToDo make xmachine only hold info
  // Make this function hold all XGraph agent graphs
  // Create state graph
  // Create dependency graph

  // Consolidate agent graphs into a model graph
  for (agent = agents_.begin();
      agent != agents_.end(); ++agent) {
    // Generate agent graph
    (*agent).generateDependencyGraph();
    graphs.insert((*agent).getDependencyGraph());
  }

  dependencyGraph_.importGraphs(graphs);
}

void XModel::setPath(std::string path) {
  path_ = path;
}

std::string XModel::getPath() {
  return path_;
}

void XModel::setName(std::string name) {
  // If name is not set then set name
  // This stops sub models renaming the root model
  if (name_ == "") name_ = name;
}

std::string XModel::getName() {
  return name_;
}

/*!
 * \brief Adds included model to a list
 * \param[in] path Path of the sub model
 * \return Boolean, true if name is unique
 * If the model file is not already in the list then it is added.
 * If not then false is returned.
 */
bool XModel::addIncludedModel(std::string path) {
  unsigned int ii;

  for (ii = 0; ii < includedModels_.size(); ++ii) {
    if (includedModels_.at(ii) == path) return false;
  }

  includedModels_.push_back(path);
  return true;
}

std::vector<std::string> * XModel::getIncludedModels() {
  return &includedModels_;
}

XVariable * XModel::addConstant() {
  XVariable * xvariable = new XVariable;
  constants_.push_back(xvariable);
  return xvariable;
}

boost::ptr_vector<XVariable> * XModel::getConstants() {
  return &constants_;
}

XADT * XModel::addADT() {
  XADT * xadt = new XADT;
  adts_.push_back(xadt);
  return xadt;
}

/*!
 * \brief Returns an adt object with given name
 * \param[in] name Name of the adt
 * \return Pointer to the adt object or 0 if not found
 * This function is used to validate adt names and provide a
 * pointer to the object if valid.
 */
XADT * XModel::getADT(std::string name) {
  boost::ptr_vector<XADT>::iterator it;
  for (it = adts_.begin(); it != adts_.end(); ++it)
    if (name == (*it).getName()) return &(*it);
  throw flame::exceptions::flame_model_exception(
      "ADT does not exist");
}

boost::ptr_vector<XADT> * XModel::getADTs() {
  return &adts_;
}

void XModel::addTimeUnit(XTimeUnit * tU) {
  boost::ptr_vector<XTimeUnit>::iterator it;

  // Check time unit does not already exist
  for (it = timeUnits_.begin(); it != timeUnits_.end(); ++it)
    if (tU->getName() == (*it).getName() &&
        tU->getUnit() == (*it).getUnit() &&
        tU->getPeriodString() == (*it).getPeriodString()) {
      // Free time unit
      delete tU;
      // Return and do not add to time units
      return;
    }

  // Add time unit id
  tU->setID(timeUnits_.size());
  // Add time unit to vector
  timeUnits_.push_back(tU);
}

boost::ptr_vector<XTimeUnit> * XModel::getTimeUnits() {
  return &timeUnits_;
}

void XModel::addFunctionFile(std::string file) {
  functionFiles_.push_back(file);
}

std::vector<std::string> * XModel::getFunctionFiles() {
  return &functionFiles_;
}

XMachine * XModel::addAgent(std::string name) {
  // Try and get agent
  XMachine * xmachine = getAgent(name);
  // If agent already exists then return it
  if (xmachine) return xmachine;
  // If agent does not exist then create new agent
  xmachine = new XMachine;
  // Assign name to new agent
  xmachine->setName(name);
  // Set ID
  xmachine->setID(agents_.size());
  agents_.push_back(xmachine);
  return xmachine;
}

boost::ptr_vector<XMachine> * XModel::getAgents() {
  return &agents_;
}

XMachine * XModel::getAgent(std::string name) {
  boost::ptr_vector<XMachine>::iterator it;
  for (it = agents_.begin(); it != agents_.end(); ++it)
    if (name == (*it).getName()) return &(*it);
  return 0;
}

XMessage * XModel::addMessage() {
  XMessage * xmessage = new XMessage;
  xmessage->setID(messages_.size());
  messages_.push_back(xmessage);
  return xmessage;
}

XMessage * XModel::addMessage(std::string name) {
  XMessage * xmessage = addMessage();
  xmessage->setName(name);
  return xmessage;
}

/*!
 * \brief Returns a message object with given name
 * \param[in] name Name of the message
 * \return Pointer to the message object or 0 if not found
 * This function is used to validate message names and provide a
 * pointer to the object if valid.
 */
XMessage * XModel::getMessage(std::string name) {
  boost::ptr_vector<XMessage>::iterator it;
  for (it =  messages_.begin(); it != messages_.end(); ++it)
    if (name == (*it).getName()) return &(*it);
  return 0;
}

boost::ptr_vector<XMessage> * XModel::getMessages() {
  return &messages_;
}

void XModel::addAllowedDataType(std::string name) {
  allowedDataTypes_.push_back(name);
}

std::vector<std::string> * XModel::getAllowedDataTypes() {
  return &allowedDataTypes_;
}

bool XModel::doesAgentExist(std::string agent_name) const {
  boost::ptr_vector<XMachine>::const_iterator it;
  for (it = agents_.begin(); it != agents_.end(); ++it)
    if (agent_name == (*it).getName()) return true;
  return false;
}

std::string XModel::getAgentVariableType(
        std::string agent_name, std::string var_name) const {
  boost::ptr_vector<XMachine>::const_iterator it;
  StringPairSet::const_iterator var;

  for (it = agents_.begin(); it != agents_.end(); ++it)
    if (agent_name == (*it).getName()) {
      StringPairSet variables = (*it).getVariablesSet();
      for (var = variables.begin(); var != variables.end(); ++var)
          if (var_name == (*var).second) return (*var).first;
      throw flame::exceptions::flame_model_exception(
            "Agent variable does not exist");
    }

  throw flame::exceptions::flame_model_exception("Agent does not exist");
}

StringSet XModel::getAgentNames() const {
  boost::ptr_vector<XMachine>::const_iterator it;
  StringSet agents;

  for (it = agents_.begin(); it != agents_.end(); ++it)
    agents.insert((*it).getName());

  return agents;
}

StringPairSet XModel::getAgentVariables(std::string agent_name) const {
  boost::ptr_vector<XMachine>::const_iterator it;

  for (it = agents_.begin(); it != agents_.end(); ++it)
    if (agent_name == (*it).getName()) return (*it).getVariablesSet();

  throw flame::exceptions::flame_model_exception("Agent does not exist");
}

AgentMemory XModel::getAgentMemoryInfo() const {
  boost::ptr_vector<XMachine>::const_iterator it;
  AgentMemory agentMemory;

  for (it = agents_.begin(); it != agents_.end(); ++it)
    agentMemory.insert(std::pair<std::string, std::vector<Var> >(
        (*it).getName(), (*it).getVariablesVector()) );

  return agentMemory;
}

TaskIdSet XModel::getAgentTasks() const {
  return dependencyGraph_.getAgentTasks();
}

TaskIdSet XModel::getAgentIOTasks() const {
  return dependencyGraph_.getAgentIOTasks();
}

TaskId XModel::getInitIOTask() const {
  return dependencyGraph_.getInitIOTask();
}

TaskId XModel::getFinIOTask() const {
  return dependencyGraph_.getFinIOTask();
}

TaskIdSet XModel::getMessageBoardSyncTasks() const {
  return dependencyGraph_.getMessageBoardSyncTasks();
}

TaskIdSet XModel::getMessageBoardClearTasks() const {
  return dependencyGraph_.getMessageBoardClearTasks();
}

TaskIdMap XModel::getTaskDependencies() const {
  return dependencyGraph_.getTaskDependencies();
}

std::string XModel::getTaskName(TaskId id) const {
  return dependencyGraph_.getTaskName(id);
}

std::string XModel::getTaskAgentName(TaskId id) const {
  return dependencyGraph_.getTaskAgentName(id);
}

std::string XModel::getTaskFunctionName(TaskId id) const {
  return dependencyGraph_.getTaskFunctionName(id);
}

StringSet XModel::getTaskReadOnlyVariables(TaskId id) const {
  return dependencyGraph_.getTaskReadOnlyVariables(id);
}

StringSet XModel::getTaskWriteVariables(TaskId id) const {
  return dependencyGraph_.getTaskWriteVariables(id);
}

StringSet XModel::getTaskOutputMessages(TaskId id) const {
  return dependencyGraph_.getTaskOutputMessages(id);
}

StringSet XModel::getTaskInputMessages(TaskId id) const {
  return dependencyGraph_.getTaskInputMessages(id);
}

void XModel::outputStateGraph(const std::string& file_name) const {
  stateGraph_.writeGraphviz(file_name);
}

void XModel::outputDependencyGraph(const std::string& file_name) const {
  dependencyGraph_.writeGraphviz(file_name);
}

int XModel::generateStateGraph() {
  // for each agent import state graph
  boost::ptr_vector<XMachine>::iterator agent;
  std::set<StateGraph*> graphs;

  stateGraph_.setName(name_);

  // Consolidate agent state graphs into a model state graph
  for (agent = agents_.begin();
      agent != agents_.end(); ++agent) {
    graphs.insert((*agent).getStateGraph());
  }

  stateGraph_.importStateGraphs(graphs);

  return 0;
}

int XModel::checkCyclicDependencies() {
  // ToDo
  return 0;
}

const TaskList * XModel::getTaskList() const {
  return dependencyGraph_.getTaskList();
}

}}  // namespace flame::model
