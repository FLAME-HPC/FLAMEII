/*!
 * \file flame2/model/xadt.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XADT: holds abstract data type information
 */
#ifndef MODEL__XADT_HPP_
#define MODEL__XADT_HPP_
#include <boost/ptr_container/ptr_vector.hpp>
#include <string>
#include <vector>
#include "xvariable.hpp"

namespace flame { namespace model {

/*!
 * Class to hold user define abstract data type information
 */
class XADT {
  public:
    /*!
     * \brief Constructor
     *
     * Initialises holding dynamic arrays to be false.
     */
    XADT();
    /*!
     * \brief Prints XADT
     *
     * Prints out XADT name and variables.
     */
    void print();
    /*!
     * \brief Set data type name
     * \param[in] name The name
     */
    void setName(std::string name);
    /*!
     * \brief Get data type name
     * \return Name
     */
    std::string getName();
    /*!
     * \brief Add variable to the data type
     * \return Pointer to new XVariable
     */
    XVariable * addVariable();
    /*!
     * \brief Get list of variables
     * \return Pointer to boost pointer vector of XVariable
     */
    boost::ptr_vector<XVariable> * getVariables();
    /*!
     * \brief Set if data type holds dynamic arrays
     * \param[in] b Boolean value
     */
    void setHoldsDynamicArray(bool b);
    /*!
     * \brief Does data type hold dynamic arrays
     * \return Boolean result
     */
    bool holdsDynamicArray();

  private:
    //! \brief Data type name
    std::string name_;
    //! \brief List of data type variables
    boost::ptr_vector<XVariable> variables_;
    //! \brief Does the data type hold dynamic arrays
    bool holdsDynamicArray_;
};
}}  // namespace flame::model
#endif  // MODEL__XADT_HPP_
