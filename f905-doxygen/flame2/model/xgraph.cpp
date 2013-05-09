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
  edge2dependency_ = new EdgeMap;
  endTask_ = 0;
  startTask_ = 0;
}

XGraph::~XGraph() {
  EdgeMap::iterator eit;
  // Free edge dependency mapping
  for (eit = edge2dependency_->begin();
      eit != edge2dependency_->end(); ++eit)
    delete (*eit).second;
  delete edge2dependency_;
  // Free graph
  delete graph_;
}

Vertex XGraph::addVertex(ModelTask * t) {
  // create new shared ptr for Task pointer
  ModelTaskPtr ptr(t);
  // Return vertex
  return addVertex(ptr);
}

Vertex XGraph::addVertex(ModelTaskPtr ptr) {
  // Add vertex to graph
  Vertex v = add_vertex(*graph_);
  // Add task to vertex task mapping
  tasklist_.addTask(ptr);
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
  tasklist_.removeTask(v);
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

Vertex XGraph::getVertex(ModelTask * t) {
  return tasklist_.getIndex(t);
}

ModelTask * XGraph::getTask(Vertex v) const {
  return tasklist_.getTask(v);
}

Dependency * XGraph::getDependency(Edge e) {
  Dependency * d = edge2dependency_->find(e)->second;

  if (d == 0) throw flame::exceptions::flame_model_exception(
      "Model graph dependency not initialised");

  // Return dependency associated with the edge
  return d;
}

Vertex XGraph::getEdgeSource(Edge e) const {
  return boost::source(e, *graph_);
}

Vertex XGraph::getEdgeTarget(Edge e) const {
  return boost::target(e, *graph_);
}

std::pair<EdgeIterator, EdgeIterator> XGraph::getEdges() const {
  return boost::edges(*graph_);
}

std::pair<VertexIterator, VertexIterator> XGraph::getVertices() const {
  return boost::vertices(*graph_);
}

int XGraph::getVertexOutDegree(Vertex v) const {
  return boost::out_degree(v, *graph_);
}

std::pair<InEdgeIterator, InEdgeIterator> XGraph::getVertexInEdges(
    Vertex v) const {
  return boost::in_edges(v, *graph_);
}

std::pair<OutEdgeIterator, OutEdgeIterator> XGraph::getVertexOutEdges(
    Vertex v) const {
  return boost::out_edges(v, *graph_);
}

void XGraph::removeRedundantDependencies() {
  size_t ii;
  // The resultant transitive reduction graph
  Graph * trgraph = new Graph;
  std::vector<ModelTaskPtr> * trvertex2task =
      new std::vector<ModelTaskPtr>(tasklist_.getTaskCount());

  // Create a map to get a property of a graph, in this case the vertex index
  typedef boost::property_map<Graph, boost::vertex_index_t>::const_type
      VertexIndexMap;
  VertexIndexMap index_map = get(boost::vertex_index, *graph_);
  // A vector of vertices to hold trgraph vertices
  std::vector<Vertex> to_tc_vec(boost::num_vertices(*graph_));
  // Property map iterator
  // Iterator: Vertex *
  // OffsetMap: VertexIndexMap
  // Value type of iterator: Vertex
  // Reference type of iterator: Vertex&
  boost::iterator_property_map<Vertex *, VertexIndexMap, Vertex, Vertex&>
  g_to_tc_map(&to_tc_vec[0], index_map);

  boost::transitive_reduction(*graph_, *trgraph, g_to_tc_map, index_map);

  // Create new vertex task mapping for trgraph
  for (ii = 0; ii < boost::num_vertices(*graph_); ++ii)
    trvertex2task->at(to_tc_vec[ii]) = tasklist_.getTaskPtr(ii);

  // Make graph_ point to trgraph
  delete graph_;
  graph_ = trgraph;
  // Make vertex2task_ point to trvertex2task
  tasklist_.replaceTaskVector(trvertex2task);
  // Clear edge2dependency_ as edges no longer valid
  EdgeMap::iterator eit;
  for (eit = edge2dependency_->begin(); eit != edge2dependency_->end(); ++eit)
    delete ((*eit).second);
  edge2dependency_->clear();
}

/*!
 \brief Has cycle exception struct

 Has cycle exception used by cycle detector and passes
 back edge to already discovered vertex.
 */
struct has_cycle : public std::exception {
    explicit has_cycle(Edge d) : d_(d) {}
    const Edge edge() const throw() {
      return d_;
    }

  protected:
    Edge d_;
};

/*!
 \brief Visitor function cycle detector struct

 Visitor function object passed to depth_first_search.
 Contains back_edge method that is called when the depth_first_search
 explores an edge to an already discovered vertex.
 */
struct cycle_detector : public boost::default_dfs_visitor {
    void back_edge(Edge edge_t, const Graph &) {
      throw has_cycle(edge_t);
    }
};

std::pair<int, std::string> XGraph::checkCyclicDependencies() {
  // error message
  std::string error_msg;
  // visitor cycle detector for use with depth_first_search
  cycle_detector vis;

  try {
    // Depth first search applied to graph
    boost::depth_first_search(*graph_, visitor(vis));
    // If cyclic dependency is caught
  } catch(const has_cycle& err) {
    // Find associated dependency
    Dependency * d = getDependency(err.edge());
    // Find associated tasks
    ModelTask * t1 = getTask(boost::source(err.edge(), *graph_));
    ModelTask * t2 = getTask(boost::target(err.edge(), *graph_));
    error_msg.append("Error: cycle detected ");
    error_msg.append(t1->getName());
    error_msg.append(" -> ");
    error_msg.append(d->getName());
    error_msg.append(" -> ");
    error_msg.append(t2->getName());
    error_msg.append("\n");
    return std::make_pair(1, error_msg);
  }

  return std::make_pair(0, error_msg);
}


struct vertex_label_writer {
    explicit vertex_label_writer(
        const TaskList &tasklist) : tasklist_(tasklist) {}
    void operator()(std::ostream& out, const Vertex& v) const {
      ModelTask * t = tasklist_.getTask(v);
      if (t->getTaskType() == ModelTask::io_pop_write) {
        out << " [label=\"";
        std::set<std::string>::iterator it;
        for (it = t->getWriteVariables()->begin();
            it != t->getWriteVariables()->end(); ++it) {
          out << "" << (*it) << "\\n";
        }
        out << "\" shape=folder, style=filled, fillcolor=orange]";
      } else {
        out << " [label=\"";
        if (t->getTaskType() == ModelTask::xmessage_sync)
          out << "SYNC: " << t->getName() << "\"";
        else if (t->getTaskType() == ModelTask::xmessage_clear)
          out << "CLEAR: " << t->getName() << "\"";
        else if (t->getTaskType() == ModelTask::start_agent ||
            t->getTaskType() == ModelTask::start_model)
          out << "Start\\n" << t->getParentName() << "\"";
        else if (t->getTaskType() == ModelTask::finish_agent ||
            t->getTaskType() == ModelTask::finish_model)
          out << "Finish\\n" << t->getParentName() << "\"";
        else
          out << t->getName() << "\"";
        if (t->getTaskType() == ModelTask::xfunction)
          out << " shape=rect, style=filled, fillcolor=yellow";
        if (t->getTaskType() == ModelTask::xcondition)
          out << " shape=invhouse, style=filled, fillcolor=yellow";
        if (t->getTaskType() == ModelTask::start_agent ||
            t->getTaskType() == ModelTask::finish_agent ||
            t->getTaskType() == ModelTask::start_model ||
            t->getTaskType() == ModelTask::finish_model)
          out << " shape=ellipse, style=filled, fillcolor=red";
        if (t->getTaskType() == ModelTask::xvariable ||
            t->getTaskType() == ModelTask::xstate)
          out << " shape=ellipse, style=filled, fillcolor=white";
        if (t->getTaskType() == ModelTask::xmessage_clear ||
            t->getTaskType() == ModelTask::xmessage_sync ||
            t->getTaskType() == ModelTask::xmessage) {
          out << " shape=parallelogram, style=filled, ";
          out << "fillcolor=lightblue";
        }
        if (t->getTaskType() == ModelTask::io_pop_write)
          out << " shape=folder, style=filled, fillcolor=orange";
        out << "]";
      }
    }

  protected:
    const TaskList &tasklist_;
};

struct edge_label_writer {
    enum ArrowType { arrowForward = 0, arrowBackward };
    edge_label_writer(EdgeMap * em,
        ArrowType at) :
          edge2dependency(em),
          arrowType(at) {}
    void operator()(std::ostream& out, const Edge& e) const {
      Dependency * d = 0;
      EdgeMap::iterator it = edge2dependency->find(e);
      if (it != edge2dependency->end()) d = (*it).second;
      out << " [";
      if (d) if (d->getDependencyType() != Dependency::blank)
        out << "label=\"" << d->getGraphName() << "\" ";
      if (arrowType == edge_label_writer::arrowBackward) out << "dir = back";
      out << "]";
    }
  protected:
    EdgeMap * edge2dependency;
    ArrowType arrowType;
};

struct graph_writer {
    void operator()(std::ostream& /*out*/) const {
      // out << "node [shape = rect]" << std::endl;
    }
};

/*  stategraph

struct vertex_label_writer {
    explicit vertex_label_writer(std::vector<TaskPtr> * vm) : vertex2task(vm) {}
    void operator()(std::ostream& out, const Vertex& v) const {
      Task * t = vertex2task->at(v).get();
      if (t->getTaskType() == Task::io_pop_write) {
        out << " [label=\"";
        std::set<std::string>::iterator it;
        for (it = t->getWriteVariables()->begin();
            it != t->getWriteVariables()->end(); ++it) {
          out << "" << (*it) << "\\n";
        }
        out << "\" shape=folder, style=filled, fillcolor=orange]";
      } else {
        out << " [label=\"";
        if (t->getTaskType() == Task::xmessage_sync)
          out << "SYNC: " << t->getName() << "\"";
        else if (t->getTaskType() == Task::xmessage_clear)
          out << "CLEAR: " << t->getName() << "\"";
        else if (t->getTaskType() == Task::start_agent ||
            t->getTaskType() == Task::start_model)
          out << "Start\\n" << t->getParentName() << "\"";
        else if (t->getTaskType() == Task::finish_agent ||
            t->getTaskType() == Task::finish_model)
          out << "Finish\\n" << t->getParentName() << "\"";
        else
          out << t->getName() << "\"";
        if (t->getTaskType() == Task::xfunction)
          out << " shape=rect, style=filled, fillcolor=yellow";
        if (t->getTaskType() == Task::xcondition)
          out << " shape=invhouse, style=filled, fillcolor=yellow";
        if (t->getTaskType() == Task::start_agent ||
            t->getTaskType() == Task::finish_agent ||
            t->getTaskType() == Task::start_model ||
            t->getTaskType() == Task::finish_model)
          out << " shape=ellipse, style=filled, fillcolor=red";
        if (t->getTaskType() == Task::xvariable ||
            t->getTaskType() == Task::xstate)
          out << " shape=ellipse, style=filled, fillcolor=white";
        if (t->getTaskType() == Task::xmessage_clear ||
            t->getTaskType() == Task::xmessage_sync ||
            t->getTaskType() == Task::xmessage) {
          out << " shape=parallelogram, style=filled, ";
          out << "fillcolor=lightblue";
        }
        if (t->getTaskType() == Task::io_pop_write)
          out << " shape=folder, style=filled, fillcolor=orange";
        out << "]";
      }
    }

  protected:
    std::vector<TaskPtr> * vertex2task;
};

struct edge_label_writer {
    enum ArrowType { arrowForward = 0, arrowBackward };
    edge_label_writer(EdgeMap * em,
        ArrowType at) :
          edge2dependency(em),
          arrowType(at) {}
    void operator()(std::ostream& out, const Edge& e) const {
      Dependency * d = 0;
      EdgeMap::iterator it = edge2dependency->find(e);
      if (it != edge2dependency->end()) d = (*it).second;
      out << " [";
      if (d) if (d->getDependencyType() != Dependency::blank)
        out << "label=\"" << d->getGraphName() << "\" ";
      if (arrowType == edge_label_writer::arrowBackward) out << "dir = back";
      out << "]";
    }
  protected:
    EdgeMap * edge2dependency;
    ArrowType arrowType;
};
*/

void XGraph::writeGraphviz(const std::string& fileName) const {
  std::fstream graphfile;
  graphfile.open(fileName.c_str(), std::fstream::out);

  boost::write_graphviz(graphfile, *graph_,
      vertex_label_writer(tasklist_),
      edge_label_writer(edge2dependency_,
          edge_label_writer::arrowForward),
          graph_writer());

  graphfile.clear();
}

bool XGraph::edgeExists(Vertex v1, Vertex v2) const {
  std::pair<Edge, bool> p = boost::edge(v1, v2, *graph_);
  return p.second;
}

std::vector<Vertex> XGraph::getTopologicalSortedVertices() {
  std::vector<Vertex> sorted_vertices;
  boost::topological_sort(*graph_, std::back_inserter(sorted_vertices));
  return sorted_vertices;
}

void XGraph::setStartTask(ModelTask * task) {
  startTask_ = task;
}

ModelTask * XGraph::getStartTask() {
  return startTask_;
}

void XGraph::setEndTask(ModelTask * task) {
  endTask_ = task;
}

ModelTask * XGraph::getEndTask() {
  return endTask_;
}

void XGraph::addEndTask(ModelTask * task) {
  endTasks_.insert(task);
}

std::set<ModelTask*> * XGraph::getEndTasks() {
  return &endTasks_;
}

EdgeMap * XGraph::getEdgeDependencyMap() {
  return edge2dependency_;
}

const TaskList * XGraph::getTaskList() const {
  const TaskList * cp = &tasklist_;
  return cp;
}

size_t XGraph::getTaskCount() {
  return tasklist_.getTaskCount();
}

}}   // namespace flame::model
