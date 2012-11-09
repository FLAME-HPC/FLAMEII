/*!
 * \file flame2/model/xfunction.cpp
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
#include "flame2/config.hpp"
#include "xfunction.hpp"

namespace flame { namespace model {

/*!
 * \brief Initialises XFunction
 *
 * Initialises XFunction with no condition and no memory access info.
 */
XFunction::XFunction()
    : condition_(0),
    memoryAccessInfoAvailable_(false) {}

XFunction::XFunction(std::string name)
    : name_(name),
    condition_(0),
    memoryAccessInfoAvailable_(false) {}

/*!
 * \brief Cleans up XFunction
 *
 * Cleans up XFunction by deleting condition and ioputs.
 */
XFunction::~XFunction() {
    /* Delete any condition */
    delete condition_;
}

/*!
 * \brief Prints the XFunction
 *
 * Prints the XFunction to standard out.
 */
void XFunction::print() {
    boost::ptr_vector<XIOput>::iterator it;
    std::fprintf(stdout, "\tFunction Name: %s\n", getName().c_str());
    std::fprintf(stdout, "\t\tCurrent State: %s\n", getCurrentState().c_str());
    std::fprintf(stdout, "\t\tNext State: %s\n", getNextState().c_str());
    if (condition_) {
        std::fprintf(stdout, "\t\tCondition:\n");
        condition_->print();
    }
    std::fprintf(stdout, "\t\tInputs:\n");
    for (it = inputs_.begin(); it != inputs_.end(); ++it) (*it).print();
    std::fprintf(stdout, "\t\tOutputs:\n");
    for (it = outputs_.begin(); it != outputs_.end(); ++it) (*it).print();
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

boost::ptr_vector<XIOput> * XFunction::getInputs() {
    return &inputs_;
}

XIOput * XFunction::addOutput() {
    XIOput * xoutput = new XIOput;
    outputs_.push_back(xoutput);
    return xoutput;
}

boost::ptr_vector<XIOput> * XFunction::getOutputs() {
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

void XFunction::setMemoryAccessInfoAvailable(bool b) {
    memoryAccessInfoAvailable_ = b;
}

bool XFunction::getMemoryAccessInfoAvailable() {
    return memoryAccessInfoAvailable_;
}

void XFunction::addReadOnlyVariable(std::string name) {
    readOnlyVariables_.push_back(name);
}

std::vector<std::string> * XFunction::getReadOnlyVariables() {
    return &readOnlyVariables_;
}

void XFunction::addReadWriteVariable(std::string name) {
    readWriteVariables_.push_back(name);
}

std::vector<std::string> * XFunction::getReadWriteVariables() {
    return &readWriteVariables_;
}

}}  // namespace flame::model
