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

/*!
 * \brief Initialises XFunction
 *
 * Initialises XFunction with no condition and no memory access info.
 */
XFunction::XFunction() {
    /* Initialise pointers */
    condition_ = 0;
    /* Default is that there is no memory access information */
    memoryAccessInfoAvailable_ = false;
}

/*!
 * \brief Cleans up XFunction
 *
 * Cleans up XFunction by deleting condition and ioputs.
 */
XFunction::~XFunction() {
    /* Delete inputs */
    while (!inputs_.empty()) {
        delete inputs_.back();
        inputs_.pop_back();
    }
    /* Delete outputs */
    while (!outputs_.empty()) {
        delete outputs_.back();
        outputs_.pop_back();
    }
    /* Delete any condition */
    if (condition_ != 0) delete condition_;
}

/*!
 * \brief Prints the XFunction
 *
 * Prints the XFunction to standard out.
 */
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
    for (ii = 0; ii < inputs_.size(); ii++) inputs_.at(ii)->print();
    std::fprintf(stdout, "\t\tOutputs:\n");
    for (ii = 0; ii < outputs_.size(); ii++) outputs_.at(ii)->print();
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
    if (condition_ == 0) condition_ = new XCondition;
    else
        throw std::invalid_argument(
            "a condition has already been added to the function");
    return condition_;
}

XCondition * XFunction::getCondition() {
    return condition_;
}

void XFunction::setTask(Task * task) {
    task_ = task;
}

Task * XFunction::getTask() {
    return task_;
}

void XFunction::setMemoryAccessInfoAvailable(bool b) {
    memoryAccessInfoAvailable_ = b;
}

bool XFunction::getMemoryAccessInfoAvailable() {
    return memoryAccessInfoAvailable_;
}

void XFunction::addReadOnlyVariable(XVariable * v) {
    readOnlyVariables_.push_back(v);
}

std::vector<XVariable*> * XFunction::getReadOnlyVariables() {
    return &readOnlyVariables_;
}

void XFunction::addReadWriteVariable(XVariable * v) {
    readWriteVariables_.push_back(v);
}

std::vector<XVariable*> * XFunction::getReadWriteVariables() {
    return &readWriteVariables_;
}

}}  // namespace flame::model
