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

XMessage::XMessage()
  : id_(0) {}

void XMessage::print() {
  boost::ptr_vector<XVariable>::iterator it;
  std::fprintf(stdout, "\tMessage Name: %s\n", getName().c_str());
  for (it = variables_.begin(); it != variables_.end(); it++)
    (*it).print();
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

XVariable * XMessage::addVariable(std::string type, std::string name) {
  XVariable * xvariable = addVariable();
  xvariable->setType(type);
  xvariable->setName(name);
  return xvariable;
}

boost::ptr_vector<XVariable> * XMessage::getVariables() {
  return &variables_;
}

bool XMessage::validateVariableName(std::string name) {
  boost::ptr_vector<XVariable>::iterator it;
  for (it = variables_.begin(); it != variables_.end(); it++)
    if (name == (*it).getName()) return true;
  return false;
}

void XMessage::setID(int id) {
  id_ = id;
}

int XMessage::getID() {
  return id_;
}

}}  // namespace flame::model
