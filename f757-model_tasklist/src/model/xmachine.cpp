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
#include "./xmachine.hpp"

namespace flame { namespace model {

XMachine::XMachine() {
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

}}  // namespace flame::model
