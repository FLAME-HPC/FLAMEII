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

/*!
 * \brief Class to hold agent information
 */
class XMachine {
  public:
    XMachine();
    /*!
     * \brief Prints XMachine
     *
     * Prints XMachine to standard out.
     */
    void print();
    /*!
     * \brief Set the agent name
     * \param[in] name The name
     */
    void setName(std::string name);
    /*!
     * \brief Get the agent name
     * \return The agent name
     */
    std::string getName() const;
    /*!
     * \brief Add a variable to the agent memory
     * \return A pointer to the XVariable
     */
    XVariable * addVariable();
    /*!
     * \brief Add a variable to the agent memory using its type and name
     * \param[in] type The variable type
     * \param[in] name The variable name
     * \return A pointer to the XVariable
     */
    XVariable * addVariable(std::string type, std::string name);
    /*!
     * \brief Get the agent variables
     * \return Pointer the a boost pointer vector to XVariable
     */
    boost::ptr_vector<XVariable> * getVariables();
    /*!
     * \brief Get the agent variables as a set of string pairs
     * \return The set of string pairs
     */
    std::set<Var> getVariablesSet() const;
    /*!
     * \brief Get the agent variables as a vector of string pairs
     * \return The vector of string pairs
     */
    std::vector<Var> getVariablesVector() const;
    /*!
     * \brief Get an agent variable via its name
     * \param[in] name The variable name
     * \return The pointer to the associated XVariable
     */
    XVariable * getVariable(std::string name);
    /*!
     * \brief Add an agent function
     * \return Pointer to the XFunction
     */
    XFunction * addFunction();
    /*!
     * \brief Add an agent function giving the agent name and states
     * \param[in] name The function name
     * \param[in] currentState The state the transition is leaving
     * \param[in] nextState The state the transition is arriving
     * \return Pointer to the XFunction
     */
    XFunction * addFunction(std::string name,
            std::string currentState, std::string nextState);
    /*!
     * \brief Get an agent function using its name and states
     * \param[in] name The agent name
     * \param[in] current_state The state the transition is leaving
     * \param[in] next_state The state the transition is arriving
     * \return Pointer tot he XFunction
     */
    XFunction * getFunction(std::string name,
            std::string current_state, std::string next_state);
    /*!
     * \brief Get the agent functions
     * \return Pointer to a boost pointer vector of XFunction
     */
    boost::ptr_vector<XFunction> * getFunctions();
    /*!
     * \brief Validate a variable exists in the agent memory
     * \param[in] name The variable name
     * \return The boolean result
     */
    bool validateVariableName(std::string name);
    /*!
     * \brief Find the agent start and end states
     * \return The return code
     * The return code returns 0 for no error, 1 for no start states found
     * and 2 for multiple start states found
     */
    int findStartEndStates();
    /*!
     * \brief Get the start state
     * \return The start state name
     */
    std::string getStartState();
    /*!
     * \brief Get the end states
     * \return A set of end state names
     */
    std::set<std::string> getEndStates();
    /*!
     * \brief Generate the agent state graph
     */
    void generateStateGraph();
    /*!
     * \brief Generate the agent dependency graph
     */
    void generateDependencyGraph();
    /*!
     * \brief Get the state graph
     * \return The state graph
     */
    StateGraph * getStateGraph();
    /*!
     * \brief Get the dependency graph
     * \return The dependency graph
     */
    DependencyGraph * getDependencyGraph();
    /*!
     * \brief Check cyclic dependencies
     * \return First integer for number of errors,
     *         second string for error message
     */
    std::pair<int, std::string> checkCyclicDependencies();
    /*!
     * \brief Checks for conditions on functions from a state
     *        with more than one out edge
     * \return first integer for number of errors,
     *         second string for error message
     */
    std::pair<int, std::string> checkFunctionConditions();
    /*!
     * \brief Set the identification number
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
    //! \brief Agent name
    std::string name_;
    //! \brief Agent variables
    boost::ptr_vector<XVariable> variables_;
    //! \brief Agent functions
    boost::ptr_vector<XFunction> functions_;
    //! \brief Agent start state name
    std::string startState_;
    //! \brief Agent end states names
    std::set<std::string> endStates_;
    //! \brief Graph used to hold state graph
    StateGraph stateGraph_;
    //! \brief Graph used to hold dependency graph
    DependencyGraph dependencyGraph_;
};
}}  // namespace flame::model
#endif  // MODEL__XMACHINE_HPP_
