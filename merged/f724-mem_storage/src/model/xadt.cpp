/*!
 * \file src/model/xadt.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XADT: holds abstract data type information
 */
#include <cstdio>
#include <string>
#include <vector>
#include "./xadt.hpp"

namespace flame { namespace model {

XADT::XADT() {
    holdsDynamicArray_ = false;
}

XADT::~XADT() {
    /* Delete variables */
    XVariable * var;
    while (!variables_.empty()) {
        var = variables_.back();
        delete var;
        variables_.pop_back();
    }
}

void XADT::print() {
    unsigned int ii;
    std::fprintf(stdout, "\tADT Name: %s\n", getName().c_str());
    for (ii = 0; ii < getVariables()->size(); ii++) {
        getVariables()->at(ii)->print();
    }
}

void XADT::setName(std::string name) {
    name_ = name;
}

std::string XADT::getName() {
    return name_;
}

XVariable * XADT::addVariable() {
    XVariable * xvariable = new XVariable;
    variables_.push_back(xvariable);
    return xvariable;
}

std::vector<XVariable*> * XADT::getVariables() {
    return &variables_;
}

void XADT::setHoldsDynamicArray(bool b) {
    holdsDynamicArray_ = b;
}

bool XADT::holdsDynamicArray() {
    return holdsDynamicArray_;
}

}}  // namespace flame::model
