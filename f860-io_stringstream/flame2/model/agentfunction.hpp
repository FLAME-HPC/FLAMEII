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

class AgentFunction {
  public:
    explicit AgentFunction(std::string name,
        std::string current_state, std::string next_state);

    void addInput(std::string name);
    void addOutput(std::string name);
    void addReadOnlyVariable(std::string name);
    void addReadWriteVariable(std::string name);

    std::string getName() const;
    std::string getCurrentState() const;
    std::string getNextState() const;
    std::set<std::string> getInputs() const;
    std::set<std::string> getOutputs() const;
    std::set<std::string> getReadOnlyVariables() const;
    std::set<std::string> getReadWriteVariables() const;

  private:
    std::string name_;
    std::string currentState_;
    std::string nextState_;
    std::set<std::string> inputs_;
    std::set<std::string> outputs_;
    std::set<std::string> readOnlyVariables_;
    std::set<std::string> readWriteVariables_;
};
}}  // namespace flame::model
#endif  // MODEL__AGENTFUNCTION_HPP_
