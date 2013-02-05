/*!
 * \file flame2/model/xmachine.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XMachine: holds agent information
 */
#include <cstdio>
#include <string>
#include <vector>
#include <set>
#include <utility>
#include "flame2/config.hpp"
#include "flame2/mem/memory_manager.hpp"
#include "flame2/exceptions/model.hpp"
#include "xmachine.hpp"

namespace flame { namespace model {

XMachine::XMachine() : id_(0) {}

/*!
 * \brief Prints XMachine
 *
 * Prints XMachine to standard out.
 */
void XMachine::print() {
  boost::ptr_vector<XVariable>::iterator it;
  boost::ptr_vector<XFunction>::iterator f_it;

  std::fprintf(stdout, "\tAgent Name: %s\n", getName().c_str());
  for (it = variables_.begin(); it != variables_.end(); it++)
    (*it).print();
  for (f_it = functions_.begin(); f_it != functions_.end(); ++f_it)
    (*f_it).print();
}

void XMachine::setName(std::string name) {
  name_ = name;
  functionDependencyGraph_.setAgentName(name);
}

std::string XMachine::getName() const {
  return name_;
}

XVariable * XMachine::addVariable() {
  XVariable * xvariable = new XVariable;
  variables_.push_back(xvariable);
  return xvariable;
}

XVariable * XMachine::addVariable(std::string type, std::string name) {
  XVariable * xvariable = addVariable();
  xvariable->setType(type);
  xvariable->setName(name);
  return xvariable;
}

boost::ptr_vector<XVariable> * XMachine::getVariables() {
  return &variables_;
}

std::set<Var> XMachine::getVariablesSet() const {
  boost::ptr_vector<XVariable>::const_iterator it;
  std::set<Var> varSet;

  for (it = variables_.begin(); it != variables_.end(); it++)
    varSet.insert(Var((*it).getType(), (*it).getName()));

  return varSet;
}

std::vector<Var> XMachine::getVariablesVector() const {
  boost::ptr_vector<XVariable>::const_iterator it;
  std::vector<Var> varVec;

  for (it = variables_.begin(); it != variables_.end(); it++)
    varVec.push_back(std::make_pair((*it).getType(), (*it).getName()));

  return varVec;
}

XVariable * XMachine::getVariable(std::string name) {
  boost::ptr_vector<XVariable>::iterator it;
  for (it = variables_.begin(); it != variables_.end(); it++)
    if ((*it).getName() == name) return &(*it);
  return 0;
}

XFunction * XMachine::addFunction() {
  XFunction * xfunction = new XFunction;
  functions_.push_back(xfunction);
  return xfunction;
}

XFunction * XMachine::addFunction(std::string name,
    std::string currentState, std::string nextState) {
  XFunction * xfunction = addFunction();
  xfunction->setName(name);
  xfunction->setCurrentState(currentState);
  xfunction->setNextState(nextState);
  return xfunction;
}

XFunction * XMachine::getFunction(std::string name,
    std::string current_state, std::string next_state) {
  boost::ptr_vector<XFunction>::iterator func;
  for (func = functions_.begin(); func != functions_.end();
      ++func) {
    if ((*func).getName() == name &&
        (*func).getCurrentState() == current_state &&
        (*func).getNextState() == next_state)
      return &(*func);
  }
  throw flame::exceptions::flame_model_exception(
      "Function does not exist");
}

boost::ptr_vector<XFunction> * XMachine::getFunctions() {
  return &functions_;
}

bool XMachine::validateVariableName(std::string name) {
  boost::ptr_vector<XVariable>::iterator it;
  for (it = variables_.begin(); it != variables_.end(); it++)
    if (name == (*it).getName()) return true;
  return false;
}

int XMachine::findStartEndStates() {
  // Map of state names and boolean for valid start state
  std::set<std::string> startStates;
  std::set<std::string>::iterator s;
  boost::ptr_vector<XFunction>::iterator f;

  // Reset end states list
  endStates_.clear();

  // For each function
  for (f = functions_.begin(); f != functions_.end(); ++f) {
    // Add current state to possible end states list
    endStates_.insert((*f).getNextState());
    // Add current state to possible start states list
    startStates.insert((*f).getCurrentState());
  }
  // For each function
  for (f = functions_.begin(); f != functions_.end(); ++f) {
    // If start states contain a next state then remove
    s = startStates.find((*f).getNextState());
    if (s != startStates.end()) startStates.erase(s);
    // If end states contain a current state then remove
    s = endStates_.find((*f).getCurrentState());
    if (s != endStates_.end()) endStates_.erase(s);
  }
  // No start states found
  if (startStates.size() == 0) return 1;
  // Multiple start states found
  else if (startStates.size() > 1) return 2;
  // One start state
  startState_ = (*startStates.begin());

  return 0;
}

std::string XMachine::getStartState() {
  return startState_;
}

std::set<std::string> XMachine::getEndStates() {
  return endStates_;
}

int XMachine::generateDependencyGraph() {
  return functionDependencyGraph_.generateDependencyGraph(getVariables());
}

/*
 * This function is called from the model validator and
 * is then used to check for cycles and function conditions.
 */
int XMachine::generateStateGraph() {
  return functionDependencyGraph_.generateStateGraph(
      &functions_, startState_, endStates_);
}

XGraph * XMachine::getFunctionDependencyGraph() {
  return &functionDependencyGraph_;
}

std::pair<int, std::string> XMachine::checkCyclicDependencies() {
  return functionDependencyGraph_.checkCyclicDependencies();
}

std::pair<int, std::string> XMachine::checkFunctionConditions() {
  return functionDependencyGraph_.checkFunctionConditions();
}

void XMachine::addToModelGraph(XGraph * modelGraph) {
  modelGraph->import(&functionDependencyGraph_);
}

void XMachine::setID(int id) {
  id_ = id;
}

int XMachine::getID() {
  return id_;
}

}}  // namespace flame::model
