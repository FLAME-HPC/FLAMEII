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
#include "xdatatype.hpp"
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
    boost::ptr_vector<XDataType> * datatypes_;
    //! \brief List of time units
    boost::ptr_vector<XTimeUnit> * timeUnits_;
    //! \brief List of agents
    boost::ptr_vector<XMachine> * agents_;
    //! \brief List of messages
    boost::ptr_vector<XMessage> * messages_;
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
    int validateDataType(XDataType * adt);
    /*!
     * \brief Validate message
     * \param[in] xmessage The message
     * \return Number of errors
     */
    int validateMessage(XMessage * xmessage);
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
};
}}  // namespace flame::model
#endif  // MODEL__XMODEL_VALIDATE_HPP_
