/*!
 * \file flame2/model/xdatatype.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XDataType: holds model data type information
 */
#include <cstdio>
#include <string>
#include <vector>
#include "flame2/config.hpp"
#include "xdatatype.hpp"

namespace flame { namespace model {

XDataType::XDataType()
  : id_(0), name_(), variables_(), holdsDynamicArray_(false),
    isFundamental_(false) {}

XDataType::XDataType(std::string name, bool isfundamental)
  : id_(0), name_(name), variables_(), holdsDynamicArray_(false),
    isFundamental_(isfundamental) {}

void XDataType::print() {
  boost::ptr_vector<XVariable>::iterator it;
  std::printf("\tADT Name: %s\n", getName().c_str());
  for (it = variables_.begin(); it != variables_.end(); it++)
    (*it).print();
}

void XDataType::setName(std::string name) {
  name_ = name;
}

std::string XDataType::getName() const {
  return name_;
}

XVariable * XDataType::addVariable() {
  XVariable * xvariable = new XVariable;
  variables_.push_back(xvariable);
  return xvariable;
}

boost::ptr_vector<XVariable> * XDataType::getVariables() {
  return &variables_;
}

void XDataType::setHoldsDynamicArray(bool b) {
  holdsDynamicArray_ = b;
}

bool XDataType::holdsDynamicArray() const {
  return holdsDynamicArray_;
}

void XDataType::setFundamental(bool isfundamental) {
  isFundamental_ = isfundamental;
}

bool XDataType::isFundamental() const {
  return isFundamental_;
}

void XDataType::setID(int id) {
  id_ = id;
}

int XDataType::getID() {
  return id_;
}

}}  // namespace flame::model
