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

/*!
 * \brief Class to hold condition information
 */
class XCondition {
  public:
    /*!
     * \brief Initialise all condition variables
     *
     * Initialise all condition variables.
     */
    XCondition();
    /*!
     * \brief Cleans up XCondtion
     *
     * Cleans up XCondtion by deleting any nested conditions.
     */
    ~XCondition();
    /*!
     * \brief Print the condition to stdout
     *
     * Print the condition to standard out.
     */
    void print();
    /*!
     * \brief Process symbols in conditions/filters
     * \return Number of errors
     * Handles agent/message variables and numbers for values and handles operators.
     */
    int processSymbols();
    /*!
     * \brief Process symbols in time conditions
     * \return Number of errors
     */
    int processSymbolsTime();
    /*!
     * \brief Process symbol values
     * \return Number of errors
     */
    int processSymbolsValues();
    /*!
     * \brief Process symbol in conditions
     * \return Number of errors
     */
    int processSymbolsConditions();
    /*!
     * \brief Validate the condition
     * \param[in] agent The agent
     * \param[in] message The message
     * \param[in] model The model
     * \param[in] rootCondition The condition
     * \return Number of errors
     */
    int validate(XMachine * agent, XMessage * message, XModel * model,
        XCondition * rootCondition);
    /*!
     * \brief Validate a time condition
     * \param[in] agent The agent
     * \param[in] model The model
     * \param[in] rootCondition The condition
     * \return Number of errors
     */
    int validateTime(XMachine * agent, XModel * model,
        XCondition * rootCondition);
    /*!
     * \brief Validate values
     * \param[in] agent The agent
     * \param[in] xmessage The message
     * \param[in] rootCondition The condition
     * \return Number of errors
     */
    int validateValues(XMachine * agent, XMessage * xmessage,
        XCondition * rootCondition);
    /*!
     * \brief Get read only variables used by the condition
     * \return Set of variable names
     */
    std::set<std::string> * getReadOnlyVariables();
    /*!
     * \brief Add read only variable
     * \param[in] name The variable name
     */
    void addReadOnlyVariable(std::string name);
    /*!
     * \brief Is condition negated
     * \return Result
     */
    bool isNot();
    /*!
     * \brief Set condition to be negated
     * \param[in] b Boolean value
     */
    void setIsNot(bool b);
    /*!
     * \brief Is condition a time condition
     * \return Boolean result
     */
    bool isTime();
    /*!
     * \brief Set conditon as a time condition
     * \param[in] b Boolean value
     */
    void setIsTime(bool b);
    /*!
     * \brief Does condition use values
     * \return Boolean result
     */
    bool isValues();
    /*!
     * \brief Set condition as using values
     * \param[in] b Boolean value
     */
    void setIsValues(bool b);
    /*!
     * \brief Is condition make of sub conditions
     * \return Boolean result
     */
    bool isConditions();
    /*!
     * \brief Set condition as using sub conditions
     * \param[in] b Boolean value
     */
    void setIsConditions(bool b);
    /*!
     * \brief Is lhs an agent variable
     * \return Boolean result
     */
    bool lhsIsAgentVariable();
    /*!
     * \brief Set lhs as an agent variable
     * \param[in] b Boolean value
     */
    void setLhsIsAgentVariable(bool b);
    /*!
     * \brief Is rhs an agent variable
     * \return Boolean result
     */
    bool rhsIsAgentVariable();
    /*!
     * \brief Set rhs as an agent variable
     * \param[in] b Boolean value
     */
    void setRhsIsAgentVariable(bool b);
    /*!
     * \brief Is lhs a value
     * \return Boolean value
     */
    bool lhsIsValue();
    /*!
     * \brief Set lhs as a value
     * \param[in] b Boolean value
     */
    void setLhsIsValue(bool b);
    /*!
     * \brief Is rhs a value
     * \result Boolean result
     */
    bool rhsIsValue();
    /*!
     * \brief Set rhs as a value
     * \param[in] b Boolean value
     */
    void setRhsIsValue(bool b);
    /*!
     * \brief Is lhs a sub condition
     * \return Boolean result
     */
    bool lhsIsCondition();
    /*!
     * \brief Set lhs as a sub condition
     * \param[in] b Boolean value
     */
    void setLhsIsCondition(bool b);
    /*!
     * \brief Is rhs a sub condition
     * \return Boolen value
     */
    bool rhsIsCondition();
    /*!
     * \brief Set rhs as a sub condition
     * \param[in] b Boolean value
     */
    void setRhsIsCondition(bool b);
    /*!
     * \brief Is lhs a message variable
     * \return Boolean result
     */
    bool lhsIsMessageVariable();
    /*!
     * \brief Set lhs as a message variable
     * \param[in] b Boolean value
     */
    void setLhsIsMessageVariable(bool b);
    /*!
     * \brief Is rhs a message variable
     * * \return Boolean result
     */
    bool rhsIsMessageVariable();
    /*!
     * \brief Set rhs as a message variable
     * \param[in] b Boolean value
     */
    void setRhsIsMessageVariable(bool b);
    /*!
     * \brief Get lhs double value
     * \return Lhs double value
     */
    double lhsDouble();
    /*!
     * \brief Set lhs double value
     * \param[in] d Double value
     */
    void setLhsDouble(double d);
    /*!
     * \brief Get rhs double value
     * \return Rhs double value
     */
    double rhsDouble();
    /*!
     * \brief Set rhs double value
     * \param[in] d Double value
     */
    void setRhsDouble(double d);
    /*!
     * \brief Get temp value
     * \return Temp value
     */
    std::string tempValue();
    /*!
     * \brief Set temp value
     * \param[in] s Temp value
     */
    void setTempValue(std::string s);
    /*!
     * \brief Get lhs string
     * \return Lhs string
     */
    std::string lhs();
    /*!
     * \brief Set lhs string
     * \param[in] s Lhs String
     */
    void setLhs(std::string s);
    /*!
     * \brief Get operator string
     * \return Operator string
     */
    std::string op();
    /*!
     * \brief Set operator string
     * \param[in] s Operator string
     */
    void setOp(std::string s);
    /*!
     * \brief Get rhs string
     * \return Rhs string
     */
    std::string rhs();
    /*!
     * \brief Set rhs string
     * \param[in] s Rhs string
     */
    void setRhs(std::string s);
    /*!
     * \brief Get time period string
     * \return Time period string
     */
    std::string timePeriod();
    /*!
     * \brief Set time period string
     * \param[in] s Time period string
     */
    void setTimePeriod(std::string s);
    /*!
     * \brief Get time phase variable
     * \return Time phase variable
     */
    std::string timePhaseVariable();
    /*!
     * \brief Set time phase variable
     * \param[in] s Time phase variable
     */
    void setTimePhaseVariable(std::string s);
    /*!
     * \brief Get time duration string
     * \return Time duration string
     */
    std::string timeDurationString();
    /*!
     * \brief Set time duration string
     * \param[in] s Time duration string
     */
    void setTimeDurationString(std::string s);
    /*!
     * \brief Get time phase value
     * \return Time phase value
     */
    int timePhaseValue();
    /*!
     * \brief Set time phase value
     * \param[in] i Time phase value
     */
    void setTimePhaseValue(int i);
    /*!
     * \brief If time phase is a variable
     * * \return Boolean result
     */
    bool timePhaseIsVariable();
    /*!
     * \brief Set if time phase is a variable
     * \param[in] b Boolean value
     */
    void setTimePhaseIsVariable(bool b);
    /*!
     * \brief Get time duration
     * \return Time duration
     */
    int timeDuration();
    /*!
     * \brief Set time duration
     * \param[in] i Time duration
     */
    void setTimeDuration(int i);
    /*!
     * \brief If found time duration
     * * \return Boolean result
     */
    bool foundTimeDuration();
    /*!
     * \brief Set if found time duration
     * \param[in] b Boolean value
     */
    void setFoundTimeDuration(bool b);
    /*!
     * \brief Get lhs sub condition
     * \return Lhs condition
     */
    XCondition * lhsCondition();
    /*!
     * \brief Set lhs sub condition
     * \param[in] c Lhs condition
     */
    void setLhsCondition(XCondition * c);
    /*!
     * \brief Get rhs sub condition
     * \return Lhs condition
     */
    XCondition * rhsCondition();
    /*!
     * \brief Set rhs sub condition
     * \param[in] c Rhs condition
     */
    void setRhsCondition(XCondition * c);

