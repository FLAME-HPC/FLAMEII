/*!
 * \file flame2/model/xmessage.cpp
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
#include "flame2/config.hpp"
#include "xmessage.hpp"

namespace flame { namespace model {

XMessage::XMessage() {
    syncFinishTask_ = 0;
    syncStartTask_ = 0;
}

/*!
 * \brief Cleans up XMessage
 *
 * Cleans up XMessage by deleting the variables list.
 */
XMessage::~XMessage() {
    /* Delete variables */
    while (!variables_.empty()) {
        delete variables_.back();
        variables_.pop_back();
    }
}

/*!
 * \brief Prints XMessage
 *
 * Prints XMessage to standard out.
 */
void XMessage::print() {
    unsigned int ii;
    std::fprintf(stdout, "\tMessage Name: %s\n", getName().c_str());
    for (ii = 0; ii < getVariables()->size(); ii++)
        getVariables()->at(ii)->print();
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

bool XMessage::validateVariableName(std::string name) {
    unsigned int ii;
    for (ii = 0; ii < variables_.size(); ii++)
        if (name == variables_.at(ii)->getName()) return true;
    return false;
}

void XMessage::setSyncStartTask(Task * task) {
    syncStartTask_ = task;
}

Task * XMessage::getSyncStartTask() {
    return syncStartTask_;
}

void XMessage::setSyncFinishTask(Task * task) {
    syncFinishTask_ = task;
}

Task * XMessage::getSyncFinishTask() {
    return syncFinishTask_;
}

}}  // namespace flame::model
