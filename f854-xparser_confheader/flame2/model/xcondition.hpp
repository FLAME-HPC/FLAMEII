/*!
 * \file flame2/model/xcondition.hpp
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
#include <set>

namespace flame { namespace model {

class XVariable;
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
    int validate(XMachine * agent, XMessage * message, XModel * model,
        XCondition * rootCondition);
    int validateTime(XMachine * agent, XModel * model,
        XCondition * rootCondition);
    int validateValues(XMachine * agent, XMessage * xmessage,
        XCondition * rootCondition);
    std::set<std::string> * getReadOnlyVariables();
    void addReadOnlyVariable(std::string s);
    bool isNot();
    void setIsNot(bool b);
    bool isTime();
    void setIsTime(bool b);
    bool isValues();
    void setIsValues(bool b);
    bool isConditions();
    void setIsConditions(bool b);
    bool lhsIsAgentVariable();
    void setLhsIsAgentVariable(bool b);
    bool rhsIsAgentVariable();
    void setRhsIsAgentVariable(bool b);
    bool lhsIsValue();
    void setLhsIsValue(bool b);
    bool rhsIsValue();
    void setRhsIsValue(bool b);
    bool lhsIsCondition();
    void setLhsIsCondition(bool b);
    bool rhsIsCondition();
    void setRhsIsCondition(bool b);
    bool lhsIsMessageVariable();
    void setLhsIsMessageVariable(bool b);
    bool rhsIsMessageVariable();
    void setRhsIsMessageVariable(bool b);
    double lhsDouble();
    void setLhsDouble(double d);
    double rhsDouble();
    void setRhsDouble(double d);
    /*! \brief tempValue holds a value before being set as lhs or rhs */
    std::string tempValue();
    void setTempValue(std::string s);
    std::string lhs();
    void setLhs(std::string s);
    std::string op();
    void setOp(std::string s);
    std::string rhs();
    void setRhs(std::string s);
    std::string timePeriod();
    void setTimePeriod(std::string s);
    std::string timePhaseVariable();
    void setTimePhaseVariable(std::string s);
    std::string timeDurationString();
    void setTimeDurationString(std::string s);
    int timePhaseValue();
    void setTimePhaseValue(int i);
    bool timePhaseIsVariable();
    void setTimePhaseIsVariable(bool b);
    int timeDuration();
    void setTimeDuration(int i);
    bool foundTimeDuration();
    void setFoundTimeDuration(bool b);
    XCondition * lhsCondition();
    void setLhsCondition(XCondition * c);
    XCondition * rhsCondition();
    void setRhsCondition(XCondition * c);

  private:
    /* read in values */
    /*! \brief tempValue holds a value before being set as lhs or rhs */
    std::string tempValue_;
    std::string lhs_;
    std::string op_;
    std::string rhs_;
    /* condition is negated */
    bool isNot_;
    /* condition is time based */
    bool isTime_;
    std::string timePeriod_;
    std::string timePhaseVariable_;
    std::string timeDurationString_;
    int timePhaseValue_;
    bool timePhaseIsVariable_;
    int timeDuration_;
    bool foundTimeDuration_;
    /* condition uses values */
    bool isValues_;
    bool lhsIsValue_;
    bool rhsIsValue_;
    bool lhsIsAgentVariable_;
    bool rhsIsAgentVariable_;
    bool lhsIsMessageVariable_;
    bool rhsIsMessageVariable_;
    double lhsDouble_;
    double rhsDouble_;
    /* condition is a nested condition */
    bool isConditions_;
    bool lhsIsCondition_;
    bool rhsIsCondition_;
    XCondition * lhsCondition_;
    XCondition * rhsCondition_;
    /*! \brief Agent variables that the condition reads */
    std::set<std::string> readOnlyVariables_;

    void printValues();
    void printConditions();
    void printTime();
    int processSymbolsValue(std::string * hs, bool * hsIsAgentVariable,
        bool * hsIsValue, bool * hsIsMessageVariable, double * hsDouble);
    int validateValue(XMachine * agent, XMessage * xmessage,
        bool * hsIsAgentVariable, std::string * hs,
        bool * hsIsMessageVariable, XCondition * rootCondition);
};
}}  // namespace flame::model
#endif  // MODEL__XCONDITION_HPP_
