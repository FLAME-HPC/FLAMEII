/*!
 * \file flame2/model/dependencygraph.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief DependencyGraph: holds dependency graph
 */
#ifndef MODEL__DEPENDENCYGRAPH_HPP_
#define MODEL__DEPENDENCYGRAPH_HPP_
#include <boost/graph/adjacency_list.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <utility>  // for std::pair
#include "flame2/exe/task_manager.hpp"
#include "dependency.hpp"
#include "model_task.hpp"
#include "xfunction.hpp"
#include "xvariable.hpp"
#include "stategraph.hpp"
#include "xgraph.hpp"

namespace flame { namespace model {

//! Define task id as unsigned integer
typedef size_t TaskId;
//! Define multimap of task ids to hold dependencies
typedef std::multimap<TaskId, TaskId> TaskIdMap;

class DependencyGraph {
  public:
    //! Constructor
    DependencyGraph();
    //! Generate dependency graph (from imported state graph)
    int generateDependencyGraph(boost::ptr_vector<XVariable> * variables);
    //! Checks for cyclic dependencies within a graph
    //! \return first integer for number of errors,
    //!         second string for error message
    std::pair<int, std::string> checkCyclicDependencies();
    //! Checks for conditions on functions from a state
    //! with more than one out edge
    //! \return first integer for number of errors,
    //!         second string for error message
    std::pair<int, std::string> checkFunctionConditions();
    //! Set the name of the graph (agent or model name)
    void setName(std::string name);
    //! Import a dependency graph
    void import(DependencyGraph * graph);
    //! Write graph out to a dot file
    void writeGraphviz(const std::string& fileName) const;
    //! Import a set of dependency graphs.
    //! Used by models to import agent graphs
    void importGraphs(std::set<DependencyGraph*> graphs);
    //! Import a state graph as a starting point
    void importStateGraph(StateGraph * stateGraph);
    //! Return all the graph dependencies
    TaskIdMap getTaskDependencies() const;
    //! Return the task list
    const TaskList * getTaskList() const;
#ifdef TESTBUILD
    //! Check to see if a dependencies between tasks exists
    bool dependencyExists(ModelTask::TaskType type1, std::string name1,
        ModelTask::TaskType type2, std::string name2);
    //! Add a task
    Vertex addTestVertex(ModelTask * t);
    //! Add a dependency
    void addTestEdge(Vertex to, Vertex from, std::string name,
            Dependency::DependencyType type);
    //! Set a start task
    void setTestStartTask(ModelTask * task);
    //! Set an end task
    void addTestEndTask(ModelTask * task);
#endif

  private:
    //! The underlying graph
    XGraph graph_;
    //! The graph name (either model or agent name)
    std::string name_;

    //! Return new message vertex or existing one if found
    Vertex getMessageVertex(std::string name, ModelTask::TaskType type);
    //! Change message tasks to sync tasks
    void changeMessageTasksToSync();
    //! Add message clear tasks
    void addMessageClearTasks();
    //! Change state tasks with more than 1 outgoing transition into
    //! a condition task
    void transformConditionalStatesToConditions(
            boost::ptr_vector<XVariable> * variables);
    //! Set start task to not be a state and remove all state tasks
    void contractStateVertices();
    //! Remove state type dependencies
    void removeStateDependencies();
    //! Add data dependencies between writing and reading functions for
    //! each variable
    void AddVariableOutput();
    //! Remove vertex type and replace with edge with given dependency type
    void contractVertices(ModelTask::TaskType taskType,
            Dependency::DependencyType dependencyType);
#ifdef GROUP_WRITE_VARS
    //! Return true if both sets are the same
    bool compareTaskSets(std::set<size_t> a, std::set<size_t> b);
#endif
};

}}  // namespace flame::model
#endif  // MODEL__DEPENDENCYGRAPH_HPP_
