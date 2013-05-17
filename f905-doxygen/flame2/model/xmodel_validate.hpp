/*!
 * \file flame2/model/xmodel_validate.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XModelValidate: holds model validator
 */
#ifndef MODEL__XMODEL_VALIDATE_HPP_
#define MODEL__XMODEL_VALIDATE_HPP_
#include <string>
#include <vector>
#include <set>
#include "xmachine.hpp"
#include "xvariable.hpp"
#include "xadt.hpp"
#include "xtimeunit.hpp"
#include "xmessage.hpp"

namespace flame { namespace model {

/*!
 * \brief This class is used to validate a model
 */
class XModelValidate {
  public:
    /*!
     * \brief Constructor
     * \param[in] m The XModel to validate
     */
    explicit XModelValidate(XModel * m);
    /*!
     * \brief Validate the model
     * \return The number of errors found
     * \todo Check condition op IN has valid variables (if implemented).
     * \todo Check for no agents (if needed).
     * \todo Remove agents with no memory variables (if needed).
     * Detailed description.
     */
    int validate();

  private:
    //! \brief The model to validate
    XModel * model;
    //! \brief List of function files
    std::vector<std::string> * functionFiles_;
    //! \brief List of environment constants
    boost::ptr_vector<XVariable> * constants_;
    //! \brief List of abstract data types
    boost::ptr_vector<XADT> * adts_;
    //! \brief List of time units
    boost::ptr_vector<XTimeUnit> * timeUnits_;
    //! \brief List of agents
    boost::ptr_vector<XMachine> * agents_;
    //! \brief List of messages
    boost::ptr_vector<XMessage> * messages_;
    /*!
     * \brief Handle dynamic arrays in variable type
     * \param[in] variable The variable
     */
    void processVariableDynamicArray(XVariable * variable);
    /*!
     * \brief Handle static arrays in variable name
     * \param[in] variable The variable
     * \return Number of errors
     */
    int processVariableStaticArray(XVariable * variable);
    /*!
     * \brief Process variable
     * \param[in] variable The variable
     * \return Number of errors
     */
    int processVariable(XVariable * variable);
    /*!
     * \brief Process variables
     * \param[in] variables_ The variables
     * \return Number of errors
     */
    int processVariables(boost::ptr_vector<XVariable> * variables_);
    /*!
     * \brief Check variable exists within variables list
     * \param[in] name Variable name
     * \param[in] variables The variables
     * \return Boolean result
     */
    bool variableExists(std::string name,
            boost::ptr_vector<XVariable> * variables);
    /*!
     * \brief Process memory access variable
     * \param[in] name Variable name
     * \param[in] variables The variables
     * \param[in,out] usedVariables Variable list used to check duplicates
     * \return Number of errors
     */
    int processMemoryAccessVariable(std::string name,
            boost::ptr_vector<XVariable> * variables,
            std::set<std::string> * usedVariables);
    /*!
     * \brief Process agent function
     * \param[in] function The function
     * \param[in] variables Agent variables
     * \return Number of errors
     */
    int processAgentFunction(XFunction * function,
            boost::ptr_vector<XVariable> * variables);
    /*!
     * \brief Validate variable name
     * \param[in] v The variable to validate
     * \param[in,out] errors The number of errors
     * \param[in] variables Agent variables
     */
    void validateVariableName(XVariable * v, int * errors,
            boost::ptr_vector<XVariable> * variables);
    /*!
     * \brief Validate variable type
     * \param[in] v The variable to validate
     * \param[in,out] errors The number of errors
     * \param[in] allowDyamicArrays Flag to allow dynamic arrays
     */
    void validateVariableType(XVariable * v, int * errors,
            bool allowDyamicArrays);
    /*!
     * \brief Validate variables
     * \param[in] variables_ The variables
     * \param[in] allowDyamicArrays Flag to allow dynamic arrays
     * \return Number of errors
     */
    int validateVariables(boost::ptr_vector<XVariable> * variables_,
            bool allowDyamicArrays);
    /*!
     * \brief Validate function file
     * \param[in] name Function file name
     * \return Number of errors
     */
    int validateFunctionFile(std::string name);
    /*!
     * \brief Validate time unit
     * \param[in] timeUnit The time unit
     * \return Number of errors
     */
    int validateTimeUnit(XTimeUnit * timeUnit);
    /*!
     * \brief process time unit period
     * \param[in] timeUnit The time unit
     * \return Number of errors
     */
    int processTimeUnitPeriod(XTimeUnit * timeUnit);
    /*!
     * \brief Process time unit unit
     * \param[in] timeUnit The time unit
     * \return Number of errors
     */
    int processTimeUnitUnit(XTimeUnit * timeUnit);
    /*!
     * \brief Process time unit
     * \param[in] timeUnit The time unit
     * \return Number of errors
     */
    int processTimeUnit(XTimeUnit * timeUnit);
    /*!
     * \brief Validate abstract data type
     * \param[in] adt The abstract data type
     * \return Number of errors
     */
    int validateADT(XADT * adt);
    /*!
     * \brief Validate agent
     * \param[in] agent The agent
     * \return Number of errors
     */
    int validateAgent(XMachine * agent);
    /*!
     * \brief Validate agent state graph
     * \param[in] agent The agent
     * \return Number of errors
     */
    int validateAgentStateGraph(XMachine * agent);
    /*!
     * \brief Validate agent function inputs and outputs
     * \param[in] xfunction The function
     * \param[in] agent The agent
     * \return Number of errors
     */
    int validateAgentFunctionIOput(XFunction * xfunction, XMachine * agent);
    /*!
     * \brief Validate agent function
     * \param[in] xfunction The function
     * \param[in] agent The agent
     * \return Number of errors
     */
    int validateAgentFunction(XFunction * xfunction, XMachine * agent);
    /*!
     * \brief Validate agent communication
     * \param[in] xioput The input or output
     * \param[in] agent The agent
     * \return Number of errors
     */
    int validateAgentCommunication(XIOput * xioput, XMachine * agent);
    /*!
     * \brief Validate agent condition/filter
     * \param[in] xcondition The condition/filter
     * \param[in] agent The agent
     * \param[in] xmessage The message
     * \return Number of errors
     */
    int validateAgentConditionOrFilter(XCondition * xcondition,
            XMachine * agent, XMessage * xmessage);
    /*!
     * \brief Validate input sort
     * \param[in] xioput The input
     * \param[in] xmessage The message
     * \return Number of errors
     */
    int validateSort(XIOput * xioput, XMessage * xmessage);
    /*!
     * \brief Validate input random string
     * \param[in] xioput The input
     * \return Number of errors
     */
    int validateRandomString(XIOput * xioput);
    /*!
     * \brief Validate message
     * \param[in] xmessage The message
     * \return Number of errors
     */
    int validateMessage(XMessage * xmessage);
    /*!
     * \brief Validates a name string
     * \param[in] name The string to validate
     * \return Boolean result
     * The iterator tries to find the first element that satisfies the predicate.
     * If no disallowed character is found then you end with name.end() which is
     * taken to be success and true is returned.
     */
    bool name_is_allowed(std::string name);
    /*!
     * \brief Validate time units
     * \param[in] timeUnits The list of time units
     * \return Number of errors
     */
    int validateTimeUnits(boost::ptr_vector<XTimeUnit> * timeUnits);
    /*!
     * \brief Validate model graph
     */
    void validateModelGraph();
    /*!
     * \brief Cast string to integer
     * \param[in] str The string
     * \param[out] i The integer
     * \return False if error, true if successful
     */
    bool castStringToInt(std::string * str, int * i);
};
}}  // namespace flame::model
#endif  // MODEL__XMODEL_VALIDATE_HPP_
