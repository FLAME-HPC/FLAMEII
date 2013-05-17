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

//! \brief Define task id as unsigned integer
typedef size_t TaskId;
//! \brief Define multimap of task ids to hold dependencies
typedef std::multimap<TaskId, TaskId> TaskIdMap;

/*!
 * \brief Class to hold a dependency graph
 */
class DependencyGraph {
  public:
    /*!
     * \brief Constructor
     */
    DependencyGraph();
    /*!
     * \brief Generate dependency graph (from imported state graph)
     * \param[in] variables Agent variables
     */
    void generateDependencyGraph(boost::ptr_vector<XVariable> * variables);
    /*!
     * \brief Checks for cyclic dependencies within a graph
     * \return first integer for number of errors,
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
     * \brief Set the name of the graph (agent or model name)
     * \param[in] name The agent/model name
     */
    void setName(std::string name);
    /*!
     * \brief Import a dependency graph
     * \param[in] graph Dependency graph to import
     */
    void import(DependencyGraph * graph);
    /*!
     * \brief Write graph out to a dot file
     * \param[in] fileName The path to the file to write out to
     */
    void writeGraphviz(const std::string& fileName) const;
    /*!
     * \brief Import a set of dependency graphs.
     * \param[in] graphs Set of dependency graphs to import
     *
     * Used by models to import agent graphs
     */
    void importGraphs(std::set<DependencyGraph*> graphs);
    /*!
     * \brief Import a state graph as a starting point
     * \param[in] stateGraph State graph to import
     */
    void importStateGraph(StateGraph * stateGraph);
    /*!
     * \brief Return all the graph dependencies
     * \return The task dependencies
     */
    TaskIdMap getTaskDependencies() const;
    /*!
     * \brief Return the task list
     * \return The task list
     */
    const TaskList * getTaskList() const;
#ifdef TESTBUILD
    /*!
     * \brief Check to see if a dependencies between tasks exists
     * \param[in] type1 First task
     * \param[in] name1 First task name
     * \param[in] type2 Second task
     * \param[in] name2 Second task name
     * \return Boolean result
     */
    bool dependencyExists(ModelTask::TaskType type1, std::string name1,
        ModelTask::TaskType type2, std::string name2);
    /*!
     * \brief Add a task
     * \param[in] t The task to add
     */
    Vertex addTestVertex(ModelTask * t);
    /*!
     * \brief Add a dependency
     * \param[in] to Source vertex
     * \param[in] from Target vertex
     * \param[in] name Dependency name
     * \param[in] type Dependency type
     */
    void addTestEdge(Vertex to, Vertex from, std::string name,
            Dependency::DependencyType type);
    /*!
     * \brief Set a start task
     * \param[in] task The start task
     */
    void setTestStartTask(ModelTask * task);
    /*!
     * \brief Set an end task
     * \param[in] task The end task
     */
    void addTestEndTask(ModelTask * task);
#endif

  private:
    //! \brief The underlying graph
    XGraph graph_;
    //! \brief The graph name (either model or agent name)
    std::string name_;

    /*!
     * \brief Get message vertex
     * \param[in] name The message name
     * \param[in] type The task type
     * \return The vertex
     */
    Vertex getMessageVertex(std::string name, ModelTask::TaskType type);
    /*!
     * \brief Change message tasks to sync tasks
     */
    void changeMessageTasksToSync();
    /*!
     * \brief Add message clear tasks
     */
    void addMessageClearTasks();
    /*!
     * \brief Transform conditional states to conditions
     * \param[in] variables Agent variables
     *
     * Change state tasks with more than 1 outgoing transition into
     * a condition task
     */
    void transformConditionalStatesToConditions(
            boost::ptr_vector<XVariable> * variables);
    /*!
     * \brief Set start task to not be a state and remove all state tasks
     */
    void contractStateVertices();
    /*!
     * \brief Remove state type dependencies
     */
    void removeStateDependencies();
    /*!
     * \brief Add data dependencies between writing and reading functions for
     *        each variable
     */
    void AddVariableOutput();
    /*!
     * \brief Remove vertex type and replace with edge with given dependency type
     * \param[in] taskType The task type to replace
     * \param[in] dependencyType The dependency type to be the replacement
     */
    void contractVertices(ModelTask::TaskType taskType,
            Dependency::DependencyType dependencyType);
    /*!
     * \brief Find a string from one set in another set
     * \param[in] a Set of strings to search
     * \param[in] find_in_a Set of string to find
     * \return Boolean result
     */
    bool setContains(
        std::set<std::string>* a, std::set<std::string>* find_in_a);
#ifdef GROUP_WRITE_VARS
    /*!
     * \brief Return true if both sets are the same
     */
    bool compareTaskSets(std::set<size_t> a, std::set<size_t> b);
#endif
};

}}  // namespace flame::model
#endif  // MODEL__DEPENDENCYGRAPH_HPP_
