/*!
 * \file src/model/xfunction.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XFunction: holds transition function information
 */
#include <cstdio>
#include <string>
#include <vector>
#include <stdexcept>
#include "./xfunction.hpp"

namespace flame { namespace model {

XFunction::XFunction() {
    condition_ = 0;
}

XFunction::~XFunction() {
    /* Delete inputs */
    XIOput * xinput;
    while (!inputs_.empty()) {
        xinput = inputs_.back();
        delete xinput;
        inputs_.pop_back();
    }
    /* Delete outputs */
    XIOput * xoutput;
    while (!outputs_.empty()) {
        xoutput = outputs_.back();
        delete xoutput;
        outputs_.pop_back();
    }
    /* Delete any condition */
    if (condition_ != 0) delete condition_;
}

void XFunction::print() {
    unsigned int ii;
    std::fprintf(stdout, "\tFunction Name: %s\n", getName().c_str());
    std::fprintf(stdout, "\t\tCurrent State: %s\n", getCurrentState().c_str());
    std::fprintf(stdout, "\t\tNext State: %s\n", getNextState().c_str());
    if (condition_ != 0) {
        std::fprintf(stdout, "\t\tCondition:\n");
        condition_->print();
    }
    std::fprintf(stdout, "\t\tInputs:\n");
    for (ii = 0; ii < inputs_.size(); ii++) {
        inputs_.at(ii)->print();
    }
    std::fprintf(stdout, "\t\tOutputs:\n");
    for (ii = 0; ii < outputs_.size(); ii++) {
        outputs_.at(ii)->print();
    }
}

void XFunction::setName(std::string name) {
    name_ = name;
}

std::string XFunction::getName() {
    return name_;
}

void XFunction::setCurrentState(std::string name) {
    currentState_ = name;
}

std::string XFunction::getCurrentState() {
    return currentState_;
}

void XFunction::setNextState(std::string name) {
    nextState_ = name;
}

std::string XFunction::getNextState() {
    return nextState_;
}

XIOput * XFunction::addInput() {
    XIOput * xinput = new XIOput;
    inputs_.push_back(xinput);
    return xinput;
}

std::vector<XIOput*> * XFunction::getInputs() {
    return &inputs_;
}

XIOput * XFunction::addOutput() {
    XIOput * xoutput = new XIOput;
    outputs_.push_back(xoutput);
    return xoutput;
}

std::vector<XIOput*> * XFunction::getOutputs() {
    return &outputs_;
}

XCondition * XFunction::addCondition() {
    if (condition_ == 0) {
        condition_ = new XCondition;
    } else {
        throw std::invalid_argument(
            "a condition has already been added to the function");
    }
    return condition_;
}

XCondition * XFunction::getCondition() {
    return condition_;
}

}}  // namespace flame::model
