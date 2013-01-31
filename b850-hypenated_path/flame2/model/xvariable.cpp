/*!
 * \file flame2/model/xvariable.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XVariable: holds agent variable information
 */
#include <string>
#include <cstdio>
#include "flame2/config.hpp"
#include "xvariable.hpp"

namespace flame { namespace model {

/*!
 * \brief Initialises XVariable
 *
 * Initialises XVariable as
 * fundamental (int or double)
 * variable (not constant)
 * scaler (not an array).
 */
XVariable::XVariable()
: isDynamicArray_(false),
  isStaticArray_(false),
  staticArraySize_(0),
  hasADTType_(false),
  holdsDynamicArray_(false),
  constantSet_(false),
  constant_(false) {}

/*!
 * \brief Initialises XVariable
 *
 * Initialises XVariable using a name.
 */
XVariable::XVariable(std::string name)
: name_(name),
  isDynamicArray_(false),
  isStaticArray_(false),
  staticArraySize_(0),
  hasADTType_(false),
  holdsDynamicArray_(false),
  constantSet_(false),
  constant_(false) {}

/*!
 * \brief Prints XVariable
 *
 * Prints XVariable to
 * standard out.
 */
void XVariable::print() {
  std::fprintf(stdout, "\t%s %s\n",
      getType().c_str(), getName().c_str());
}

/* All other functions are for setting and getting
 * class variables */

void XVariable::setName(std::string name) {
  name_ = name;
}

std::string XVariable::getName() {
  return name_;
}

void XVariable::setType(std::string type) {
  type_ = type;
}

std::string XVariable::getType() {
  return type_;
}

void XVariable::setIsDynamicArray(bool b) {
  isDynamicArray_ = b;
}

bool XVariable::isDynamicArray() {
  return isDynamicArray_;
}

void XVariable::setIsStaticArray(bool b) {
  isStaticArray_ = b;
}

bool XVariable::isStaticArray() {
  return isStaticArray_;
}

void XVariable::setStaticArraySize(int size) {
  staticArraySize_ = size;
}

int XVariable::getStaticArraySize() {
  return staticArraySize_;
}

void XVariable::setHasADTType(bool b) {
  hasADTType_ = b;
}

bool XVariable::hasADTType() {
  return hasADTType_;
}

void XVariable::setHoldsDynamicArray(bool b) {
  holdsDynamicArray_ = b;
}

bool XVariable::holdsDynamicArray() {
  return holdsDynamicArray_;
}

void XVariable::setConstantString(std::string name) {
  constantString_ = name;
}

std::string XVariable::getConstantString() {
  return constantString_;
}

void XVariable::setConstantSet(bool set) {
  constantSet_ = set;
}

bool XVariable::isConstantSet() {
  return constantSet_;
}

void XVariable::setConstant(bool constant) {
  constant_ = constant;
}

bool XVariable::isConstant() {
  return constant_;
}

}}  // namespace flame::model
