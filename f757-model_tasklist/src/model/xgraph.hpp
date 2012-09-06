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
#include "./dependency.hpp"

namespace flame { namespace model {

class XFunction;
class XVariable;
class Task;

// Define graph type
// (bidirectional for access to boost::in_edges as well as boost::out_edges)
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS> Graph;
// Define vertex and edge descriptor types
typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
typedef boost::graph_traits<Graph>::edge_descriptor Edge;
// Define vertex iterator
typedef boost::graph_traits<Graph>::vertex_iterator VertexIterator;
// Define vertex and edge mappings
typedef std::map<Vertex, Task *> VertexMap;
typedef std::map<Edge, Dependency *> EdgeMap;

class XGraph {
  public:
    XGraph() {}
    ~XGraph();
    Vertex addVertex(Task * t);
    void addEdge(Task * to, Task * from, Dependency * d);
    Edge addEdge(Vertex to, Vertex from, Dependency * d);
    int check_dependency_loops();
    int check_function_conditions();
    void write_graphviz(std::string fileName);
    void add_variable_verticies_to_graph(std::vector<XVariable*> * variables);
    void setStartVector(Vertex sv);
    void add_condition_vertices_to_graph();
    void add_function_task_to_graph(XFunction * function);
#ifdef TESTBUILD
    Graph * getGraph() { return &graph_; }
#endif

  private:
    Vertex getVertex(Task * t);
    Task * getTask(Vertex v);
    Dependency * getDependency(Edge e);
    void discover_last_variable_writes(XVariable * variable,
            Vertex vertex,
            std::set<Vertex> * finished,
            std::set<Vertex> * writing);
    Graph graph_;
    VertexMap vertex2task_;
    EdgeMap edge2dependency_;
    Vertex startVertex_;
};

}}  // namespace flame::model
#endif  // MODEL__MODEL_MANAGER_HPP_
