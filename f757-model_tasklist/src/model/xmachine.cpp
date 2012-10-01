/*!
 * \file src/model/xmachine.cpp
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
#include "./xmachine.hpp"
#include "../mem/memory_manager.hpp"

namespace flame { namespace model {

XMachine::XMachine() {
    name_ = "";
    startState_ = "";
}

/*!
 * \brief Cleans up XMachine
 *
 * Cleans up XMachine by deleting variable list and functions list.
 */
XMachine::~XMachine() {
    /* Delete variables */
    while (!variables_.empty()) {
        delete variables_.back();
        variables_.pop_back();
    }
    /* Delete functions */
    while (!functions_.empty()) {
        delete functions_.back();
        functions_.pop_back();
    }
}

/*!
 * \brief Prints XMachine
 *
 * Prints XMachine to standard out.
 */
void XMachine::print() {
    unsigned int ii;
    std::fprintf(stdout, "\tAgent Name: %s\n", getName().c_str());
    for (ii = 0; ii < getVariables()->size(); ii++)
        getVariables()->at(ii)->print();
    for (ii = 0; ii < functions_.size(); ii++)
        functions_.at(ii)->print();
}

void XMachine::setName(std::string name) {
    name_ = name;
    functionDependencyGraph_.setAgentName(name);
}

const std::string XMachine::getName() {
    return name_;
}

XVariable * XMachine::addVariable() {
    XVariable * xvariable = new XVariable;
    variables_.push_back(xvariable);
    return xvariable;
}

std::vector<XVariable*> * XMachine::getVariables() {
    return &variables_;
}

XVariable * XMachine::getVariable(std::string name) {
    unsigned int ii;
    for (ii = 0; ii < variables_.size(); ii++)
        if (variables_.at(ii)->getName() == name) return variables_.at(ii);
    return 0;
}

XFunction * XMachine::addFunction() {
    XFunction * xfunction = new XFunction;
    functions_.push_back(xfunction);
    return xfunction;
}

std::vector<XFunction*> * XMachine::getFunctions() {
    return &functions_;
}

bool XMachine::validateVariableName(std::string name) {
    unsigned int ii;
    for (ii = 0; ii < variables_.size(); ii++)
        if (name == variables_.at(ii)->getName()) return true;
    return false;
}

int XMachine::findStartEndStates() {
    // Map of state names and boolean for valid start state
    std::set<std::string> startStates;
    std::set<std::string>::iterator s;
    std::vector<XFunction*>::iterator f;

    // Reset end states list
    endStates_.clear();

    // For each function
    for (f = functions_.begin(); f != functions_.end(); ++f) {
        // Add current state to possible end states list
        endStates_.insert((*f)->getNextState());
        // Add current state to possible start states list
        startStates.insert((*f)->getCurrentState());
    }
    // For each function
    for (f = functions_.begin(); f != functions_.end(); ++f) {
        // If start states contain a next state then remove
        s = startStates.find((*f)->getNextState());
        if (s != startStates.end()) startStates.erase(s);
        // If end states contain a current state then remove
        s = endStates_.find((*f)->getCurrentState());
        if (s != endStates_.end()) endStates_.erase(s);
    }
    if (startStates.size() == 0) {
        // No start states found
        std::fprintf(stderr,
            "Error: %s agent doesn't have a start state\n", name_.c_str());
        return 1;
    } else if (startStates.size() > 1) {
        // Multiple start states found
        std::fprintf(stderr,
    "Error: %s agent has multiple possible start states:\n", name_.c_str());
        for (s = startStates.begin(); s != startStates.end(); s++)
            std::fprintf(stderr, "\t%s\n", s->c_str());
        return 2;
    }
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
            functions_, startState_, endStates_);
}

XGraph * XMachine::getFunctionDependencyGraph() {
    return &functionDependencyGraph_;
}

int XMachine::checkCyclicDependencies() {
    return functionDependencyGraph_.checkCyclicDependencies();
}

int XMachine::checkFunctionConditions() {
    return functionDependencyGraph_.checkFunctionConditions();
}

int XMachine::registerWithMemoryManager() {
    std::vector<XVariable*>::iterator vit;
    size_t pop_size_hint = 100;
        flame::mem::MemoryManager& memoryManager =
                    flame::mem::MemoryManager::GetInstance();

    /* Register agent with memory manager */
    memoryManager.RegisterAgent(name_);
    /* Register agent memory variables */
    for (vit = variables_.begin(); vit != variables_.end(); vit++) {
        if ((*vit)->getType() == "int") {
            /* Register int variable */
            memoryManager.RegisterAgentVar<int>
                (name_, (*vit)->getName());
        } else if ((*vit)->getType() == "double") {
            /* Register double variable */
            memoryManager.RegisterAgentVar<double>
                (name_, (*vit)->getName());
        }
    }
    /* Population Size hint */
    memoryManager.HintPopulationSize(name_, pop_size_hint);

    return 0;
}

void XMachine::addToModelGraph(XGraph * modelGraph) {
    modelGraph->import(&functionDependencyGraph_);
}

}}  // namespace flame::model
