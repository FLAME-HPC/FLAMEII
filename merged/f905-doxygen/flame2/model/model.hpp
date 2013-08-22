/*!
 * \file flame2/model/model.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Model: model object for users to use
 */

#ifndef MODEL__MODEL_HPP_
#define MODEL__MODEL_HPP_
#include <string>
#include <set>
#include <map>
#include "flame2/model/xmodel.hpp"
#include "flame2/model/agentfunction.hpp"

namespace flame { namespace model {

/*!
 * \brief Used to create a model and register with framework
 */
class Model {
  public:
    /*!
     * \brief Constructor
     */
    Model();
    /*!
     * \brief Constructor with model name
     * \param[in] model The model name
     */
    explicit Model(std::string model);
    /*!
     * \brief Register agent function with the framework
     * \param[in] name The function name
     * \param[in] f_ptr Framework task function
     */
    void registerAgentFunction(std::string name,
            flame::exe::TaskFunction f_ptr);
    /*!
     * \brief Validate the model
     */
    void validate();
    /*!
     * \brief Add an agent
     * \param[in] name The agent name
     */
    void addAgent(std::string name);
    /*!
     * \brief Add an agent variable
     * \param[in] agent_name The agent name
     * \param[in] type The variable type
     * \param[in] name The variable name
     */
    void addAgentVariable(std::string agent_name,
            std::string type, std::string name);
    /*!
     * \brief Add an agent function
     * \param[in] agent_name The agent name
     * \param[in] agentFunction The agent function
     */
    void addAgentFunction(std::string agent_name,
        const AgentFunction& agentFunction);
    /*!
     * \brief Add a message
     * \param[in] name The message name
     */
    void addMessage(std::string name);
    /*!
     * \brief Add a message variable
     * \param[in] message_name The message name
     * \param[in] type The variable type
     * \param[in] name The variable name
     */
    void addMessageVariable(std::string message_name,
            std::string type, std::string name);
    /*!
     * \brief Get the task function of an agent function
     * \param[in] name The function name
     * \return The task function
     */
    flame::exe::TaskFunction getAgentFunctionPointer(std::string name) const;
    /*!
     * \brief Check the model has been validated
     * \return Boolean result
     */
    bool isValidated() const;
    /*!
     * \brief Get agent memory info
     * \return The agent memory info
     */
    AgentMemory getAgentMemoryInfo() const;
    /*!
     * \brief Get task dependencies
     * \return Task dependencies
     */
    TaskIdMap getTaskDependencies() const;
    /*!
     * \brief Get the task list
     * \return The task list
     */
    const TaskList * getTaskList() const;
#ifdef TESTBUILD
    /*!
     * \brief Get the underlying model
     * \return The underlying model
     */
    flame::model::XModel * getXModel() { return &model_; }
#endif

  private:
    //! \brief Check for a validated model
    bool validated_;
    //! \brief The underlying model
    flame::model::XModel model_;
    //! \brief A map from function name to function pointer
    std::map<std::string, flame::exe::TaskFunction> funcMap_;

    /*!
     * \brief Get agent
     * \param[in] name Agent name
     * \return The agent
     *
     * getAgent handled in Model because XModel returns 0 if
     * there is no agent so Model throws the exception
     */
    XMachine * getAgent(std::string name);
};

}}  // namespace flame::model
#endif  // MODEL__MODEL_HPP_
