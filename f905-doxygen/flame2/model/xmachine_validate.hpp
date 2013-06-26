/*!
 * \file flame2/model/xmachine_validate.hpp
 * \author Simon Coakley
 * \date 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XMachineValidate: holds agent validator
 */
#ifndef MODEL__XMACHINE_VALIDATE_HPP_
#define MODEL__XMACHINE_VALIDATE_HPP_
#include <string>
#include <vector>
#include <set>
#include "xmachine.hpp"
#include "xmessage.hpp"
#include "xdatatype.hpp"
#include "xtimeunit.hpp"
#include "printerr.hpp"

namespace flame { namespace model {

class XMachineValidate {
  public:
    XMachineValidate(const boost::ptr_vector<XDataType>& datatypes,
        boost::ptr_vector<XMessage>& messages,
        boost::ptr_vector<XTimeUnit>& timeUnits);
    /*!
     * \brief Validate agent
     * \param[in] agent The agent
     * \return Number of errors
     */
    int validateAgent(XMachine * agent);

  private:
    //! \brief A list of data types to check variables
    const boost::ptr_vector<XDataType>& datatypes_;
    //! \brief List of messages
    boost::ptr_vector<XMessage>& messages_;
    //! \brief List of time units
    boost::ptr_vector<XTimeUnit>& timeUnits_;
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
};
}}  // namespace flame::model
#endif  // MODEL__XMACHINE_VALIDATE_HPP_
