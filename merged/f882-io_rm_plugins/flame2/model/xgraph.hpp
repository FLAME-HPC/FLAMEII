/*!
 * \file flame2/model/xgraph.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XGraph: used to hold a graph
 */
#ifndef MODEL__GRAPH_HPP_
#define MODEL__GRAPH_HPP_
#include <boost/graph/adjacency_list.hpp>
#include <boost/property_map/property_map.hpp>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <utility>  // for std::pair
#include "dependency.hpp"
#include "task_list.hpp"

namespace flame { namespace model {

/* \brief Define graph type
 *
 * Vectors are used for vertex and edge containers.
 * Bidirectional graph used for access to boost::in_edges
 * as well as boost::out_edges.
 */
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS>
          Graph;
//! Define vertex descriptor type
typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
//! Define edge descriptor type
typedef boost::graph_traits<Graph>::edge_descriptor Edge;
//! Define vertex iterator
typedef boost::graph_traits<Graph>::vertex_iterator VertexIterator;
//! Define edge iterator
typedef boost::graph_traits<Graph>::edge_iterator EdgeIterator;
//! Define edge mapping
typedef std::map<Edge, Dependency *> EdgeMap;
//! Define out edge iterator
typedef boost::graph_traits<Graph>::out_edge_iterator OutEdgeIterator;
//! Define in edge iterator
typedef boost::graph_traits<Graph>::in_edge_iterator InEdgeIterator;

class XGraph {
  public:
    //! Constructor
    XGraph();
    //! Destructor
    ~XGraph();
    //! Add vertex given associated task
    Vertex addVertex(ModelTask * t);
    //! Add vertex given associated task pointer object
    //! (Required when importing graphs to model graph)
    Vertex addVertex(ModelTaskPtr ptr);
    //! Add edge
    //! \param to target vertex
    //! \param from source vertex
    //! \param name dependency name
    //! \param type dependency type
    Edge addEdge(Vertex to, Vertex from, std::string name,
      Dependency::DependencyType type);
    //! Remove dependency
    void removeDependency(Edge e);
    //! \return Vertex given its task
    Vertex getVertex(ModelTask * t);
    //! \return Task given its vertex
    ModelTask * getTask(Vertex v) const;
    //! \return Dependency given edge
    Dependency * getDependency(Edge e);
    //! Remove vertex
    void removeVertex(Vertex v);
    //! Remove vertices given in vector of tasks
    void removeVertices(std::vector<Vertex> * tasks);
    //! \return Vertex source of edge
    Vertex getEdgeSource(Edge e) const;
    //! \return Vertex target of edge
    Vertex getEdgeTarget(Edge e) const;
    //! \return Edge iterators to iterate though all edges
    std::pair<EdgeIterator, EdgeIterator> getEdges() const;
    //! \return Vertex iterators to iterate through all vertices
    std::pair<VertexIterator, VertexIterator> getVertices() const;
    //! \return Number of outgoing edges from a vertex
    int getVertexOutDegree(Vertex v) const;
    //! In edge iterators to iterate though all in edges of a vertex
    std::pair<InEdgeIterator, InEdgeIterator> getVertexInEdges(Vertex v) const;
    //! Out edge iterators to iterate though all out edges of a vertex
    std::pair<OutEdgeIterator, OutEdgeIterator> getVertexOutEdges(
        Vertex v) const;
    //! Remove redundant edges
    void removeRedundantDependencies();
    //! Checks for cyclic dependencies within a graph
    //! \return first integer for number of errors,
    //!         second string for error message
    std::pair<int, std::string> checkCyclicDependencies();
    //! Write graph out to a dot file
    void writeGraphviz(const std::string& fileName) const;
    //! \return If edge exists between the two vertices
    bool edgeExists(Vertex v1, Vertex v2) const;
    //! \return Vector of vertices in topological order
    std::vector<Vertex> getTopologicalSortedVertices();
    //! Set start task
    void setStartTask(ModelTask * task);
    //! \return Start task
    ModelTask * getStartTask();
    //! Set end task
    void setEndTask(ModelTask * task);
    //! \return End task
    ModelTask * getEndTask();
    //! Add an end task
    void addEndTask(ModelTask * task);
    //! \return Set of end tasks
    std::set<ModelTask*> * getEndTasks();
    //! \return Map between edges and dependencies
    EdgeMap * getEdgeDependencyMap();
    //! \return List of tasks
    const TaskList * getTaskList() const;
    //! \return Length of the task list
    size_t getTaskCount();

  private:
    //! Pointer to a graph so that graphs can be swapped
    Graph * graph_;
    //! List of tasks
    TaskList tasklist_;
    //! Map between edges and dependencies
    EdgeMap * edge2dependency_;
    //! Start task
    ModelTask * startTask_;
    //! End task
    ModelTask * endTask_;
    //! Set of end tasks
    std::set<ModelTask*> endTasks_;
};

}}  // namespace flame::model
#endif  // MODEL__GRAPH_HPP_
