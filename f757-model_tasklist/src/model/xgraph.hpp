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

namespace flame { namespace model {

class XFunction;
class XVariable;

// Define graph type
// Vectors used for vertex and edge containers
// Bidirectional for access to boost::in_edges as well as boost::out_edges
// Index type used to index vertices
typedef boost::adjacency_list
        <boost::vecS, boost::vecS, boost::bidirectionalS> Graph;
// Define vertex and edge descriptor types
typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
typedef boost::graph_traits<Graph>::edge_descriptor Edge;
// Define vertex and edge iterators
typedef boost::graph_traits<Graph>::vertex_iterator VertexIterator;
typedef boost::graph_traits<Graph>::edge_iterator EdgeIterator;
// Define vertex and edge mappings
// typedef std::map<Vertex, Task *> VertexMap;
typedef std::map<Edge, Dependency *> EdgeMap;

class XGraph {
  public:
    XGraph();
    ~XGraph();
    int generateStateGraph(std::vector<XFunction*> functions,
            std::string startState);
    int generateDependencyGraph(std::vector<XVariable*> * variables);
    int checkCyclicDependencies();
    int checkFunctionConditions();
#ifdef TESTBUILD
    Graph * getGraph() { return graph_; }
    void testBoostGraphLibrary();
#endif

  private:
    Vertex addVertex(Task * t);
    void addEdge(Task * to, Task * from, Dependency * d);
    void addEdge(Task * to, Task * from, Dependency::DependencyType type);
    Edge addEdge(Vertex to, Vertex from, Dependency * d);
    void write_graphviz(std::string fileName);
    void add_variable_vertices_to_graph(std::vector<XVariable*> * variables);
    void setStartVector(Vertex sv);
    void addConditionVertices();
    void contractStateVertices();
    void contract_variable_vertices_from_graph();
    void remove_redendant_dependencies();
    void remove_state_dependencies();
    void add_condition_dependencies();
    void AddVariableOutput(std::vector<XVariable*> * variables);
    void contractVertices(Task::TaskType taskType,
            Dependency::DependencyType dependencyType);
    Task * addStateToGraph(std::string name, std::string startState);
    Task * addMessageToGraph(std::string name);
    Vertex getVertex(Task * t);
    Task * getTask(Vertex v);
    Dependency * getDependency(Edge e);
    void removeTask(Vertex v);
    void removeTasks(std::vector<Vertex> * tasks);
    void removeDependency(Edge e);
    void discover_conditions(Vertex vertex, Vertex current,
            std::set<Vertex> * conditions);
    void discover_last_variable_writes(std::string variable,
            Vertex vertex,
            std::set<Vertex> * finished,
            std::set<Vertex> * writing);
    Graph * graph_;
    std::vector<Task *> * vertex2task_;
    EdgeMap * edge2dependency_;
    Vertex startVertex_;
};

}}  // namespace flame::model
#endif  // MODEL__MODEL_MANAGER_HPP_