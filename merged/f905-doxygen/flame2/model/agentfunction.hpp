/*!
 * \file flame2/model/agentfunction.hpp
 * \author Simon Coakley
 * \date 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief AgentFunction: holds agent function information
 */
#ifndef MODEL__AGENTFUNCTION_HPP_
#define MODEL__AGENTFUNCTION_HPP_
#include <boost/ptr_container/ptr_vector.hpp>
#include <string>
#include <set>

namespace flame { namespace model {

/*!
 * \brief Class to hold agent function information
 */
class AgentFunction {
  public:
    /*!
     * \brief Constructor
     * \param[in] name Function name
     * \param[in] current_state Source state
     * \param[in] next_state Target state
     */
    explicit AgentFunction(std::string name,
        std::string current_state, std::string next_state);
    /*!
     * \brief Add input
     * \param[in] name Message name
     */
    void addInput(std::string name);
    /*!
     * \brief Add output
     * \param[in] name Message name
     */
    void addOutput(std::string name);
    /*!
     * \brief Add read only variable
     * \param[in] name Variable name
     */
    void addReadOnlyVariable(std::string name);
    /*!
     * \brief Add read write variable
     * \param[in] name Variable name
     */
    void addReadWriteVariable(std::string name);
    /*!
     * \brief Get function name
     * \return Function name
     */
    std::string getName() const;
    /*!
     * \brief Get source state
     * \return Source state name
     */
    std::string getCurrentState() const;
    /*!
     * \brief Get target state
     * \return Target state name
     */
    std::string getNextState() const;
    /*!
     * \brief Get inputs
     * \return Set of message names
     */
    std::set<std::string> getInputs() const;
    /*!
     * \brief Get outputs
     * \return Set of message names
     */
    std::set<std::string> getOutputs() const;
    /*!
     * \brief Get read only variables
     * \return Set of variable names
     */
    std::set<std::string> getReadOnlyVariables() const;
    /*!
     * \brief Get read write variables
     * \return Set of variable names
     */
    std::set<std::string> getReadWriteVariables() const;

  private:
    //! \brief Function name
    std::string name_;
    //! \brief Source state
    std::string currentState_;
    //! \brief Target state
    std::string nextState_;
    //! \brief Set of inputs
    std::set<std::string> inputs_;
    //! \brief Set of outputs
    std::set<std::string> outputs_;
    //! \brief Set of read only variables
    std::set<std::string> readOnlyVariables_;
    //! \brief Set of read write variables
    std::set<std::string> readWriteVariables_;
};
}}  // namespace flame::model
#endif  // MODEL__AGENTFUNCTION_HPP_
