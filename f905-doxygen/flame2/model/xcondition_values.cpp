/*!
 * \file flame2/model/xcondition_values.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XConditionValues: holds condition values
 */
#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>
#include <cstdio>
#include <string>
#include <set>
#include "flame2/config.hpp"
#include "xcondition_values.hpp"
#include "xmachine.hpp"
#include "xmessage.hpp"
#include "xmodel.hpp"
#include "printerr.hpp"

namespace flame { namespace model {

XConditionValues::XConditionValues()
  : tempValue_(""),
  lhs_(""),
  rhs_(""),
  lhsIsValue_(false),
  rhsIsValue_(false),
  lhsIsAgentVariable_(false),
  rhsIsAgentVariable_(false),
  lhsIsMessageVariable_(false),
  rhsIsMessageVariable_(false),
  lhsDouble_(0.0),
  rhsDouble_(0.0) {}

void XConditionValues::printLHSValue() {
  if (lhsIsAgentVariable_) std::fprintf(stdout, "a.%s", lhs_.c_str());
  else if (lhsIsMessageVariable_) std::fprintf(stdout, "m.%s", lhs_.c_str());
  else if (lhsIsValue_) std::fprintf(stdout, "%f", lhsDouble_);
}

void XConditionValues::printRHSValue() {
  if (rhsIsAgentVariable_) std::fprintf(stdout, "a.%s", rhs_.c_str());
  else if (rhsIsMessageVariable_) std::fprintf(stdout, "m.%s", rhs_.c_str());
  else if (rhsIsValue_) std::fprintf(stdout, "%f", rhsDouble_);
}

int XConditionValues::processSymbolsValue(std::string * hs,
    bool * hsIsAgentVariable, bool * hsIsValue, bool * hsIsMessageVariable,
    double * hsDouble) {
  int errors = 0;
  /* Handle agent variable */
  if (boost::starts_with(*hs, "a.")) {
    *hsIsAgentVariable = true;
    *hsIsValue = false;
    hs->erase(0, 2);
    /* Handle message variable */
  } else if (boost::starts_with(*hs, "m.")) {
    *hsIsMessageVariable = true;
    *hsIsValue = false;
    hs->erase(0, 2);
    /* Handle number */
  } else {
    *hsIsValue = true;
    /* Try and cast to a double */
    try {
      *hsDouble = boost::lexical_cast<double>(*hs);
    } catch(const boost::bad_lexical_cast& E) {
      printErr("Error: Condition/filter value not variable or number: %s\n",
          hs->c_str());
      ++errors;
    }
  }
  return errors;
}

int XConditionValues::processSymbolsValues() {
  int errors = 0;

  /* Process lhs value */
  errors += processSymbolsValue(&lhs_, &lhsIsAgentVariable_,
      &lhsIsValue_, &lhsIsMessageVariable_, &lhsDouble_);
  /* Process rhs value */
  errors += processSymbolsValue(&rhs_, &rhsIsAgentVariable_,
      &rhsIsValue_, &rhsIsMessageVariable_, &rhsDouble_);

  return errors;
}

int XConditionValues::validateValue(XMachine * agent, XMessage * xmessage,
    bool * hsIsAgentVariable, std::string * hs,
    bool * hsIsMessageVariable, std::set<std::string> * readOnlyVariables) {
  /* Handle agent variable */
  if (*hsIsAgentVariable) {
    /* Try and validate */
    if (!agent->validateVariableName(*hs)) {
      printErr("Error: value is not a valid agent variable: %s\n",
          hs->c_str());
      return 1;
    } else {
      // If agent variable is valid then add to
      // read only variable list
      readOnlyVariables->insert(
          agent->getVariable(*hs)->getName());
    }
    /* Handle message variable */
  } else if (*hsIsMessageVariable) {
    /* If message type exists */
    if (xmessage != 0) {
      /* Try and validate */
      if (!xmessage->validateVariableName(*hs)) {
        printErr("Error: value is not a valid message variable: %s\n",
            hs->c_str());
        return 1;
      }
    } else {
      printErr("Error: cannot validate value %s: %s\n",
          "as the message type is invalid", hs->c_str());
      return 1;
    }
  }
  return 0;
}

int XConditionValues::validateValues(XMachine * agent, XMessage * xmessage,
    std::set<std::string> * readOnlyVariables) {
  int errors = 0;
  /* If values validate any agent or message variables */
  errors += validateValue(agent, xmessage, &lhsIsAgentVariable_,
      &lhs_, &lhsIsMessageVariable_, readOnlyVariables);
  errors += validateValue(agent, xmessage, &rhsIsAgentVariable_,
      &rhs_, &rhsIsMessageVariable_, readOnlyVariables);

  return errors;
}

bool XConditionValues::isLHSandRHSvalues() {
  if (lhsIsValue_ && rhsIsValue_) return true;
  return false;
}


bool XConditionValues::lhsIsAgentVariable() {
  return lhsIsAgentVariable_;
}

void XConditionValues::setLhsIsAgentVariable(bool b) {
  lhsIsAgentVariable_ = b;
}

bool XConditionValues::rhsIsAgentVariable() {
  return rhsIsAgentVariable_;
}

void XConditionValues::setRhsIsAgentVariable(bool b) {
  rhsIsAgentVariable_ = b;
}

bool XConditionValues::lhsIsValue() {
  return lhsIsValue_;
}

void XConditionValues::setLhsIsValue(bool b) {
  lhsIsValue_ = b;
}

bool XConditionValues::rhsIsValue() {
  return rhsIsValue_;
}

void XConditionValues::setRhsIsValue(bool b) {
  rhsIsValue_ = b;
}

bool XConditionValues::lhsIsMessageVariable() {
  return lhsIsMessageVariable_;
}

void XConditionValues::setLhsIsMessageVariable(bool b) {
  lhsIsMessageVariable_ = b;
}

bool XConditionValues::rhsIsMessageVariable() {
  return rhsIsMessageVariable_;
}

void XConditionValues::setRhsIsMessageVariable(bool b) {
  rhsIsMessageVariable_ = b;
}

double XConditionValues::lhsDouble() {
  return lhsDouble_;
}

void XConditionValues::setLhsDouble(double d) {
  lhsDouble_ = d;
}

double XConditionValues::rhsDouble() {
  return rhsDouble_;
}

void XConditionValues::setRhsDouble(double d) {
  rhsDouble_ = d;
}

std::string XConditionValues::tempValue() {
  return tempValue_;
}

void XConditionValues::setTempValue(std::string s) {
  tempValue_ = s;
}

std::string XConditionValues::lhs() {
  return lhs_;
}

void XConditionValues::setLhs(std::string s) {
  lhs_ = s;
}


std::string XConditionValues::rhs() {
  return rhs_;
}

void XConditionValues::setRhs(std::string s) {
  rhs_ = s;
}

}}  // namespace flame::model
