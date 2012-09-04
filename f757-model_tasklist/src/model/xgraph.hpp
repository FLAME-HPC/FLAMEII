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
#include "./task.hpp"

namespace flame { namespace model {

// Define graph type
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS> Graph;
// Define vertex and edge descriptor types
typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
typedef boost::graph_traits<Graph>::edge_descriptor Edge;
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
    void test_layers();
    void setStartVector(Vertex sv);
    void add_branch_vertices_to_graph();
#ifdef TESTBUILD
    Graph * getGraph() { return &graph_; }
#endif

  private:
    Vertex getVertex(Task * t);
    Task * getTask(Vertex v);
    Dependency * getDependency(Edge e);
    Graph graph_;
    VertexMap vertex2task_;
    EdgeMap edge2dependency_;
    Vertex startVertex_;
};

}}  // namespace flame::model
#endif  // MODEL__MODEL_MANAGER_HPP_
