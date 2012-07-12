/*!
 * \file src/model/xcondition.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XCondition: holds condition/filter information
 */
#ifndef MODEL__XCONDITION_HPP_
#define MODEL__XCONDITION_HPP_
#include <string>
#include <vector>

namespace flame { namespace model {

class XMachine;
class XMessage;
class XModel;

class XCondition {
  public:
    XCondition();
    ~XCondition();
    void print();
    int processSymbols();
    int processSymbolsTime();
    int processSymbolsValues();
    int processSymbolsConditions();
    int validate(XMachine * agent, XMessage * message, XModel * model);
    int validateTime(XMachine * agent, XModel * model);
    int validateValues(XMachine * agent, XMessage * xmessage);
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

  private:
    void printValues(std::string lhs, std::string op, std::string rhs,
            bool lhsIsAgentVariable, bool rhsIsAgentVariable,
            bool lhsIsMessageVariable, bool rhsIsMessageVariable,
            bool lhsIsValue, bool rhsIsValue,
            double lhsDouble, double rhsDouble);
    void printConditions(XCondition * lhsCondition,
            std::string op, XCondition * rhsCondition);
    void printTime(std::string timePeriod, std::string timePhaseVariable,
            int timePhaseValue, int timeDuration, bool timePhaseIsVariable,
            bool foundTimeDuration);
    int processSymbolsValue(std::string * hs, bool * hsIsAgentVariable,
            bool * hsIsValue, bool * hsIsMessageVariable, double * hsDouble);
    int validateValue(XMachine * agent, XMessage * xmessage,
            bool * hsIsAgentVariable,
            std::string * hs, bool * hsIsMessageVariable);
};
}}  // namespace flame::model
#endif  // MODEL__XCONDITION_HPP_
