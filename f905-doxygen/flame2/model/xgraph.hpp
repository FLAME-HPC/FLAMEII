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

/*!
 * \brief Define graph type
 *
 * Vectors are used for vertex and edge containers.
 * Bidirectional graph used for access to boost::in_edges
 * as well as boost::out_edges.
 */
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS>
          Graph;
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
//! \brief Define out edge iterator
typedef boost::graph_traits<Graph>::out_edge_iterator OutEdgeIterator;
//! \brief Define in edge iterator
typedef boost::graph_traits<Graph>::in_edge_iterator InEdgeIterator;

/*!
 * \brief Class to hold a boost graph with associated methods
 */
class XGraph {
  public:
    /*!
     * \brief Constructor
     */
    XGraph();
    /*!
     * \brief Destructor
     */
    ~XGraph();
    /*!
     * \brief Add vertex given associated task
     * \param[in] t The task
     * \return The vertex
     */
    Vertex addVertex(ModelTask * t);
    /*!
     * \brief Add vertex given associated task pointer object
     * \param[in] ptr The task pointer
     * \return The vertex added
     *
     * Required when importing graphs to model graph
     */
    Vertex addVertex(ModelTaskPtr ptr);
    /*!
     * \brief Add edge
     * \param[in] to Target vertex
     * \param[in] from Source vertex
     * \param[in] name Dependency name
     * \param[in] type Dependency type
     * \return The edge
     */
    Edge addEdge(Vertex to, Vertex from, std::string name,
      Dependency::DependencyType type);
    /*!
     * \brief Remove dependency
     * \param[in] e The edge
     */
    void removeDependency(Edge e);
    /*!
     * \brief Get vertex given its task
     * \param[in] t The task
     * \return The vertex
     */
    Vertex getVertex(ModelTask * t);
    /*!
     * \brief Get task given its vertex
     * \param[in] v The vertex
     * \return The task
     */
    ModelTask * getTask(Vertex v) const;
    /*!
     * \brief Get dependency given edge
     * \param[in] e The edge
     * \return The dependency
     */
    Dependency * getDependency(Edge e);
    /*!
     * \brief Remove a vertex
     * \param v The vertex to remove
     */
    void removeVertex(Vertex v);
    /*!
     * \brief Remove vertices given in vector of tasks
     * \param[in] tasks The vector of tasks
     */
    void removeVertices(std::vector<Vertex> * tasks);
    /*!
     * \brief Get vertex source of edge
     * \param[in] e The edge
     * \return The vertex source of the edge
     */
    Vertex getEdgeSource(Edge e) const;
    /*!
     * \brief Get vertex target of edge
     * \param[in] e The edge
     * \return The vertex target of the edge
     */
    Vertex getEdgeTarget(Edge e) const;
    /*!
     * \brief Get edges of the graph via iterators
     * \return Edge iterators to iterate though all edges
     */
    std::pair<EdgeIterator, EdgeIterator> getEdges() const;
    /*!
     * \brief Get vertices of the graph via iterators
     * \return Vertex iterators to iterate through all vertices
     */
    std::pair<VertexIterator, VertexIterator> getVertices() const;
    /*!
     * \brief Get the number of outgoing edges from a vertex
     * \param[in] v The vertex
     * \return Number of outgoing edges from the vertex
     */
    int getVertexOutDegree(Vertex v) const;
    /*!
     * \brief Get the in edges of a vertex
     * \param[in] v The vertex
     * \return In edge iterators to iterate though all in edges of the vertex
     */
    std::pair<InEdgeIterator, InEdgeIterator> getVertexInEdges(Vertex v) const;
    /*!
     * \brief Get the out edges of a vertex
     * \param[in] v The vertex
     * \return Out edge iterators to iterate though all out edges of the vertex
     */
    std::pair<OutEdgeIterator, OutEdgeIterator> getVertexOutEdges(
        Vertex v) const;
    /*!
     * \brief Remove redundant edges
     */
    void removeRedundantDependencies();
    /*!
     * \brief Checks for cyclic dependencies within a graph
     * \return first integer for number of errors,
     *         second string for error message
     */
    std::pair<int, std::string> checkCyclicDependencies();
    /*!
     * \brief Write graph out to a dot file
     */
    void writeGraphviz(const std::string& fileName) const;
    /*!
     * \brief Check if an edge exists between two vertices
     * \param[in] v1 The first vertex
     * \param[in] v2 The second vertex
     * \return The boolean result
     */
    bool edgeExists(Vertex v1, Vertex v2) const;
    /*!
     * \brief Get a list of vertices sorted by topology
     * \return Vector of vertices in topological order
     */
    std::vector<Vertex> getTopologicalSortedVertices();
    /*!
     * \brief Set the start task
     * \param[in] task The task to set as the start task
     */
    void setStartTask(ModelTask * task);
    /*!
     * \brief Get the start task
     * \return The start task
     */
    ModelTask * getStartTask();
    /*!
     * \brief Set the end task
     * \param[in] task The end task
     */
    void setEndTask(ModelTask * task);
    /*!
     * \brief Get the end task
     * \return The end task
     */
    ModelTask * getEndTask();
    /*!
     * \brief Add an end task
     * \param[in] task A task
     */
    void addEndTask(ModelTask * task);
    /*!
     * \brief Get the set of end tasks
     * \return Set of end tasks
     */
    std::set<ModelTask*> * getEndTasks();
    /*!
     * \brief Get the map between edges and dependencies
     * \return Map between edges and dependencies
     */
    EdgeMap * getEdgeDependencyMap();
    /*!
     * \brief Get a list of tasks
     * \return List of tasks
     */
    const TaskList * getTaskList() const;
    /*!
     * \brief Get the number of tasks
     * \return Length of the task list
     */
    size_t getTaskCount();

  private:
    //! \brief Pointer to a graph so that graphs can be swapped
    Graph * graph_;
    //! \brief List of tasks
    TaskList tasklist_;
    //! \brief Map between edges and dependencies
    EdgeMap * edge2dependency_;
    //! \brief Start task
    ModelTask * startTask_;
    //! \brief End task
    ModelTask * endTask_;
    //! \brief Set of end tasks
    std::set<ModelTask*> endTasks_;
};

}}  // namespace flame::model
#endif  // MODEL__GRAPH_HPP_
