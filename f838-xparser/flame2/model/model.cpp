/*!
 * \file flame2/model/model.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Model: model object for users to use
 */
#include <cstdio>
#include <string>
#include "flame2/config.hpp"
#include "flame2/io/io_manager.hpp"
#include "flame2/exceptions/model.hpp"
#include "model.hpp"

namespace flame {
namespace model {

Model::Model() {}

Model::Model(std::string path_to_model) {
  // Load model
  flame::io::IOManager::GetInstance().loadModel(path_to_model, &model_);
  // Validate model
  validate();
}

flame::model::XModel * Model::getXModel() {
  return &model_;
}

void Model::registerAgentFunction(std::string name,
    flame::exe::TaskFunction f_ptr) {
  model_.registerAgentFunction(name, f_ptr);
}

void Model::validate() {
  if (model_.validate() != 0) throw flame::exceptions::flame_model_exception(
      "Model could not be validated");
}

void Model::addAgent(std::string name) {
  model_.addAgent(name);
}

XMachine * Model::getAgent(std::string name) {
  // try and get named agent
  XMachine * agent = model_.getAgent(name);
  // if agent exists return
  if (agent) return agent;
  // if agent does not exist
  throw flame::exceptions::flame_model_exception("Agent does not exist");
}

void Model::addAgentVariable(std::string agent_name,
    std::string type, std::string name) {
  // get named agent
  XMachine * agent = getAgent(agent_name);
  // add variable
  agent->addVariable(type, name);
}

void Model::addAgentFunction(std::string agent_name, std::string name,
    std::string current_state, std::string next_state) {
  // get named agent
  XMachine * agent = getAgent(agent_name);
  // add function
  agent->addFunction(name, current_state, next_state);
}

void Model::addAgentFunctionInput(std::string agent_name, std::string func_name,
    std::string current_state, std::string next_state, std::string name) {
  // get named agent
  XMachine * agent = getAgent(agent_name);
  // get named function
  XFunction * func = agent->getFunction(func_name, current_state, next_state);
  // add input
  func->addInput(name);
}

void Model::addAgentFunctionOutput(std::string agent_name,
    std::string func_name, std::string current_state, std::string next_state,
    std::string name) {
  // get named agent
  XMachine * agent = getAgent(agent_name);
  // get named function
  XFunction * func = agent->getFunction(func_name, current_state, next_state);
  // add output
  func->addOutput(name);
}

void Model::addAgentFunctionReadWriteVariable(std::string agent_name,
    std::string func_name, std::string current_state, std::string next_state,
    std::string name) {
  // get named agent
  XMachine * agent = getAgent(agent_name);
  // get named function
  XFunction * func = agent->getFunction(func_name, current_state, next_state);
  // add read write variable
  func->addReadWriteVariable(name);
}

void Model::addAgentFunctionReadOnlyVariable(std::string agent_name,
    std::string func_name, std::string current_state, std::string next_state,
    std::string name) {
  // get named agent
  XMachine * agent = getAgent(agent_name);
  // get named function
  XFunction * func = agent->getFunction(func_name, current_state, next_state);
  // add read only variable
  func->addReadOnlyVariable(name);
}

void Model::addMessage(std::string name) {
  model_.addMessage(name);
}

void Model::addMessageVariable(std::string message_name,
    std::string type, std::string name) {
  // get named message
  XMessage * message = model_.getMessage(message_name);
  // add variable
  message->addVariable(type, name);
}

}}  // namespace flame::model
