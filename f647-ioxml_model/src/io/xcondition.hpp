/*!
 * \file src/io/xcondition.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XCondition: holds condition/filter information
 */
#ifndef IO__XCONDITION_HPP_
#define IO__XCONDITION_HPP_
#include <string>
#include <vector>

namespace flame { namespace io {

class XMachine;
class XMessage;

class XCondition {
  public:
    XCondition();
    ~XCondition();
    void print();
    int processSymbols();
    int validate(XMachine * agent, XMessage * message = 0);

  // private:
    bool isNot;
    bool isTime;
    bool isValues;
    bool isConditions;
    bool lhsIsAgentVariable;
    bool rhsIsAgentVariable;
    bool lhsIsValue;
    bool rhsIsValue;
    bool lhsIsCondition;
    bool rhsIsCondition;
    bool lhsIsMessageVariable;
    bool rhsIsMessageVariable;
    double lhsDouble;
    double rhsDouble;
    /*! \brief tempValue holds a value before being set as lhs or rhs */
    std::string tempValue;
    std::string lhs;
    std::string op;
    std::string rhs;
    std::string timePeriod;
    std::string timePhaseVariable;
    std::string timeDurationString;
    int timePhaseValue;
    bool timePhaseIsVariable;
    int timeDuration;
    bool foundTimeDuration;
    XCondition * lhsCondition;
    XCondition * rhsCondition;
};
}}  // namespace flame::io
#endif  // IO__XCONDITION_HPP_
