/*!
 * \file flame2/model/dependencygraph.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief DependencyGraph: holds dependency graph
 */
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/transitive_reduction.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/lexical_cast.hpp>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <utility>
#include <exception>
#include <functional>  // For greater<>
#include <algorithm>   // For sort
#include "flame2/config.hpp"
#include "flame2/exe/task_manager.hpp"
#include "flame2/exceptions/model.hpp"
#include "xgraph.hpp"

namespace flame { namespace model {

XGraph::XGraph() {
  // Initialise pointers
  graph_ = new Graph;
  vertex2task_ = new std::vector<TaskPtr>;
  edge2dependency_ = new EdgeMap;
  endTask_ = 0;
  startTask_ = 0;
}

XGraph::~XGraph() {
  EdgeMap::iterator eit;
  // Free vertex task mapping
  delete vertex2task_;
  // Free edge dependency mapping
  for (eit = edge2dependency_->begin();
      eit != edge2dependency_->end(); ++eit)
    delete (*eit).second;
  delete edge2dependency_;
  // Free graph
  delete graph_;
}

Vertex XGraph::addVertex(Task * t) {
  // Add vertex to graph
  Vertex v = add_vertex(*graph_);
  // create new shared ptr for Task pointer
  TaskPtr ptr(t);
  // Add task to vertex task mapping
  vertex2task_->push_back(ptr);
  // Return vertex
  return v;
}

Vertex XGraph::addVertex(TaskPtr ptr) {
  // Add vertex to graph
  Vertex v = add_vertex(*graph_);
  // Add task to vertex task mapping
  vertex2task_->push_back(ptr);
  // Return vertex
  return v;
}

void XGraph::removeVertex(Vertex v) {
  // Iterators
  boost::graph_traits<Graph>::out_edge_iterator oei, oei_end;
  boost::graph_traits<Graph>::in_edge_iterator iei, iei_end;
  std::set<Edge>::iterator eit;
  // Set of edges to remove
  std::set<Edge> edgesToRemove;
  // Add all in and out vertex edges to set of edges to be removed
  for (boost::tie(iei, iei_end) = boost::in_edges(v, *graph_);
      iei != iei_end; ++iei)
    edgesToRemove.insert((Edge)*iei);
  for (boost::tie(oei, oei_end) = boost::out_edges(v, *graph_);
      oei != oei_end; ++oei)
    edgesToRemove.insert((Edge)*oei);
  // Remove all edges in set of edges to be removed
  for (eit = edgesToRemove.begin(); eit != edgesToRemove.end(); ++eit)
    removeDependency(*eit);
  // Remove task from vertex to task mapping mapping
  vertex2task_->erase(vertex2task_->begin() + v);
  // Remove edge from graph
  boost::remove_vertex(v, *graph_);
}

// Tasks removed largest first so that indexes are not changed
void XGraph::removeVertices(std::vector<Vertex> * tasks) {
  std::vector<Vertex>::iterator vit;
  // Sort vertices largest first
  std::sort(tasks->begin(), tasks->end(), std::greater<size_t>());
  // Remove tasks in order
  for (vit = tasks->begin(); vit != tasks->end(); ++vit)
    removeVertex((*vit));
}

void XGraph::removeDependency(Edge e) {
  // Find dependency
  EdgeMap::iterator it = edge2dependency_->find(e);
  // If found then free
  if (it != edge2dependency_->end()) {
    delete (*it).second;
    edge2dependency_->erase(it);
  }
  // Remove edge from graph
  boost::remove_edge(e, *graph_);
}

Edge XGraph::addEdge(Vertex to, Vertex from, std::string name,
    Dependency::DependencyType type) {
  // Create dependency from name and type
  Dependency * d = new Dependency(name, type);
  // Add edge to graph
  std::pair<Edge, bool> e = add_edge(to, from, *graph_);
  // Add mapping from edge to dependency
  edge2dependency_->insert(std::make_pair(e.first, d));
  // Return edge
  return e.first;
}

Vertex XGraph::getVertex(Task * t) {
  size_t ii;
  // Find index of task in vertex task mapping
  // The index corresponds to the vertex number
  for (ii = 0; ii < vertex2task_->size(); ++ii)
    if (vertex2task_->at(ii).get() == t) return ii;
  return 0;
}

Task * XGraph::getTask(Vertex v) const {
  int index = static_cast<int>(v);

  if (index < 0 || index >= static_cast<int>(vertex2task_->size()))
    throw flame::exceptions::flame_model_exception(
      "Task id does not exist");

  // Return task at index v
  return vertex2task_->at(v).get();
}

Dependency * XGraph::getDependency(Edge e) {
  Dependency * d = edge2dependency_->find(e)->second;

  if (d == 0) throw flame::exceptions::flame_model_exception(
      "Model graph dependency not initialised");

  // Return dependency associated with the edge
  return d;
}

}}   // namespace flame::model
