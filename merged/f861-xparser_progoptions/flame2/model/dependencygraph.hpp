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

namespace flame { namespace model {

/* \brief Define graph type
 *
 * Vectors are used for vertex and edge containers.
 * Bidirectional graph used for access to boost::in_edges
 * as well as boost::out_edges.
 */
typedef boost::adjacency_list
        <boost::vecS, boost::vecS, boost::bidirectionalS> Graph;
//! \brief Define vertex descriptor type
typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
//! \brief Define edge descriptor type
typedef boost::graph_traits<Graph>::edge_descriptor Edge;
//! \brief Define vertex iterator
typedef boost::graph_traits<Graph>::vertex_iterator VertexIterator;
//! \brief Define edge iterator
typedef boost::graph_traits<Graph>::edge_iterator EdgeIterator;
//! \brief Define edge mapping
typedef std::map<Edge, Dependency *> EdgeMap;

//! Use a shared pointer to automatically handle Task pointers
typedef boost::shared_ptr<Task> TaskPtr;

typedef std::set< std::pair<std::string, std::string> > StringPairSet;
typedef std::set<std::string> StringSet;
typedef size_t TaskId;
typedef std::set<TaskId> TaskIdSet;
typedef std::map<TaskId, TaskId> TaskIdMap;

class DependencyGraph {
  public:
    DependencyGraph();
    ~DependencyGraph();
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
    void generateTaskList(std::vector<Task*> * tasks);
    void setName(std::string name);
    void import(DependencyGraph * graph);
    std::vector<TaskPtr> * getVertexTaskMap();
    Graph * getGraph() { return graph_; }
    void writeGraphviz(const std::string& fileName) const;
    void importGraphs(std::set<DependencyGraph*> graphs);
    void importStateGraph(StateGraph * stateGraph);
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
    Graph * graph_;
    /*! \brief Ptr to vertex task so that mappings can be swapped */
    std::vector<TaskPtr> * vertex2task_;
    EdgeMap * edge2dependency_;
    Task * startTask_;
    std::set<Task *> endTasks_;
    Task * endTask_;
    std::string name_;

    Vertex getMessageVertex(std::string name, Task::TaskType type);
    void changeMessageTasksToSync();
    void addMessageClearTasks();
    int registerAllowAccess(flame::exe::Task * task,
            std::set<std::string> vars, bool writeable);
    int registerAllowMessage(flame::exe::Task * task,
            std::set<std::string> messages, bool post);
    Vertex addVertex(Task * t);
    Vertex addVertex(TaskPtr ptr);
    Edge addEdge(Vertex to, Vertex from, std::string name,
            Dependency::DependencyType type);
    void addStartTask(boost::ptr_vector<XVariable> * variables);
    void addEndTask();
    void copyWritingAndReadingVerticesFromInEdges(Vertex v, Task * t);
    void addConditionDependenciesAndUpdateLastConditions(Vertex v, Task * t);
    void addWriteDependencies(Vertex v, Task * t);
    void addReadDependencies(Vertex v, Task * t);
    void addWritingVerticesToList(Vertex v, Task * t);
    void addDataDependencies(boost::ptr_vector<XVariable> * variables);
    void setStartTask(Task * task);
    void transformConditionalStatesToConditions(
            boost::ptr_vector<XVariable> * variables);
    void contractStateVertices();
    void contractVariableVertices();
    void removeRedundantDependencies();
    void removeStateDependencies();
    bool compareTaskSets(std::set<size_t> a, std::set<size_t> b);
    void AddVariableOutput();
    void contractVertices(Task::TaskType taskType,
            Dependency::DependencyType dependencyType);
    Vertex getVertex(Task * t);
    Task * getTask(Vertex v) const;
    Dependency * getDependency(Edge e);
    void removeVertex(Vertex v);
    void removeVertices(std::vector<Vertex> * tasks);
    void removeDependency(Edge e);
};

}}  // namespace flame::model
#endif  // MODEL__DEPENDENCYGRAPH_HPP_
