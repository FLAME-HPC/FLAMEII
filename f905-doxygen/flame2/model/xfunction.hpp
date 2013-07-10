/*!
 * \file flame2/model/xfunction.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XFunction: holds transition function information
 */
#ifndef MODEL__XFUNCTION_HPP_
#define MODEL__XFUNCTION_HPP_
#include <boost/ptr_container/ptr_vector.hpp>
#include <string>
#include <vector>
#include "xioput.hpp"
#include "xcondition.hpp"
#include "xvariable.hpp"

namespace flame { namespace model {

class Task;

/*!
 * \brief Class to hold agent function information
 */
class XFunction {
  public:
    /*!
     * \brief Constructor
     *
     * Initialises XFunction with no condition and no memory access info.
     */
    XFunction();
    /*!
     * \brief Constructs XFunction
     * \param[in] name The function name
     *
     * Initialises XFunction with a name and no condition and
     * no memory access info.
     */
    explicit XFunction(std::string name);
    /*!
     * \brief Destructor
     *
     * Cleans up XFunction by deleting condition and ioputs.
     */
    ~XFunction();
    /*!
     * \brief Prints the XFunction
     *
     * Prints the XFunction to standard out.
     */
    void print();
    /*!
     * \brief Set the name
     * param[in] name The function name
     */
    void setName(std::string name);
    /*!
     * \brief Get the function name
     * \return The name
     */
    std::string getName();
    /*!
     * \brief Set the source state
     * \param[in] name The source state name
     */
    void setCurrentState(std::string name);
    /*!
     * \brief Get the source state name
     * \return The source state name
     */
    std::string getCurrentState();
    /*!
     * \brief Set the target state name
     * \param[in] name The target state name
     */
    void setNextState(std::string name);
    /*!
     * \brief Get the target state name
     * \return The target state name
     */
    std::string getNextState();
    /*!
     * \brief Add an input
     * \return Pointer to the new XIOput
     */
    XIOput * addInput();
    /*!
     * \brief Add an input using the message name
     * \param[in] name The message name
     * \return Pointer to the new XIOput
     */
    XIOput * addInput(std::string name);
    /*!
     * \brief Get inputs
     * \return Pointer to boost pointer vector of XIOput
     */
    boost::ptr_vector<XIOput> * getInputs();
    /*!
     * \brief Add output
     * \return Pointer to new XIOput
     */
    XIOput * addOutput();
    /*!
     * \brief Add output using message name
     * \param[in] name The message name
     * \return Pointer to new XIOput
     */
    XIOput * addOutput(std::string name);
    /*!
     * \brief Get outputs
     * \return Pointer to boost pointer vector of XIOput
     */
    boost::ptr_vector<XIOput> * getOutputs();
    /*!
     * \brief Add condition
     * \return Pointer to new XCondition
     */
    XCondition * addCondition();
    /*!
     * \brief Get condition
     * \return Pointer to XCondition
     */
    XCondition * getCondition();
    /*!
     * \brief Set memory access information as being available
     * \param[in] b Boolean value
     */
    void setMemoryAccessInfoAvailable(bool b);
    /*!
     * \brief Is memory access information available
     * \return Boolean value
     */
    bool getMemoryAccessInfoAvailable();
    /*!
     * \brief Add read only variable
     * \param[in] name The variable name
     */
    void addReadOnlyVariable(std::string name);
    /*!
     * \brief Get read only variables
     * \return String vector of read only variables
     */
    std::vector<std::string> * getReadOnlyVariables();
    /*!
     * \brief Add read write variable
     * \param[in] name The variable name
     */
    void addReadWriteVariable(std::string name);
    /*!
     * \brief Get read write variables
     * \return String vector of read write variables
     */
    std::vector<std::string> * getReadWriteVariables();

  private:
    //! \brief The function name
    std::string name_;
    //! \brief The source state
    std::string currentState_;
    //! \brief The target state
    std::string nextState_;
    //! \brief List of inputs
    boost::ptr_vector<XIOput> inputs_;
    //! \brief List of outputs
    boost::ptr_vector<XIOput> outputs_;
    //! \brief The condition if it exists
    XCondition * condition_;
    //! \brief If information on memory access available
    bool memoryAccessInfoAvailable_;
    //! \brief List of read only variables the function accesses
    std::vector<std::string> readOnlyVariables_;
    //! \brief List of read only variables the function accesses
    std::vector<std::string> readWriteVariables_;
    //! \brief This class has pointer members so disable copy constructor
    XFunction(const XFunction&);
    //! \brief This class has pointer members so disable assignment operation
    void operator=(const XFunction&);
};
}}  // namespace flame::model
#endif  // MODEL__XFUNCTION_HPP_
