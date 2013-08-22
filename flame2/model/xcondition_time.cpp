/*!
 * \file flame2/model/xcondition_time.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XConditionTime: holds condition time information
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

XConditionTime::XConditionTime()
  : timePeriod_(""),
  timePhaseVariable_(""),
  timeDurationString_(""),
  timePhaseValue_(0),
  timePhaseIsVariable_(false),
  timeDuration_(0),
  foundTimeDuration_(false) {}

void XConditionTime::printTime() {
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

int XConditionTime::processSymbolsTime() {
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

std::string XConditionTime::timePeriod() {
  return timePeriod_;
}

void XConditionTime::setTimePeriod(std::string s) {
  timePeriod_ = s;
}

std::string XConditionTime::timePhaseVariable() {
  return timePhaseVariable_;
}

void XConditionTime::setTimePhaseVariable(std::string s) {
  timePhaseVariable_ = s;
}

std::string XConditionTime::timeDurationString() {
  return timeDurationString_;
}

void XConditionTime::setTimeDurationString(std::string s) {
  timeDurationString_ = s;
}

int XConditionTime::timePhaseValue() {
  return timePhaseValue_;
}

void XConditionTime::setTimePhaseValue(int i) {
  timePhaseValue_ = i;
}

bool XConditionTime::timePhaseIsVariable() {
  return timePhaseIsVariable_;
}

void XConditionTime::setTimePhaseIsVariable(bool b) {
  timePhaseIsVariable_ = b;
}

int XConditionTime::timeDuration() {
  return timeDuration_;
}

void XConditionTime::setTimeDuration(int i) {
  timeDuration_ = i;
}

bool XConditionTime::foundTimeDuration() {
  return foundTimeDuration_;
}

void XConditionTime::setFoundTimeDuration(bool b) {
  foundTimeDuration_ = b;
}

}}  // namespace flame::model
