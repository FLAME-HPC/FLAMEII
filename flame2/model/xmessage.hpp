/*!
 * \file flame2/model/xmessage.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XMessage: holds message information
 */
#ifndef MODEL__XMESSAGE_HPP_
#define MODEL__XMESSAGE_HPP_
#include <boost/ptr_container/ptr_vector.hpp>
#include <string>
#include <vector>
#include "xvariable.hpp"

namespace flame { namespace model {

/*!
 * \brief Class to hold model messages information
 */
class XMessage {
  public:
    /*!
     * \brief Constructor
     *
     * Initialises id to be zero and start and finish tasks to be null.
     */
    XMessage();
    /*!
     * \brief Prints XMessage
     *
     * Prints XMessage to standard out.
     */
    void print();
    /*!
     * \brief Set the message name
     * \param[in] name The message name
     */
    void setName(std::string name);
    /*!
     * \brief Set the message name
     * \return The message name
     */
    std::string getName();
    /*!
     * \brief Add a variable
     * \return Pointer to the newly created XVariable
     */
    XVariable * addVariable();
    /*!
     * \brief Add a variable with the variable type and name
     * \param[in] type The variable type
     * \param[in] name The variable name
     * \return Pointer to the newly created XVariable
     */
    XVariable * addVariable(std::string type, std::string name);
    /*!
     * \brief Get variables
     * \return A pointer to a boost pointer vector with XVariable
     */
    boost::ptr_vector<XVariable> * getVariables();
    /*!
     * \brief Finds a variable name in the message memory
     * \param[in] name The variable name
     * \return The boolean result
     */
    bool validateVariableName(std::string name);
    /*!
     * \brief Set identification number
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
    //! \brief Message name
    std::string name_;
    //! \brief Message variables
    boost::ptr_vector<XVariable> variables_;
};
}}  // namespace flame::model
#endif  // MODEL__XMESSAGE_HPP_
