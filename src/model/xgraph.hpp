/*!
 * \file src/model/xgraph.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XGraph: holds graph information
 */
#ifndef MODEL__XGRAPH_HPP_
#define MODEL__XGRAPH_HPP_
#include <boost/graph/adjacency_list.hpp>
#include <boost/property_map/property_map.hpp>
#include <vector>
#include <string>
#include <map>
#include <set>
#include "./dependency.hpp"
#include "./task.hpp"
#include "./xfunction.hpp"
#include "./xvariable.hpp"

namespace flame { namespace model {

/* \brief Define graph type
 *
 * Vectors are used for vertex and edge containers.
 * Bidirectional graph used for access to boost::in_edges
 * as well as boost::out_edges.
 */
typedef boost::adjacency_list
        <boost::vecS, boost::vecS, boost::bidirectionalS> Graph;
/* \brief Define vertex descriptor type */
typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
/* \brief Define edge descriptor type */
typedef boost::graph_traits<Graph>::edge_descriptor Edge;
/* \brief Define vertex iterator */
typedef boost::graph_traits<Graph>::vertex_iterator VertexIterator;
/* \brief Define edge iterator */
typedef boost::graph_traits<Graph>::edge_iterator EdgeIterator;
/* \brief Define edge mapping */
typedef std::map<Edge, Dependency *> EdgeMap;

class XGraph {
  public:
    XGraph();
    ~XGraph();
    int generateStateGraph(std::vector<XFunction*> functions,
            std::string startState, std::set<std::string> endStates);
    int generateDependencyGraph(std::vector<XVariable*> * variables);
    int checkCyclicDependencies();
    int checkFunctionConditions();
    void generateTaskList(std::vector<Task*> * tasks);
    void setAgentName(std::string agentName);
    void import(XGraph * graph);
    void splitMessageTasks();
    void setTasksImported(bool b);
    std::vector<Task *> * getVertexTaskMap();
    Graph * getGraph() { return graph_; }
    void writeGraphviz(std::string fileName);
#ifdef TESTBUILD
    void testBoostGraphLibrary();
    bool testCompareTaskSets();
#endif

  private:
    Vertex addVertex(Task * t);
    Edge addEdge(Vertex to, Vertex from, std::string name,
            Dependency::DependencyType type);
    Task * generateStateGraphStatesAddStateToGraph(
            std::string name, std::string startState);
    void generateStateGraphStates(XFunction * function, Task * task,
            std::string startState);
    void generateStateGraphVariables(XFunction * function, Task * task);
    Task * generateStateGraphMessagesAddMessageToGraph(std::string name);
    void generateStateGraphMessages(XFunction * function, Task * task);
    void addStartTask(std::vector<XVariable*> * variables);
    void addEndTask();
    void copyWritingAndConditionVerticesFromInEdges(Vertex v, Task * t);
    void addConditionDependenciesAndUpdateLastConditions(Vertex v, Task * t);
    void addReadDependencies(Vertex v, Task * t);
    void addWritingVerticesToList(Vertex v, Task * t);
    void addDataAndConditionDependencies(std::vector<XVariable*> * variables);
    void setStartTask(Task * task);
    void transformConditionalStatesToConditions();
    void contractStateVertices();
    void contractVariableVertices();
    void removeRedundantDependencies();
    void removeStateDependencies();
    bool compareTaskSets(std::set<size_t> a, std::set<size_t> b);
    void AddVariableOutput(std::vector<XVariable*> * variables);
    void contractVertices(Task::TaskType taskType,
            Dependency::DependencyType dependencyType);
    Vertex getVertex(Task * t);
    Task * getTask(Vertex v);
    Dependency * getDependency(Edge e);
    void removeVertex(Vertex v);
    void removeVertices(std::vector<Vertex> * tasks);
    void removeDependency(Edge e);
    Vertex getMessageVertex(std::string name, Task::TaskType type);
    Graph * graph_;
    std::vector<Task *> * vertex2task_;
    EdgeMap * edge2dependency_;
    Task * startTask_;
    std::set<Task *> endTasks_;
    Task * endTask_;
    std::string agentName_;
    bool taskImported_;
};

}}  // namespace flame::model
#endif  // MODEL__MODEL_MANAGER_HPP_
