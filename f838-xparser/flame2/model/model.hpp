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
#include "flame2/model/xmodel.hpp"
#include "flame2/mb/message_board_manager.hpp"

namespace flame { namespace model {

//! \brief Used to create a model and register with framework
class Model {
  public:
    Model();
    explicit Model(std::string model);
    //! Register agent function with the framework
    void registerAgentFunction(std::string name,
            flame::exe::TaskFunction f_ptr);
    //! Register message type with the framework
    template <typename T>
    void registerMessageType(std::string name) {
      flame::mb::MessageBoardManager::GetInstance().RegisterMessage<T>(name);
    }
    //! Return the underlying model
    flame::model::XModel * getXModel();
    //! Validate the model
    void validate();
    //! Add an agent
    void addAgent(std::string name);
    //! Add an agent variable
    void addAgentVariable(std::string agent_name,
            std::string type, std::string name);
    //! Add an agent function
    void addAgentFunction(std::string agent_name, std::string name,
            std::string current_state, std::string next_state);
    //! Add an agent function input
    void addAgentFunctionInput(std::string agent_name, std::string func_name,
            std::string current_state, std::string next_state,
            std::string name);
    //! Add an agent function output
    void addAgentFunctionOutput(std::string agent_name, std::string func_name,
            std::string current_state, std::string next_state,
            std::string name);
    //! Add an agent function read write variable access
    void addAgentFunctionReadWriteVariable(std::string agent_name,
            std::string func_name,
            std::string current_state, std::string next_state,
            std::string name);
    //! Add an agent function read only variable access
    void addAgentFunctionReadOnlyVariable(std::string agent_name,
            std::string func_name,
            std::string current_state, std::string next_state,
            std::string name);
    //! Add a message
    void addMessage(std::string name);
    //! Add a message variable
    void addMessageVariable(std::string message_name,
            std::string type, std::string name);

  private:
    flame::model::XModel model_;  //!< The underlying model

    XMachine * getAgent(std::string name);  //!< Return the named agent
};

}}  // namespace flame::model
#endif  // MODEL__MODEL_HPP_
