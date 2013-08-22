/*!
 * \file flame2/model/xmodel.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XModel: holds model information
 */
#ifndef MODEL__XMODEL_HPP_
#define MODEL__XMODEL_HPP_
#include <boost/ptr_container/ptr_vector.hpp>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <utility>
#include "xmachine.hpp"
#include "xvariable.hpp"
#include "xdatatype.hpp"
#include "xtimeunit.hpp"
#include "xmessage.hpp"
#include "xmodel_validate.hpp"
#include "stategraph.hpp"
#include "dependencygraph.hpp"

namespace flame { namespace model {

//! \brief Type to hold variable type and name
typedef std::pair<std::string, std::string> Var;
//! \brief Type to hold agent name and variables
typedef std::map<std::string, std::vector<Var> > AgentMemory;
//! \brief Type to hold a set of string pairs
typedef std::set< std::pair<std::string, std::string> > StringPairSet;
//! \brief Type to hold a set of strings
typedef std::set<std::string> StringSet;

/*!
 * \brief This class holds model information
 */
class XModel {
  public:
    /*!
     * \brief Constructor
     */
    XModel();
    /*!
     * \brief Print a model to stdout
     *
     * Print a whole model out to standard out.
     */
    void print();
    /*!
     * \brief Validates the model
     * \return Return code which is the number of errors found
     *
     * Validates the model then if valid generates the dependency graph.
     */
    int validate();
    /*!
     * \brief Set the absolute path to the model file
     * \param[in] path The path to the model file
     */
    void setPath(std::string path);
    /*!
     * \brief Get the absolute path to the model file
     * \return The path to the model file
     */
    std::string getPath();
    /*!
     * \brief Set the name of the model
     * \param[in] name The name of the model
     */
    void setName(std::string name);
    /*!
     * \brief Get the name of the model
     * \return The name of the model
     */
    std::string getName();
    /*!
     * \brief Adds included model to a list
     * \param[in] name Path of the sub model
     * \return Boolean, true if name is unique
     * If the model file is not already in the list then it is added.
     * If not then false is returned.
     */
    bool addIncludedModel(std::string name);
    /*!
     * \brief Get the list of included models
     * \return The vector of strings containing path of included models
     */
    std::vector<std::string> * getIncludedModels();
    /*!
     * \brief Add an environment constant
     * \return A pointer to the new XVariable created
     */
    XVariable * addConstant();
    /*!
     * \brief Get the environment constants
     * \return A boost pointer vector of environment constants
     */
    boost::ptr_vector<XVariable> * getConstants();
    /*!
     * \brief Add a user defined abstract data type
     * \return A pointer to the new XADT created
     */
    XDataType * addDataType();
    /*!
     * \brief Returns an adt object with given name
     * \param[in] name Name of the adt
     * \return Pointer to the adt object or 0 if not found
     * This function is used to validate adt names and provide a
     * pointer to the object if valid.
     */
    XDataType * getDataType(std::string name);
    /*!
     * \brief Get the model abstract data types
     * \return Pointer to a boost pointer vector of ADTs
     */
    boost::ptr_vector<XDataType> * getDataTypes();
    /*!
     * \brief Add a time unit
     * \param[in] timeUnit The time unit to add
     */
    void addTimeUnit(XTimeUnit * timeUnit);
    /*!
     * \brief Get the time units
     * \return A pointer to a boost pointer vector of XTimeUnit
     */
    boost::ptr_vector<XTimeUnit> * getTimeUnits();
    /*!
     * \brief Add a function file path
     * \param[in] file The function file path
     */
    void addFunctionFile(std::string file);
    /*!
     * \brief Get the function files paths
     * \return A pointer to a vector of paths
     */
    std::vector<std::string> * getFunctionFiles();
    /*!
     * \brief Add an agent
     * \param[in] name The name of the agent
     * \return Pointer to the newly created XMachine
     */
    XMachine * addAgent(std::string name);
    /*!
     * \brief Get the agents
     * \return A pointer to a boost pointer vector of XMachine
     */
    boost::ptr_vector<XMachine> * getAgents();
    /*!
     * \brief Get an agent via its name
     * \param[in] name The agent name
     * \return A pointer to the XMachine
     */
    XMachine * getAgent(std::string name);
    /*!
     * \brief Add a message
     * \return A pointer to the newly created XMessage
     */
    XMessage * addMessage();
    /*!
     * \brief Add a message using its name
     * \param[in] name The message name
     * \return A pointer to the newly created XMessage
     */
    XMessage * addMessage(std::string name);
    /*!
     * \brief Returns a message object with given name
     * \param[in] name Name of the message
     * \return Pointer to the message object or 0 if not found
     * This function is used to validate message names and provide a
     * pointer to the object if valid.
     */
    XMessage * getMessage(std::string name);
    /*!
     * \brief Get the messages
     * \return A pointer to a boost pointer vector of XMessage
     */
    boost::ptr_vector<XMessage> * getMessages();
    /*!
     * \brief Check to see if an agent exists
     * \param[in] agent_name The agent name to check
     * \return A boolean describing the result
     */
    bool doesAgentExist(std::string agent_name) const;
    /*!
     * \brief Get an agent variable type
     * \param[in] agent_name The agent name
     * \param[in] var_name The variable name
     * \return The variable type
     *
     * Throws exceptions if the agent or variable does not exist
     */
    std::string getAgentVariableType(
        std::string agent_name, std::string var_name) const;
    /*!
     * \brief Get agent names
     * \return A set of agent names
     */
    StringSet getAgentNames() const;
    /*!
     * \brief Get agent variables
     * \param[in] agent_name The agent name
     * \return A set of string pairs for the variable type and name
     */
    StringPairSet getAgentVariables(std::string agent_name) const;
    /*!
     * \brief Get agent memory infor
     * \return The agent memory info
     */
    AgentMemory getAgentMemoryInfo() const;
    /*!
     * \brief Get task dependencies
     * \return mapping between model tasks
     */
    TaskIdMap getTaskDependencies() const;
    /*!
     * \brief Get task list
     * \return The list of tasks
     */
    const TaskList * getTaskList() const;
    /*!
     * \brief Write the state graph to a file
     * \param[in] file_name The file path
     */
    void outputStateGraph(const std::string& file_name) const;
    /*!
     * \brief Write the dependency graph to a file
     * \param[in] file_name The file path
     */
    void outputDependencyGraph(const std::string& file_name) const;
    /*!
     * \brief Generate the state graph
     */
    void generateStateGraph();
#ifdef TESTBUILD
    /*!
     * \brief Get the dependency graph
     * \return The dependency graph
     */
    DependencyGraph * getGraph() { return &dependencyGraph_; }
#endif

  private:
    /*!
     * \brief Generate the model dependency graph
     */
    void generateGraph();
    //! \brief The model name
    std::string name_;
    //! \brief The absolute path to the model file
    std::string path_;
    //! \brief List of paths of included models
    std::vector<std::string> includedModels_;
    //! \brief List of paths of function files
    std::vector<std::string> functionFiles_;
    //! \brief List of environment constants
    boost::ptr_vector<XVariable> constants_;
    //! \brief List of model data types
    boost::ptr_vector<XDataType> datatypes_;
    //! \brief List of time units
    boost::ptr_vector<XTimeUnit> timeUnits_;
    //! \brief List of agents
    boost::ptr_vector<XMachine> agents_;
    //! \brief List of messages
    boost::ptr_vector<XMessage> messages_;
    //! \brief The model dependency graph
    DependencyGraph dependencyGraph_;
    //! \brief The model state graph
    StateGraph stateGraph_;
};
}}  // namespace flame::model
#endif  // MODEL__XMODEL_HPP_
