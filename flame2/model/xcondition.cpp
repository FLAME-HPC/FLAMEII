/*!
 * \file flame2/model/xcondition.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XCondition: holds condition/filter information
 */
#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>
#include <cstdio>
#include <string>
#include <set>
#include "flame2/config.hpp"
#include "xcondition.hpp"
#include "xmachine.hpp"
#include "xmessage.hpp"
#include "xmodel.hpp"
#include "printerr.hpp"

namespace flame { namespace model {

XCondition::XCondition()
  : values(), time(), op_(""),
  isNot_(false),
  isTime_(false),
  isValues_(false),
  isConditions_(false),
  lhsIsCondition_(false),
  rhsIsCondition_(false),
  lhsCondition_(0),
  rhsCondition_(0), readOnlyVariables_() {}

XCondition::~XCondition() {
  /* Delete any nested conditions */
  delete lhsCondition_;
  delete rhsCondition_;
}

void XCondition::printValues() {
  /* Handle lhs */
  values.printLHSValue();
  std::fprintf(stdout, " ");
  /* Handle operator */
  std::fprintf(stdout, "%s", op_.c_str());
  std::fprintf(stdout, " ");
  /* Handle rhs */
  values.printRHSValue();
}

void XCondition::printConditions() {
  std::fprintf(stdout, "(");
  std::fprintf(stdout, "\n");
  lhsCondition_->print();
  std::fprintf(stdout, "\t\t");
  std::fprintf(stdout, ") ");
  std::fprintf(stdout, "%s", op_.c_str());
  std::fprintf(stdout, " (");
  std::fprintf(stdout, "\n");
  rhsCondition_->print();
  std::fprintf(stdout, "\t\t");
  std::fprintf(stdout, ")");
}

void XCondition::print() {
  std::fprintf(stdout, "\t\t");
  if (isNot_) std::fprintf(stdout, "not(");
  if (isValues_) printValues();
  if (isConditions_) printConditions();
  if (isTime_) time.printTime();
  if (isNot_) std::fprintf(stdout, ")");
  std::fprintf(stdout, "\n");
}

int XCondition::processSymbolsOp() {
  if (op_ == "EQ") {
    op_ = "==";
  } else if (op_ == "NEQ") {
    op_ = "!=";
  } else if (op_ == "LEQ") {
    op_ = "<=";
  } else if (op_ == "GEQ") {
    op_ = ">=";
  } else if (op_ == "LT") {
    op_ = "<";
  } else if (op_ == "GT") {
    op_ = ">";
  } else {
    /* Handle unknown operator */
    printErr("Error: Condition/filter op value not recognised: %s\n",
        op_.c_str());
    return 1;
  }

  return 0;
}

int XCondition::processSymbolsConditions() {
  int errors = 0;
  int rc;

  // process lhs condition
  rc = lhsCondition_->processSymbols();
  errors += rc;
  // process rhs condition
  rc = rhsCondition_->processSymbols();
  errors += rc;

  // process logic operator
  if (op_ == "AND") {
    op_ = "&&";
  } else if (op_ == "OR") {
    op_ = "||";
  } else {
    printErr("Error: Condition/filter op value not recognised: %s\n",
        op_.c_str());
    ++errors;
  }

  return errors;
}

int XCondition::processSymbols() {
  int errors = 0;

  if (isTime_) {
    errors += time.processSymbolsTime();
  } else {
    // Check lhs and rhs are both values or
    // both conditions else error
    if (values.isLHSandRHSvalues()) {
      isValues_ = true;
      errors += values.processSymbolsValues();
    } else if (lhsIsCondition_ && rhsIsCondition_) {
      isConditions_ = true;
      errors += processSymbolsConditions();
    } else {
      printErr(
        "Error: lhs and rhs are not both values or both nested conditions\n");
      ++errors;
    }
  }

  return errors;
}

int XCondition::validateTime(XMachine * agent,
    boost::ptr_vector<XTimeUnit>& timeUnits,
    XCondition * rootCondition) {
  int errors = 0;
  boost::ptr_vector<XTimeUnit>::iterator it;
  /* Check time period is valid time unit */
  bool validPeriod_ = false;
  for (it = timeUnits.begin(); it != timeUnits.end(); ++it) {
    if (time.timePeriod() == (*it).getName()) validPeriod_ = true;
  }
  /* Handle invalid time period */
  if (!validPeriod_) {
    printErr("Error: time period is not a valid time unit: %s\n",
        time.timePeriod().c_str());
    ++errors;
  }
  /* If time phase is an agent variable then validate it */
  if (time.timePhaseIsVariable()) {
    /* Handle invalid time phase variable */
    if (!agent->validateVariableName(time.timePhaseVariable())) {
      printErr("Error: time phase variable is not a valid agent variable: %s\n",
          time.timePhaseVariable().c_str());
      ++errors;
    } else {
      // If agent variable is valid then add to
      // read only variable list
      rootCondition->readOnlyVariables_.insert(
          agent->getVariable(time.timePhaseVariable())->getName());
    }
  }
  return errors;
}

int XCondition::validate(XMachine * agent, XMessage * xmessage,
    boost::ptr_vector<XTimeUnit>& timeUnits,
    XCondition * rootCondition) {
  int rc, errors = 0;

  // Based upon lhs and rhs define condition type
  if (lhsIsCondition_ && rhsIsCondition_) isConditions_ = true;
  else if (values.isLHSandRHSvalues()) isValues_ = true;

  if (isTime_) {
    // Validate time
    errors += validateTime(agent, timeUnits, rootCondition);
  } else if (isValues_) {
    // Validate values
    errors += values.validateValues(agent, xmessage,
        rootCondition->getReadOnlyVariables());
  } else if (isConditions_) {
    // validate lhs condition
    rc = lhsCondition_->validate(agent, xmessage, timeUnits, rootCondition);
    errors += rc;
    // validate rhs condition
    rc = rhsCondition_->validate(agent, xmessage, timeUnits, rootCondition);
    errors += rc;
  } else {
    // If not proper type then give error
    printErr("Error: condition is not valid\n");
    ++errors;
  }

  return errors;
}

// the following
// methods are
// get and set
// methods for
// private
// variables
// of the
// condition
// class

std::set<std::string> * XCondition::getReadOnlyVariables() {
  return &readOnlyVariables_;
}

void XCondition::addReadOnlyVariable(std::string s) {
  readOnlyVariables_.insert(s);
}

bool XCondition::isNot() {
  return isNot_;
}

void XCondition::setIsNot(bool b) {
  isNot_ = b;
}

bool XCondition::isTime() {
  return isTime_;
}

void XCondition::setIsTime(bool b) {
  isTime_ = b;
}

bool XCondition::isValues() {
  return isValues_;
}

void XCondition::setIsValues(bool b) {
  isValues_ = b;
}

bool XCondition::isConditions() {
  return isConditions_;
}

void XCondition::setIsConditions(bool b) {
  isConditions_ = b;
}

bool XCondition::lhsIsCondition() {
  return lhsIsCondition_;
}

void XCondition::setLhsIsCondition(bool b) {
  lhsIsCondition_ = b;
}

bool XCondition::rhsIsCondition() {
  return rhsIsCondition_;
}

void XCondition::setRhsIsCondition(bool b) {
  rhsIsCondition_ = b;
}

std::string XCondition::op() {
  return op_;
}

void XCondition::setOp(std::string s) {
  op_ = s;
}

XCondition * XCondition::lhsCondition() {
  return lhsCondition_;
}

void XCondition::setLhsCondition(XCondition * c) {
  lhsCondition_ = c;
}

XCondition * XCondition::rhsCondition() {
  return rhsCondition_;
}

void XCondition::setRhsCondition(XCondition * c) {
  rhsCondition_ = c;
}

}}  // namespace flame::model
