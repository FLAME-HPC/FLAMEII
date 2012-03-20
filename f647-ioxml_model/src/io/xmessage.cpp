/*!
 * \file src/io/xmessage.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XMessage: holds message information
 */
#include <cstdio>
#include <string>
#include <vector>
#include "./xmessage.hpp"

namespace flame { namespace io {

XMessage::~XMessage() {
    /* Delete variables */
    XVariable * var;
    while (!variables_.empty()) {
        var = variables_.back();
        delete var;
        variables_.pop_back();
    }
}

void XMessage::print() {
    unsigned int ii;
    std::fprintf(stdout, "\tMessage Name: %s\n", getName().c_str());
    for (ii = 0; ii < getVariables()->size(); ii++) {
        getVariables()->at(ii)->print();
    }
}

void XMessage::setName(std::string name) {
    name_ = name;
}

std::string XMessage::getName() {
    return name_;
}

XVariable * XMessage::addVariable() {
    XVariable * xvariable = new XVariable;
    variables_.push_back(xvariable);
    return xvariable;
}

std::vector<XVariable*> * XMessage::getVariables() {
    return &variables_;
}

}}  // namespace flame::io
