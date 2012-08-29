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
typedef boost::graph_traits<Graph>::vertex_descriptor vertex_descriptor;
typedef boost::graph_traits<Graph>::edge_descriptor edge_descriptor;
// Define vertex and edge mappings
typedef std::map<vertex_descriptor, Task *> VertexMap;
typedef std::map<edge_descriptor, Dependency *> EdgeMap;

class XGraph {
  public:
    XGraph() {}
    ~XGraph();
    void addVertex(Task * t);
    void addEdge(Task * to, Task * from, Dependency * d);
    int check_dependency_loops();
    void write_graphviz();
    void write_dependency_graph(std::string filename);

  private:
    vertex_descriptor getVertex(Task * t);
    void write_dependency_graph_dependencies(FILE *file);
    Graph graph_;
    VertexMap vertex2task_;
    EdgeMap edge2dependency_;
};

}}  // namespace flame::model
#endif  // MODEL__MODEL_MANAGER_HPP_
