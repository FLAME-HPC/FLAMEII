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

int XMachine::findStartState() {
    // Map of state names and boolean for valid start state
    std::set<std::string> states;
    std::set<std::string>::iterator s;
    std::vector<XFunction*>::iterator f;

    // Reset state state value
    startState_ = "";

    // For each function add current state to possible start states list
    for (f = functions_.begin(); f != functions_.end(); ++f)
        states.insert((*f)->getCurrentState());
    // For each function cancel states that are next states
    for (f = functions_.begin(); f != functions_.end(); ++f) {
        s = states.find((*f)->getNextState());
        // If state is valid, remove from start state list
        if (s != states.end()) states.erase(s);
    }
    // No start states found
    if (states.size() == 0) {
        std::fprintf(stderr,
            "Error: %s agent doesn't have a start state\n", name_.c_str());
        return 1;
    }
    // Multiple start states found
    if (states.size() > 1) {
        std::fprintf(stderr,
            "Error: %s agent has multiple possible start states:\n",
            name_.c_str());
        for (s = states.begin(); s != states.end(); s++)
            std::fprintf(stderr, "\t%s\n", s->c_str());
        return 2;
    }
    // One start state
    startState_ = (*states.begin());

    return 0;
}

std::string XMachine::getStartState() {
    return startState_;
}

int XMachine::generateDependencyGraph() {

    functionDependencyGraph_.write_graphviz("test1.dot");

    // Contract state vertices
    functionDependencyGraph_.contractStateVerticies();

    functionDependencyGraph_.write_graphviz("test2.dot");

    // Add condition vertices
    functionDependencyGraph_.add_condition_vertices_to_graph();

    functionDependencyGraph_.write_graphviz("test3.dot");

    // Add init vertex
    add_init_vertex_to_graph();
    // Add condition dependencies
    functionDependencyGraph_.add_condition_dependencies();
    // Add variable vertices
    functionDependencyGraph_.add_variable_vertices_to_graph(getVariables());
    // Remove state dependencies
    functionDependencyGraph_.remove_state_dependencies();


    functionDependencyGraph_.write_graphviz("test4.dot");

    // Contract variable vertices
    functionDependencyGraph_.contract_variable_verticies_from_graph();
    // Remove init vertex
    functionDependencyGraph_.remove_init_task();

    functionDependencyGraph_.write_graphviz("test5.dot");

    functionDependencyGraph_.remove_redendant_dependencies();

    functionDependencyGraph_.write_graphviz("test6.dot");

    return 0;
}

/*
 * This function is called from the model validator and
 * is then used to check for cycles and function conditions.
 */
int XMachine::generateStateGraph() {
    functionDependencyGraph_.addTransitionFunctions(functions_);

    functionDependencyGraph_.write_graphviz("test1.dot");

    return 0;
}

int XMachine::add_init_vertex_to_graph() {
    // (makes other algorithms simpler)
    XFunction * initFunc = new XFunction;
    initFunc->setName("Init");
    Vertex v = functionDependencyGraph_.add_init_task_to_graph(initFunc);
    functionDependencyGraph_.setStartVector(v);
    // Make init function write all memory variables
    std::vector<XVariable*>::iterator i;
    for (i = variables_.begin(); i != variables_.end(); i++)
        initFunc->getReadWriteVariables()->push_back((*i)->getName());
    // Add edges from init function to vertices from start state
    std::vector<XFunction*>::iterator f;
    for (f = functions_.begin(); f != functions_.end(); ++f) {
        if ((*f)->getCurrentState() == startState_) {
            Dependency * d = new Dependency;
            d->setParentName(getName());
            d->setName((*f)->getCurrentState());
            d->setDependencyType(Dependency::init);
            functionDependencyGraph_.addEdge(initFunc->getTask(), (*f)->getTask(), d);
        }
    }

    return 0;
}

XGraph * XMachine::getFunctionDependencyGraph() {
    return &functionDependencyGraph_;
}

int XMachine::checkCyclicDependencies() {
    return functionDependencyGraph_.check_dependency_loops();
}

int XMachine::checkFunctionConditions() {
    return functionDependencyGraph_.check_function_conditions();
}

}}  // namespace flame::model
