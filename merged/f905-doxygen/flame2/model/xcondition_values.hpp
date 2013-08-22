/*!
 * \file flame2/model/xcondition_values.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XConditionValues: holds condition values
 */
#ifndef MODEL__XCONDITION_VALUES_HPP_
#define MODEL__XCONDITION_VALUES_HPP_
#include <string>
#include <set>

namespace flame { namespace model {

class XVariable;
class XMachine;
class XMessage;
class XModel;
class XCondition;

/*!
 * \brief Class to hold condition values
 */
class XConditionValues {
  public:
    XConditionValues();
    /*!
     * \brief Print LHS value
     */
    void printLHSValue();
    /*!
     * \brief Print RHS value
     */
    void printRHSValue();
    /*!
     * \brief Process symbol values
     * \return Number of errors
     */
    int processSymbolsValues();
    /*!
     * \brief Check if lhs and rhs are values
     * \return The result
     */
    bool isLHSandRHSvalues();
    /*!
     * \brief Validate values
     * \param[in] agent The agent
     * \param[in] xmessage The message
     * \param[in] readOnlyVariables Condition agent variables
     * \return Number of errors
     */
    int validateValues(XMachine * agent, XMessage * xmessage,
        std::set<std::string> * readOnlyVariables);
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

  private:
    //! \brief Read in value before being set as lhs or rhs
    std::string tempValue_;
    //! \brief Read in lhs value before processing
    std::string lhs_;
    //! \brief Read in rhs value before processing
    std::string rhs_;
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
        bool * hsIsMessageVariable, std::set<std::string> * readOnlyVariables);
};
}}  // namespace flame::model
#endif  // MODEL__XCONDITION_VALUES_HPP_
