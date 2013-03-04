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
//! \brief Define out edge iterator
typedef boost::graph_traits<Graph>::out_edge_iterator OutEdgeIterator;
//! \brief Define in edge iterator
typedef boost::graph_traits<Graph>::in_edge_iterator InEdgeIterator;

typedef boost::property_map<Graph, boost::vertex_index_t>::const_type
      VertexIndexMap;

typedef std::set< std::pair<std::string, std::string> > StringPairSet;
typedef std::set<std::string> StringSet;
typedef size_t TaskId;
typedef std::set<TaskId> TaskIdSet;
typedef std::map<TaskId, TaskId> TaskIdMap;

class XGraph {
  public:
    XGraph();
    ~XGraph();
    Vertex addVertex(Task * t);
    //! Required when importing graphs to model graph
    Vertex addVertex(TaskPtr ptr);
    Edge addEdge(Vertex to, Vertex from, std::string name,
      Dependency::DependencyType type);
    void removeDependency(Edge e);
    Vertex getVertex(Task * t);
    Task * getTask(Vertex v) const;
    Dependency * getDependency(Edge e);
    void removeVertex(Vertex v);
    void removeVertices(std::vector<Vertex> * tasks);
    Vertex getEdgeSource(Edge e) const;
    Vertex getEdgeTarget(Edge e) const;
    std::pair<EdgeIterator, EdgeIterator> getEdges() const;
    std::pair<VertexIterator, VertexIterator> getVertices() const;
    int getVertexOutDegree(Vertex v) const;
    std::pair<InEdgeIterator, InEdgeIterator> getVertexInEdges(Vertex v) const;
    std::pair<OutEdgeIterator, OutEdgeIterator> getVertexOutEdges(Vertex v) const;
    void removeRedundantDependencies();
    std::pair<int, std::string> checkCyclicDependencies();
    void writeGraphviz(const std::string& fileName) const;
    bool edgeExists(Vertex v1, Vertex v2) const;
    std::vector<Vertex> getTopologicalSortedVertices();
    void setStartTask(Task * task);
    Task * getStartTask();
    void setEndTask(Task * task);
    Task * getEndTask();
    void addEndTask(Task * task);
    std::set<Task*> * getEndTasks();
    std::vector<TaskPtr> * getVertexTaskMap();
    EdgeMap * getEdgeDependencyMap();

    const TaskList * getTaskList() const;

  private:
    /*! \brief Ptr to a graph so that graphs can be swapped */
    Graph * graph_;
    TaskList tasklist_;
    EdgeMap * edge2dependency_;
    Task * startTask_;
    Task * endTask_;
    std::set<Task*> endTasks_;
};

}}  // namespace flame::model
#endif  // MODEL__GRAPH_HPP_
