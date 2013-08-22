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
#include <boost/ptr_container/ptr_vector.hpp>
#include "xcondition_values.hpp"
#include "xcondition_time.hpp"
#include "xtimeunit.hpp"

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
     * \brief Process symbol operator
     * \return Number of errors
     */
    int processSymbolsOp();
    /*!
     * \brief Process symbol in conditions
     * \return Number of errors
     */
    int processSymbolsConditions();
    /*!
     * \brief Validate the condition
     * \param[in] agent The agent
     * \param[in] message The message
     * \param[in] timeUnits Model time units
     * \param[in] rootCondition The condition
     * \return Number of errors
     */
    int validate(XMachine * agent, XMessage * message,
        boost::ptr_vector<XTimeUnit>& timeUnits,
        XCondition * rootCondition);
    /*!
     * \brief Validate a time condition
     * \param[in] agent The agent
     * \param[in] timeUnits Model time units
     * \param[in] rootCondition The condition
     * \return Number of errors
     */
    int validateTime(XMachine * agent,
        boost::ptr_vector<XTimeUnit>& timeUnits,
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
    //! \brief Condition values
    XConditionValues values;
    //! \brief Condition time
    XConditionTime time;

  private:
    //! \brief Read in op value before processing
    std::string op_;
    //! \brief If condition is negated
    bool isNot_;
    //! \brief If condition is time based
    bool isTime_;
    //! \brief Condition uses values
    bool isValues_;
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
    //! \brief This class has pointer members so disable copy constructor
    XCondition(const XCondition&);
    //! \brief This class has pointer members so disable assignment operation
    void operator=(const XCondition&);
};
}}  // namespace flame::model
#endif  // MODEL__XCONDITION_HPP_
