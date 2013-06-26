/*!
 * \file flame2/model/xdatatype.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XDataType: holds model data type information
 */
#ifndef MODEL__XDATATYPE_HPP_
#define MODEL__XDATATYPE_HPP_
#include <boost/ptr_container/ptr_vector.hpp>
#include <string>
#include <vector>
#include "xvariable.hpp"

namespace flame { namespace model {

/*!
 * Class to hold model data type information
 */
class XDataType {
  public:
    /*!
     * \brief Constructor
     *
     * Initialises holding dynamic arrays to be false.
     */
    XDataType();
    /*!
     * \brief Constructor that sets name and isFundamental
     *
     * Initialises holding dynamic arrays to be false.
     */
    XDataType(std::string name, bool isFundamental);
    /*!
     * \brief Prints data type
     *
     * Prints out data type name and variables.
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
    std::string getName() const;
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
    bool holdsDynamicArray() const;
    /*!
     * \brief Set data type as fundamental
     * \param[in] isFundamental Is fundamental
     */
    void setFundamental(bool isFundamental);
    /*!
     * \brief Is data type fundamental
     * \return Result
     */
    bool isFundamental() const;
    /*!
     * \brief Set the identification number
     * \param[in] id The identification number
     */
    void setID(int id);
    /*!
     * \brief Get the identification number
     * \return The identification number
     */
    int getID();

  private:
    //! \brief Identification number
    int id_;
    //! \brief Data type name
    std::string name_;
    //! \brief List of data type variables
    boost::ptr_vector<XVariable> variables_;
    //! \brief Does the data type hold dynamic arrays
    bool holdsDynamicArray_;
    //! \brief Is the data type fundamental
    bool isFundamental_;
};
}}  // namespace flame::model
#endif  // MODEL__XDATATYPE_HPP_
