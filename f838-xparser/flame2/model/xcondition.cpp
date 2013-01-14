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

void printErr(const char *format, ...);

namespace flame { namespace model {

/*!
 * \brief Initialise all condition variables
 *
 * Initialise all condition variables.
 */
XCondition::XCondition()
  : tempValue_(""),
  lhs_(""),
  op_(""),
  rhs_(""),
  isNot_(false),
  isTime_(false),
  timePeriod_(""),
  timePhaseVariable_(""),
  timeDurationString_(""),
  timePhaseValue_(0),
  timePhaseIsVariable_(false),
  timeDuration_(0),
  foundTimeDuration_(false),
  isValues_(false),
  lhsIsValue_(false),
  rhsIsValue_(false),
  lhsIsAgentVariable_(false),
  rhsIsAgentVariable_(false),
  lhsIsMessageVariable_(false),
  rhsIsMessageVariable_(false),
  lhsDouble_(0.0),
  rhsDouble_(0.0),
  isConditions_(false),
  lhsIsCondition_(false),
  rhsIsCondition_(false),
  lhsCondition_(0),
  rhsCondition_(0) {}

/*!
 * \brief Cleans up XCondtion
 *
 * Cleans up XCondtion by deleting any nexted conditions.
 */
XCondition::~XCondition() {
  /* Delete any nested conditions */
  delete lhsCondition_;
  delete rhsCondition_;
}

void XCondition::printValues() {
  /* Handle lhs */
  if (lhsIsAgentVariable_) std::fprintf(stdout, "a.%s", lhs_.c_str());
  else if (lhsIsMessageVariable_) std::fprintf(stdout, "m.%s", lhs_.c_str());
  else if (lhsIsValue_) std::fprintf(stdout, "%f", lhsDouble_);
  std::fprintf(stdout, " ");
  /* Handle operator */
  std::fprintf(stdout, "%s", op_.c_str());
  std::fprintf(stdout, " ");
  /* Handle rhs */
  if (rhsIsAgentVariable_) std::fprintf(stdout, "a.%s", rhs_.c_str());
  else if (rhsIsMessageVariable_) std::fprintf(stdout, "m.%s", rhs_.c_str());
  else if (rhsIsValue_) std::fprintf(stdout, "%f", rhsDouble_);
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

void XCondition::printTime() {
  std::fprintf(stdout, "time(");
  std::fprintf(stdout, "%s", timePeriod_.c_str());
  std::fprintf(stdout, ", ");
  if (timePhaseIsVariable_) std::fprintf(stdout, "a.%s",
      timePhaseVariable_.c_str());
  else
    std::fprintf(stdout, "%d", timePhaseValue_);
  if (foundTimeDuration_) {
    std::fprintf(stdout, ", ");
    std::fprintf(stdout, "%d", timeDuration_);
  }
  std::fprintf(stdout, ")");
}

/*!
 * \brief Print the condition to stdout
 *
 * Print the condition to standard out.
 */
void XCondition::print() {
  std::fprintf(stdout, "\t\t");
  if (isNot_) std::fprintf(stdout, "not(");
  if (isValues_) printValues();
  if (isConditions_) printConditions();
  if (isTime_) printTime();
  if (isNot_) std::fprintf(stdout, ")");
  std::fprintf(stdout, "\n");
}

int XCondition::processSymbolsTime() {
  int errors = 0;
  /* Handle agent phase variable */
  if (boost::starts_with(timePhaseVariable_, "a.")) {
    timePhaseVariable_.erase(0, 2);
    timePhaseIsVariable_ = true;
  } else {
    timePhaseIsVariable_ = false;
    /* Handle phase
           by trying to cast to int */
    try {
      timePhaseValue_ = boost::lexical_cast<int>(timePhaseVariable_);
    } catch(const boost::bad_lexical_cast& E) {
      printErr("Error: Cannot cast time phase to an integer: %s\n",
          timePhaseVariable_.c_str());
      ++errors;
    }
  }
  if (foundTimeDuration_) {
    /* Handle duration number
           by trying to cast to int */
    try {
      timeDuration_ = boost::lexical_cast<int>(timeDurationString_);
    } catch(const boost::bad_lexical_cast& E) {
      printErr("Error: Cannot cast time duration to an integer: %s\n",
          timeDurationString_.c_str());
      ++errors;
    }
  }
  return errors;
}

int XCondition::processSymbolsValue(std::string * hs, bool * hsIsAgentVariable,
    bool * hsIsValue, bool * hsIsMessageVariable, double * hsDouble) {
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

int XCondition::processSymbolsValues() {
  int errors = 0;

  /* Process lhs value */
  errors += processSymbolsValue(&lhs_, &lhsIsAgentVariable_,
      &lhsIsValue_, &lhsIsMessageVariable_, &lhsDouble_);
  /* Process rhs value */
  errors += processSymbolsValue(&rhs_, &rhsIsAgentVariable_,
      &rhsIsValue_, &rhsIsMessageVariable_, &rhsDouble_);
  /* Process operator */
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
    ++errors;
  }

  return errors;
}

int XCondition::processSymbolsConditions() {
  int errors = 0;
  int rc;

  rc = lhsCondition_->processSymbols();
  errors += rc;
  rc = rhsCondition_->processSymbols();
  errors += rc;

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

/*!
 * \brief Processes symbols in conditions/filters
 * \return Number of errors
 * Handles agent/message variables and numbers for values and handles operators.
 */
int XCondition::processSymbols() {
  int errors = 0;

  if (isTime_) {
    errors += processSymbolsTime();
  } else {
    /* Check lhs and rhs are both values or both conditions else error */
    if (lhsIsValue_ && rhsIsValue_) {
      isValues_ = true;
      errors += processSymbolsValues();
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

int XCondition::validateTime(XMachine * agent, XModel * model,
    XCondition * rootCondition) {
  int errors = 0;
  boost::ptr_vector<XTimeUnit>::iterator it;
  /* Check time period is valid time unit */
  bool validPeriod_ = false;
  for (it = model->getTimeUnits()->begin();
      it != model->getTimeUnits()->end(); ++it) {
    if (timePeriod_ == (*it).getName())
      validPeriod_ = true;
  }
  /* Handle invalid time period */
  if (!validPeriod_) {
    printErr("Error: time period is not a valid time unit: %s\n",
        timePeriod_.c_str());
    ++errors;
  }
  /* If time phase is an agent variable then validate it */
  if (timePhaseIsVariable_) {
    /* Handle invalid time phase variable */
    if (!agent->validateVariableName(timePhaseVariable_)) {
      printErr("Error: time phase variable is not a valid agent variable: %s\n",
          timePhaseVariable_.c_str());
      ++errors;
    } else {
      // If agent variable is valid then add to
      // read only variable list
      rootCondition->readOnlyVariables_.insert(
          agent->getVariable(timePhaseVariable_)->getName());
    }
  }
  return errors;
}

int XCondition::validateValue(XMachine * agent, XMessage * xmessage,
    bool * hsIsAgentVariable, std::string * hs,
    bool * hsIsMessageVariable, XCondition * rootCondition) {
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
      rootCondition->readOnlyVariables_.insert(
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
      printErr(
          "Error: cannot validate value as the message type is invalid: %s\n",
          hs->c_str());
      return 1;
    }
  }
  return 0;
}

int XCondition::validateValues(XMachine * agent, XMessage * xmessage,
    XCondition * rootCondition) {
  int errors = 0;
  /* If values validate any agent or message variables */
  errors += validateValue(agent, xmessage, &lhsIsAgentVariable_,
      &lhs_, &lhsIsMessageVariable_, rootCondition);
  errors += validateValue(agent, xmessage, &rhsIsAgentVariable_,
      &rhs_, &rhsIsMessageVariable_, rootCondition);

  return errors;
}

int XCondition::validate(XMachine * agent, XMessage * xmessage,
    XModel * model, XCondition * rootCondition) {
  int rc, errors = 0;

  // Based upon lhs and rhs define condition type
  if (lhsIsCondition_ && rhsIsCondition_) isConditions_ = true;
  else if (lhsIsValue_ && rhsIsValue_) isValues_ = true;

  if (isTime_) {
    // Validate time
    errors += validateTime(agent, model, rootCondition);
  } else if (isValues_) {
    // Validate values
    errors += validateValues(agent, xmessage, rootCondition);
  } else if (isConditions_) {
    // If nested conditions validate them
    rc = lhsCondition_->validate(agent, xmessage, model, rootCondition);
    errors += rc;
    rc = rhsCondition_->validate(agent, xmessage, model, rootCondition);
    errors += rc;
  } else {
    // If not proper type then give error
    printErr("Error: condition is not valid\n");
    ++errors;
  }

  return errors;
}

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

bool XCondition::lhsIsAgentVariable() {
  return lhsIsAgentVariable_;
}

void XCondition::setLhsIsAgentVariable(bool b) {
  lhsIsAgentVariable_ = b;
}

bool XCondition::rhsIsAgentVariable() {
  return rhsIsAgentVariable_;
}

void XCondition::setRhsIsAgentVariable(bool b) {
  rhsIsAgentVariable_ = b;
}

bool XCondition::lhsIsValue() {
  return lhsIsValue_;
}

void XCondition::setLhsIsValue(bool b) {
  lhsIsValue_ = b;
}

bool XCondition::rhsIsValue() {
  return rhsIsValue_;
}

void XCondition::setRhsIsValue(bool b) {
  rhsIsValue_ = b;
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

bool XCondition::lhsIsMessageVariable() {
  return lhsIsMessageVariable_;
}

void XCondition::setLhsIsMessageVariable(bool b) {
  lhsIsMessageVariable_ = b;
}

bool XCondition::rhsIsMessageVariable() {
  return rhsIsMessageVariable_;
}

void XCondition::setRhsIsMessageVariable(bool b) {
  rhsIsMessageVariable_ = b;
}

double XCondition::lhsDouble() {
  return lhsDouble_;
}

void XCondition::setLhsDouble(double d) {
  lhsDouble_ = d;
}

double XCondition::rhsDouble() {
  return rhsDouble_;
}

void XCondition::setRhsDouble(double d) {
  rhsDouble_ = d;
}

std::string XCondition::tempValue() {
  return tempValue_;
}

void XCondition::setTempValue(std::string s) {
  tempValue_ = s;
}

std::string XCondition::lhs() {
  return lhs_;
}

void XCondition::setLhs(std::string s) {
  lhs_ = s;
}

std::string XCondition::op() {
  return op_;
}

void XCondition::setOp(std::string s) {
  op_ = s;
}

std::string XCondition::rhs() {
  return rhs_;
}

void XCondition::setRhs(std::string s) {
  rhs_ = s;
}

std::string XCondition::timePeriod() {
  return timePeriod_;
}

void XCondition::setTimePeriod(std::string s) {
  timePeriod_ = s;
}

std::string XCondition::timePhaseVariable() {
  return timePhaseVariable_;
}

void XCondition::setTimePhaseVariable(std::string s) {
  timePhaseVariable_ = s;
}

std::string XCondition::timeDurationString() {
  return timeDurationString_;
}

void XCondition::setTimeDurationString(std::string s) {
  timeDurationString_ = s;
}

int XCondition::timePhaseValue() {
  return timePhaseValue_;
}

void XCondition::setTimePhaseValue(int i) {
  timePhaseValue_ = i;
}

bool XCondition::timePhaseIsVariable() {
  return timePhaseIsVariable_;
}

void XCondition::setTimePhaseIsVariable(bool b) {
  timePhaseIsVariable_ = b;
}

int XCondition::timeDuration() {
  return timeDuration_;
}

void XCondition::setTimeDuration(int i) {
  timeDuration_ = i;
}

bool XCondition::foundTimeDuration() {
  return foundTimeDuration_;
}

void XCondition::setFoundTimeDuration(bool b) {
  foundTimeDuration_ = b;
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
