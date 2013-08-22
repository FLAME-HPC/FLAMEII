/*!
 * \file flame2/model/xvariables_validate.hpp
 * \author Simon Coakley
 * \date 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XVariablesValidate: holds variables validator
 */
#ifndef MODEL__XVARIABLES_VALIDATE_HPP_
#define MODEL__XVARIABLES_VALIDATE_HPP_
#include <map>
#include <string>
#include "xdatatype.hpp"

namespace flame { namespace model {

//! \brief Used to validate variables
class XVariablesValidate {
  public:
    /*!
     * \brief Constructor
     * \param[in] datatypes Model data types
     */
    explicit XVariablesValidate(const boost::ptr_vector<XDataType>& datatypes);
    /*!
     * \brief Validate variables
     * \param[in] variables_ The variables
     * \param[in] allowDyamicArrays Flag to allow dynamic arrays
     * \return Number of errors
     */
    int validateVariables(boost::ptr_vector<XVariable> * variables_,
            bool allowDyamicArrays);

  private:
    //! \brief A list of data types to check variables
    const boost::ptr_vector<XDataType>& datatypes_;
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
     * \brief Cast string to integer
     * \param[in] str The string
     * \param[out] i The integer
     * \return False if error, true if successful
     */
    bool castStringToInt(std::string * str, int * i);
};

}}  // namespace flame::model
#endif  // MODEL__XVARIABLES_VALIDATE_HPP_