  private:
    //! \brief Read in value before being set as lhs or rhs
    std::string tempValue_;
    //! \brief Read in lhs value before processing
    std::string lhs_;
    //! \brief Read in op value before processing
    std::string op_;
    //! \brief Read in rhs value before processing
    std::string rhs_;
    //! \brief If condition is negated
    bool isNot_;
    //! \brief If condition is time based
    bool isTime_;
    //! \brief Time period string
    std::string timePeriod_;
    //! \brief Time phase variable string
    std::string timePhaseVariable_;
    //! \brief Time duration string
    std::string timeDurationString_;
    //! \brief Time phase integer value
    int timePhaseValue_;
    //! \brief If time phase is a variable
    bool timePhaseIsVariable_;
    //! \brief Time duration integer value
    int timeDuration_;
    //! \brief If found time duration
    bool foundTimeDuration_;
    //! \brief Condition uses values
    bool isValues_;
    //! \brief If lhs is a value
    bool lhsIsValue_;
    //! \brief If rhs is a value
    bool rhsIsValue_;
    //! \brief If lhs is an agent variable
    bool lhsIsAgentVariable_;
    //! \brief If rhs is an agent variable
    bool rhsIsAgentVariable_;
    //! \brief If lhs is a message variable
    bool lhsIsMessageVariable_;
    //! \brief If rhs is a message variable
    bool rhsIsMessageVariable_;
    //! \brief Lhs double value
    double lhsDouble_;
    //! \brief Rhs double value
    double rhsDouble_;
    //! \brief If condition is a nested condition
    bool isConditions_;
    //! \brief If lhs is a condition
    bool lhsIsCondition_;
    //! \brief If rhs is a condition
    bool rhsIsCondition_;
    //! \brief Pointer to lhs sub condition
    XCondition * lhsCondition_;
    //! \brief Pointer to rhs sub condition
    XCondition * rhsCondition_;
    //! \brief Agent variables that the condition reads
    std::set<std::string> readOnlyVariables_;

