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
#include <string>
#include <vector>
#include <set>
#include "flame2/exe/task_interface.hpp"
#include "xvariable.hpp"
#include "xfunction.hpp"
#include "xgraph.hpp"

namespace flame { namespace model {

class XMachine {
  public:
    XMachine();
    ~XMachine();
    void print();
    void setName(std::string name);
    const std::string getName();
    XVariable * addVariable();
    boost::ptr_vector<XVariable> * getVariables();
    XVariable * getVariable(std::string name);
    XFunction * addFunction();
    std::vector<XFunction*> * getFunctions();
    bool validateVariableName(std::string name);
    int findStartEndStates();
    std::string getStartState();
    std::set<std::string> getEndStates();
    int generateStateGraph();
    XGraph * getFunctionDependencyGraph();
    int checkCyclicDependencies();
    int checkFunctionConditions();
    int generateDependencyGraph();
    void registerWithMemoryManager();
    void addToModelGraph(XGraph * modelGraph);

  private:
    std::string name_;
    boost::ptr_vector<XVariable> variables_;
    std::vector<XFunction*> functions_;
    std::string startState_;
    std::set<std::string> endStates_;
    XGraph functionDependencyGraph_;
};
}}  // namespace flame::model
#endif  // MODEL__XMACHINE_HPP_
