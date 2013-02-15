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
typedef size_t TaskId;
typedef std::set<TaskId> TaskIdSet;
typedef std::map<TaskId, TaskId> TaskIdMap;

class XModel {
  public:
    XModel();
    void print();
    int validate();
    void setPath(std::string path);
    std::string getPath();
    void setName(std::string name);
    std::string getName();
    bool addIncludedModel(std::string name);
    std::vector<std::string> * getIncludedModels();
    XVariable * addConstant();
    boost::ptr_vector<XVariable> * getConstants();
    XADT * addADT();
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
    TaskIdSet getAgentTasks() const;
    TaskIdSet getAgentIOTasks() const;
    TaskId getInitIOTask() const;
    TaskId getFinIOTask() const;
    TaskIdSet getMessageBoardSyncTasks() const;
    TaskIdSet getMessageBoardClearTasks() const;
    TaskIdMap getTaskDependencies() const;
    std::string getTaskName(TaskId id) const;
    std::string getTaskAgentName(TaskId id) const;
    std::string getTaskFunctionName(TaskId id) const;
    StringSet getTaskReadOnlyVariables(TaskId id) const;
    StringSet getTaskWriteVariables(TaskId id) const;
    StringSet getTaskOutputMessages(TaskId id) const;
    StringSet getTaskInputMessages(TaskId id) const;

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