    /*!
     * \brief Print values
     */
    void printValues();
    /*!
     * \brief Print conditions
     */
    void printConditions();
    /*!
     * \brief Print time condition
     */
    void printTime();
    /*!
     * \brief Process value string
     * \param[in,out] hs Value string that can become an agent/message variable
     * \param[out] hsIsAgentVariable If agent variable
     * \param[out] hsIsValue If value
     * \param[out] hsIsMessageVariable If messager variable
     * \param[out] hsDouble If value then the value
     * \return Number of errors
     */
    int processSymbolsValue(std::string * hs, bool * hsIsAgentVariable,
        bool * hsIsValue, bool * hsIsMessageVariable, double * hsDouble);
    /*!
     * \brief Validate a value if agent/message variable name
     * \param[in] agent Pointer to agent
     * \param[in] xmessage Pointer to message
     * \param[in] hsIsAgentVariable If agent variable
     * \param[in] hs The value string
     * \param[in] hsIsMessageVariable If message variable
     * \param[in] rootCondition The root condition to add agent variables read to
     * \return Number of errors
     */
    int validateValue(XMachine * agent, XMessage * xmessage,
        bool * hsIsAgentVariable, std::string * hs,
        bool * hsIsMessageVariable, XCondition * rootCondition);
};
}}  // namespace flame::model
#endif  // MODEL__XCONDITION_HPP_
