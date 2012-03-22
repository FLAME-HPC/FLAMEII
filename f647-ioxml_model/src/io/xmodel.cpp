/*!
 * \file src/io/xmodel.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XModel: holds model information
 */
#include <cstdio>
#include <string>
#include <vector>
#include "./xmodel.hpp"

namespace flame { namespace io {

XModel::~XModel() {
    clear();
}

int XModel::clear() {
    name_ = "";
    /* Delete variables */
    XVariable * var;
    while (!constants_.empty()) {
        var = constants_.back();
        delete var;
        constants_.pop_back();
    }
    /* Clear adts vector */
    XADT * xadt;
    while (!adts_.empty()) {
        xadt = adts_.back();
        delete xadt;
        adts_.pop_back();
    }
    /* Clear time units vector */
    XTimeUnit * xtimeunit;
    while (!timeUnits_.empty()) {
        xtimeunit = timeUnits_.back();
        delete xtimeunit;
        timeUnits_.pop_back();
    }
    /* Clear agents vector */
    XMachine * xm;
    while (!agents_.empty()) {
        xm = agents_.back();
        delete xm;
        agents_.pop_back();
    }
    /* Clear messages vector */
    XMessage * xmessage;
    while (!messages_.empty()) {
        xmessage = messages_.back();
        delete xmessage;
        messages_.pop_back();
    }

    return 0;
}

void XModel::print() {
    std::fprintf(stdout, "Model Name: %s\n", name_.c_str());
    unsigned int ii;
    std::fprintf(stdout, "Constants:\n");
    for (ii = 0; ii < getConstants()->size(); ii++) {
            getConstants()->at(ii)->print();
    }
    std::fprintf(stdout, "Data types:\n");
    for (ii = 0; ii < adts_.size(); ii++) {
        adts_[ii]->print();
    }
    std::fprintf(stdout, "Time units:\n");
    for (ii = 0; ii < timeUnits_.size(); ii++) {
        timeUnits_[ii]->print();
    }
    std::fprintf(stdout, "Function files:\n");
    for (ii = 0; ii < functionFiles_.size(); ii++) {
        std::fprintf(stdout, "\t%s\n", functionFiles_[ii].c_str());
    }
    std::fprintf(stdout, "Agents:\n");
    for (ii = 0; ii < agents_.size(); ii++) {
        agents_[ii]->print();
    }
    std::fprintf(stdout, "Messages:\n");
    for (ii = 0; ii < messages_.size(); ii++) {
        messages_[ii]->print();
    }
}

XVariable * XModel::addConstant() {
    XVariable * xvariable = new XVariable;
    constants_.push_back(xvariable);
    return xvariable;
}

std::vector<XVariable*> * XModel::getConstants() {
    return &constants_;
}

XADT * XModel::addADT() {
    XADT * xadt = new XADT;
    adts_.push_back(xadt);
    return xadt;
}

XTimeUnit * XModel::addTimeUnit() {
    XTimeUnit * xtimeunit = new XTimeUnit;
    timeUnits_.push_back(xtimeunit);
    return xtimeunit;
}

void XModel::addFunctionFile(std::string file) {
    functionFiles_.push_back(file);
}

XMachine * XModel::addAgent() {
    XMachine * xmachine = new XMachine;
    agents_.push_back(xmachine);
    return xmachine;
}

std::vector<XMachine*> * XModel::getAgents() {
    return &agents_;
}

XMessage * XModel::addMessage() {
    XMessage * xmessage = new XMessage;
    messages_.push_back(xmessage);
    return xmessage;
}

/*!
 * \brief Returns a message object with given name
 * \param[in] name Name of the message
 * \return Pointer to the message object or 0 if not found
 * This function is used to validate message names and provide a
 * pointer to the object if valid.
 */
XMessage * XModel::getMessage(std::string name) {
    unsigned int ii;
    for (ii = 0; ii < messages_.size(); ii++)
        if (name == messages_.at(ii)->getName()) return messages_.at(ii);
    return 0;
}

}}  // namespace flame::io
