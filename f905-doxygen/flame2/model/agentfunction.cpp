/*!
 * \file flame2/model/agentfunction.cpp
 * \author Simon Coakley
 * \date 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief AgentFunction: holds agent function information
 */
#include <string>
#include <set>
#include "flame2/config.hpp"
#include "agentfunction.hpp"

namespace flame { namespace model {


AgentFunction::AgentFunction(std::string name,
    std::string current_state, std::string next_state)
: name_(name), currentState_(current_state), nextState_(next_state),
  inputs_(), outputs_(), readOnlyVariables_(), readWriteVariables_() {}

void AgentFunction::addInput(std::string name) {
  // insert input
  inputs_.insert(name);
}

void AgentFunction::addOutput(std::string name) {
  // insert output
  outputs_.insert(name);
}

void AgentFunction::addReadOnlyVariable(std::string name) {
  // insert read only variable
  readOnlyVariables_.insert(name);
}

void AgentFunction::addReadWriteVariable(std::string name) {
  // insert read write variable
  readWriteVariables_.insert(name);
}

std::string AgentFunction::getName() const {
  // return name
  return name_;
}

std::string AgentFunction::getCurrentState() const {
  // return current state
  return currentState_;
}

std::string AgentFunction::getNextState() const {
  // return next state
  return nextState_;
}

std::set<std::string> AgentFunction::getInputs() const {
  // return inputs
  return inputs_;
}

std::set<std::string> AgentFunction::getOutputs() const {
  // return outputs
  return outputs_;
}

std::set<std::string> AgentFunction::getReadOnlyVariables() const {
  // return read only variables
  return readOnlyVariables_;
}

std::set<std::string> AgentFunction::getReadWriteVariables() const {
  // return read write variables
  return readWriteVariables_;
}

}}  // namespace flame::model
