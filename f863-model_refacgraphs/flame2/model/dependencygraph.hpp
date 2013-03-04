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
#include "task.hpp"
#include "xfunction.hpp"
#include "xvariable.hpp"
#include "stategraph.hpp"
#include "xgraph.hpp"

namespace flame { namespace model {

class DependencyGraph {
  public:
    DependencyGraph();
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
    void setName(std::string name);
    void import(DependencyGraph * graph);
    std::vector<TaskPtr> * getVertexTaskMap();
    XGraph * getGraph() { return &graph_; }
    void writeGraphviz(const std::string& fileName) const;
    void importGraphs(std::set<DependencyGraph*> graphs);
    void importStateGraph(StateGraph * stateGraph);
    TaskIdMap getTaskDependencies() const;
    const TaskList * getTaskList() const;
#ifdef TESTBUILD
    bool dependencyExists(std::string name1, std::string name2);
    Vertex addTestVertex(Task * t);
    void addTestEdge(Vertex to, Vertex from, std::string name,
            Dependency::DependencyType type);
    void setTestStartTask(Task * task);
    void addTestEndTask(Task * task);
#endif

  private:
    /*! \brief Ptr to a graph so that graphs can be swapped */
    XGraph graph_;
    std::string name_;

    Vertex getMessageVertex(std::string name, Task::TaskType type);
    void changeMessageTasksToSync();
    void addMessageClearTasks();
    int registerAllowAccess(flame::exe::Task * task,
            std::set<std::string> vars, bool writeable);
    int registerAllowMessage(flame::exe::Task * task,
            std::set<std::string> messages, bool post);
    void addConditionDependenciesAndUpdateLastConditions(Vertex v, Task * t);
    void transformConditionalStatesToConditions(
            boost::ptr_vector<XVariable> * variables);
    void contractStateVertices();
    void removeStateDependencies();
    bool compareTaskSets(std::set<size_t> a, std::set<size_t> b);
    void AddVariableOutput();
    void contractVertices(Task::TaskType taskType,
            Dependency::DependencyType dependencyType);
};

}}  // namespace flame::model
#endif  // MODEL__DEPENDENCYGRAPH_HPP_
