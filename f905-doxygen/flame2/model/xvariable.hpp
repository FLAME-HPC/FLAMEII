/*!
 * \file flame2/model/xvariable.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XVariable: holds agent variable information
 */
#ifndef MODEL__XVARIABLE_HPP_
#define MODEL__XVARIABLE_HPP_
#include <string>

namespace flame { namespace model {

/*!
 * \brief Class that holds model variable information
 */
class XVariable {
  public:
    /*!
     * \brief Constructor
     *
     * Initialises XVariable as
     * fundamental (int or double)
     * variable (not constant)
     * scaler (not an array).
     */
    XVariable();
    /*!
     * \brief Constructor
     * \param[in] name The variable name
     *
     * Initialises XVariable using a name.
     */
    explicit XVariable(std::string name);
    /*!
     * \brief Prints XVariable
     *
     * Prints XVariable to standard out.
     */
    void print();
    /*!
     * \brief Set the variable name
     * \param[in] name The variable name
     */
    void setName(std::string name);
    /*!
     * \brief Get the variable name
     * \return The variable name
     */
    std::string getName() const;
    /*!
     * \brief Set the variable type
     * \param[in] type The variable type
     */
    void setType(std::string type);
    /*!
     * \brief Get the variable type
     * \return The variable type
     */
    std::string getType() const;
    /*!
     * \brief Set the variable as a dynamic array
     * \param[in] b True or false
     */
    void setIsDynamicArray(bool b);
    /*!
     * \brief Is the variable a dynamic array
     * \return True or false
     */
    bool isDynamicArray();
    /*!
     * \brief Set the variable as a static array
     * \param[in] b True or false
     */
    void setIsStaticArray(bool b);
    /*!
     * \brief Is the variable a static array
     * \return True or false
     */
    bool isStaticArray();
    /*!
     * \brief Set the variable static array size
     * \param[in] size The size of the static array
     */
    void setStaticArraySize(int size);
    /*!
     * \brief Get the variable static array size
     * \return The static array size
     */
    int getStaticArraySize();
    /*!
     * \brief Set the variable as an ADT
     * \param[in] b True or false
     */
    void setHasADTType(bool b);
    /*!
     * \brief Is the variable an ADT
     * \return True or false
     */
    bool hasADTType();
    /*!
     * \brief Set the variable as holding a dynamic array
     * \param[in] b True or false
     */
    void setHoldsDynamicArray(bool b);
    /*!
     * \brief Is the variable holding a dynamic array
     * \return True or false
     */
    bool holdsDynamicArray();
    /*!
     * \brief Set the variable constant string
     * \param[in] name The variable constant string
     */
    void setConstantString(std::string name);
    /*!
     * \brief Get the variable constant string
     * \return The variable constant string
     */
    std::string getConstantString();
    /*!
     * \brief Has the variable constant been set
     * \param[in] set True or false
     */
    void setConstantSet(bool set);
    /*!
     * \brief Has the variable constant been set
     * \return True or false
     */
    bool isConstantSet();
    /*!
     * \brief Set the variable as constant
     * \param[in] constant True or false
     */
    void setConstant(bool constant);
    /*!
     * \brief Is the variable constant
     * \return True or false
     */
    bool isConstant();

  private:
    //! \brief The variable type
    std::string type_;
    //! \brief The variable name
    std::string name_;
    //! \brief If the variable is a dynamic array
    bool isDynamicArray_;
    //! \brief If the variable is a static array
    bool isStaticArray_;
    //! \brief The static array size
    int staticArraySize_;
    //! \brief If the variable is an ADT
    bool hasADTType_;
    //! \brief If the variable holds a dynamic array
    bool holdsDynamicArray_;
    //! \brief String to hold ...
    std::string constantString_;
    //! \brief If the variable constant has been set
    bool constantSet_;
    //! \brief If the variable is constant
    bool constant_;
};
}}  // namespace flame::model
#endif  // MODEL__XVARIABLE_HPP_
