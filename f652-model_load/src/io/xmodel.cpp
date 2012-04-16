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

XModel::XModel() {
    /* Initialise list of data types */
    addAllowedDataType("int");
    addAllowedDataType("float");
    addAllowedDataType("double");
    addAllowedDataType("char"); /* Allow? */
}

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

void XModel::setPath(std::string path) {
    path_ = path;
}

std::string XModel::getPath() {
    return path_;
}

void XModel::setName(std::string name) {
    name_ = name;
}

std::string XModel::getName() {
    return name_;
}

/*!
 * \brief Adds included model to a list
 * \param[in] path Path of the sub model
 * \return Boolean, true if name is unique
 * If the model file is not already in the list then it is added.
 * If not then false is returned.
 */
bool XModel::addIncludedModel(std::string path) {
    unsigned int ii;

    for (ii = 0; ii < includedModels_.size(); ii++) {
        if (includedModels_.at(ii) == path) return false;
    }

    includedModels_.push_back(path);
    return true;
}

std::vector<std::string> * XModel::getIncludedModels() {
    return &includedModels_;
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

/*!
 * \brief Returns an adt object with given name
 * \param[in] name Name of the adt
 * \return Pointer to the adt object or 0 if not found
 * This function is used to validate adt names and provide a
 * pointer to the object if valid.
 */
XADT * XModel::getADT(std::string name) {
    unsigned int ii;
    for (ii = 0; ii < adts_.size(); ii++)
        if (name == adts_.at(ii)->getName()) return adts_.at(ii);
    return 0;
}

std::vector<XADT*> * XModel::getADTs() {
    return &adts_;
}

XTimeUnit * XModel::addTimeUnit() {
    XTimeUnit * xtimeunit = new XTimeUnit;
    timeUnits_.push_back(xtimeunit);
    return xtimeunit;
}

std::vector<XTimeUnit*> * XModel::getTimeUnits() {
    return &timeUnits_;
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

XMachine * XModel::getAgent(std::string name) {
    unsigned int ii;
    for (ii = 0; ii < agents_.size(); ii++)
        if (name == agents_.at(ii)->getName()) return agents_.at(ii);
    return 0;
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

std::vector<XMessage*> * XModel::getMessages() {
    return &messages_;
}

void XModel::addAllowedDataType(std::string name) {
    allowedDataTypes_.push_back(name);
}

std::vector<std::string> * XModel::getAllowedDataTypes() {
    return &allowedDataTypes_;
}

}}  // namespace flame::io
