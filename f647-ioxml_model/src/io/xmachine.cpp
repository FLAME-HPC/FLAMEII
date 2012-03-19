/*!
 * \file src/io/xmachine.cpp
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
#include "./xmodel.hpp"

namespace flame { namespace io {

XMachine::~XMachine() {
    /* Delete variables */
    XVariable * var;
    while (!variables_.empty()) {
        var = variables_.back();
        delete var;
        variables_.pop_back();
    }
}

void XMachine::print() {
    unsigned int ii;
    std::fprintf(stdout, "Agent Name: %s\n", getName().c_str());
    for (ii = 0; ii < getVariables()->size(); ii++) {
        getVariables()->at(ii)->print();
    }
}

void XMachine::setName(std::string name) {
    name_ = name;
}

std::string XMachine::getName() {
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

}}  // namespace flame::io
