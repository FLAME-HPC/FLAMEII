/*!
 * \file flame2/model/xmachine.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XMachine: holds agent information
 */
#ifndef MODEL__XMACHINE_HPP_
#define MODEL__XMACHINE_HPP_
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <string>
#include <vector>
#include <set>
#include <utility>
#include "xvariable.hpp"
#include "xfunction.hpp"
#include "stategraph.hpp"
#include "dependencygraph.hpp"

namespace flame { namespace model {

typedef std::pair<std::string, std::string> Var;

class XMachine {
  public:
    XMachine();
    void print();
    void setName(std::string name);
    std::string getName() const;
    XVariable * addVariable();
    XVariable * addVariable(std::string type, std::string name);
    boost::ptr_vector<XVariable> * getVariables();
    std::set<Var> getVariablesSet() const;
    std::vector<Var> getVariablesVector() const;
    XVariable * getVariable(std::string name);
    XFunction * addFunction();
    XFunction * addFunction(std::string name,
            std::string currentState, std::string nextState);
    XFunction * getFunction(std::string name,
            std::string current_state, std::string next_state);
    boost::ptr_vector<XFunction> * getFunctions();
    bool validateVariableName(std::string name);
    int findStartEndStates();
    std::string getStartState();
    std::set<std::string> getEndStates();
    int generateStateGraph();
    int generateDependencyGraph();
    StateGraph * getStateGraph();
    DependencyGraph * getDependencyGraph();
    std::pair<int, std::string> checkCyclicDependencies();
    std::pair<int, std::string> checkFunctionConditions();
    void setID(int id);
    int getID();

  private:
    int id_;
    std::string name_;
    boost::ptr_vector<XVariable> variables_;
    boost::ptr_vector<XFunction> functions_;
    std::string startState_;
    std::set<std::string> endStates_;
    //! Graph used to hold state then dependency graph
    StateGraph stateGraph_;
    DependencyGraph dependencyGraph_;
};
}}  // namespace flame::model
#endif  // MODEL__XMACHINE_HPP_
