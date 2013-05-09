/*!
 * \file flame2/model/xadt.cpp
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
#include "flame2/config.hpp"
#include "xadt.hpp"

namespace flame { namespace model {

XADT::XADT() : holdsDynamicArray_(false) {}

void XADT::print() {
  boost::ptr_vector<XVariable>::iterator it;
  std::printf("\tADT Name: %s\n", getName().c_str());
  for (it = variables_.begin(); it != variables_.end(); it++)
    (*it).print();
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

boost::ptr_vector<XVariable> * XADT::getVariables() {
  return &variables_;
}

void XADT::setHoldsDynamicArray(bool b) {
  holdsDynamicArray_ = b;
}

bool XADT::holdsDynamicArray() {
  return holdsDynamicArray_;
}

}}  // namespace flame::model
