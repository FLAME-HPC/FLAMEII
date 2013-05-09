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
#include "xadt.hpp"
#include "xtimeunit.hpp"
#include "xmessage.hpp"
#include "xmodel_validate.hpp"
#include "stategraph.hpp"
#include "dependencygraph.hpp"

namespace flame { namespace model {

typedef std::pair<std::string, std::string> Var;
typedef std::map<std::string, std::vector<Var> > AgentMemory;
typedef std::set< std::pair<std::string, std::string> > StringPairSet;
typedef std::set<std::string> StringSet;

class XModel {
  public:
    XModel();
    /*!
     * \brief Print a model to stdout
     *
     * Print a whole model out to standard out.
     */
    void print();
    int validate();
    void setPath(std::string path);
    std::string getPath();
    void setName(std::string name);
    std::string getName();
    /*!
     * \brief Adds included model to a list
     * \param[in] path Path of the sub model
     * \return Boolean, true if name is unique
     * If the model file is not already in the list then it is added.
     * If not then false is returned.
     */
    bool addIncludedModel(std::string name);
    std::vector<std::string> * getIncludedModels();
    XVariable * addConstant();
    boost::ptr_vector<XVariable> * getConstants();
    XADT * addADT();
    /*!
     * \brief Returns an adt object with given name
     * \param[in] name Name of the adt
     * \return Pointer to the adt object or 0 if not found
     * This function is used to validate adt names and provide a
     * pointer to the object if valid.
     */
    XADT * getADT(std::string name);
    boost::ptr_vector<XADT> * getADTs();
    void addTimeUnit(XTimeUnit * timeUnit);
    boost::ptr_vector<XTimeUnit> * getTimeUnits();
    void addFunctionFile(std::string file);
    std::vector<std::string> * getFunctionFiles();
    XMachine * addAgent(std::string name);
    boost::ptr_vector<XMachine> * getAgents();
    XMachine * getAgent(std::string name);
    XMessage * addMessage();
    XMessage * addMessage(std::string name);
    /*!
     * \brief Returns a message object with given name
     * \param[in] name Name of the message
     * \return Pointer to the message object or 0 if not found
     * This function is used to validate message names and provide a
     * pointer to the object if valid.
     */
    XMessage * getMessage(std::string name);
    boost::ptr_vector<XMessage> * getMessages();
    void addAllowedDataType(std::string name);
    std::vector<std::string> * getAllowedDataTypes();

    bool doesAgentExist(std::string agent_name) const;
    std::string getAgentVariableType(
        std::string agent_name, std::string var_name) const;
    StringSet getAgentNames() const;
    StringPairSet getAgentVariables(std::string agent_name) const;

    AgentMemory getAgentMemoryInfo() const;
    TaskIdMap getTaskDependencies() const;
    const TaskList * getTaskList() const;

    void outputStateGraph(const std::string& file_name) const;
    void outputDependencyGraph(const std::string& file_name) const;

    int generateStateGraph();
    int checkCyclicDependencies();
#ifdef TESTBUILD
    DependencyGraph * getGraph() { return &dependencyGraph_; }
#endif

  private:
    void generateGraph();
    std::string name_;
    //! \brief The absolute path to the model file
    std::string path_;
    std::vector<std::string> includedModels_;
    std::vector<std::string> functionFiles_;
    //! \brief A list of allowed data types to check variables
    std::vector<std::string> allowedDataTypes_;
    boost::ptr_vector<XVariable> constants_;
    boost::ptr_vector<XADT> adts_;
    boost::ptr_vector<XTimeUnit> timeUnits_;
    boost::ptr_vector<XMachine> agents_;
    boost::ptr_vector<XMessage> messages_;
    //! \brief The model dependency graph
    DependencyGraph dependencyGraph_;
    //! \brief The model state graph
    StateGraph stateGraph_;
};
}}  // namespace flame::model
#endif  // MODEL__XMODEL_HPP_
