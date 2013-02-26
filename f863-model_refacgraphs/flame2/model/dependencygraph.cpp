/*!
 * \file flame2/model/dependencygraph.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief DependencyGraph: holds dependency graph
 */
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
#include "dependencygraph.hpp"
#include "xcondition.hpp"
#include "xfunction.hpp"
#include "task.hpp"
#include "data_dependency_analyser.hpp"

namespace flame { namespace model {

DependencyGraph::DependencyGraph() {
  // Initialise pointers
  graph_ = new XGraph;
}

DependencyGraph::~DependencyGraph() {
  // Free graph
  delete graph_;
}

std::vector<TaskPtr> * DependencyGraph::getVertexTaskMap() {
  return graph_->vertex2task_;
}

void DependencyGraph::setName(std::string name) {
  graph_->name_ = name;
}

int DependencyGraph::generateDependencyGraph(
    boost::ptr_vector<XVariable> * variables) {
#ifdef OUTPUT_GRAPHS
  writeGraphviz(name_ + "_1.dot");
#endif
  // Transform conditional states (more than one out edge)
  // into a condition task
  transformConditionalStatesToConditions(variables);
  // Contract state vertices
  contractStateVertices();
#ifdef OUTPUT_GRAPHS
  writeGraphviz(name_ + "_2.dot");
#endif
  // Add data and condition dependencies
  DataDependencyAnalyser dda = DataDependencyAnalyser(graph_);
  dda.addDataDependencies(variables);
  // Remove state dependencies
  removeStateDependencies();
#ifdef OUTPUT_GRAPHS
  writeGraphviz(name_ + "_3.dot");
#endif
  // Add data output tasks
  AddVariableOutput();
#ifdef USE_VARIABLE_VERTICES
  // Contract variable vertices
  contractVariableVertices();
#endif

#ifdef OUTPUT_GRAPHS
  writeGraphviz(name_ + "_4.dot");
#endif
  // Remove redundant dependencies
  removeRedundantDependencies();
#ifdef OUTPUT_GRAPHS
  writeGraphviz(name_ + "_5.dot");
#endif
  return 0;
}

void DependencyGraph::addConditionDependenciesAndUpdateLastConditions(
    Vertex v, Task * t) {
  std::set<size_t>::iterator it;

  // Add edge for each condition vertex found
  for (it = t->getLastConditions()->begin();
      it != t->getLastConditions()->end(); ++it)
    graph_->addEdge(*it, v, "Condition", Dependency::condition);
  // If condition
  if (t->getTaskType() == Task::xcondition) {
    // Clear last conditions
    t->getLastConditions()->clear();
    // Add current condition
    t->getLastConditions()->insert(v);
  }
}

bool setContains(std::set<std::string>* a, std::set<std::string>* find_in_a) {
  std::set<std::string>::iterator fit, fit2;

  // For each string to find in 'a'
  for (fit = find_in_a->begin(); fit != find_in_a->end(); ++fit) {
    // Try and find the string
    fit2 = a->find((*fit));
    // If found then return true
    if (fit2 != a->end()) return true;
  }

  return false;
}

bool DependencyGraph::compareTaskSets(std::set<size_t> a, std::set<size_t> b) {
  std::set<size_t>::iterator a_it, b_it;
  // Compare size first
  if (a.size() != b.size()) return false;
  // Compare elements
  for (a_it = a.begin(), b_it = b.begin();
      a_it != a.end(), b_it != b.end();
      ++a_it, ++b_it) {
    if ((*a_it) != (*b_it)) return false;
  }

  return true;
}

std::string concatStringSet(std::set<std::string>* sset) {
  std::string str;
  std::set<std::string>::iterator it;

  for (it = sset->begin(); it != sset->end();) {
    str.append((*it));
    if (it++ != sset->end()) str.append(" ");
  }

  return str;
}

void DependencyGraph::AddVariableOutput() {
  // For each function that last writes a variable add dependency
  // to the data output of that variable
  VarMapToVertices::iterator vwit;
  std::set<size_t>::iterator sit;
  VarMapToVertices * lws = graph_->endTask_->getLastWrites();
  size_t count = 0;

  while (!lws->empty()) {
    // Create new io write task
    Task * task = new Task(graph_->name_,
        boost::lexical_cast<std::string>(++count), Task::io_pop_write);
    Vertex vertex = graph_->addVertex(task);
    task->addWriteVariable((*lws->begin()).first);
    // Check first var against other var task sets, if same then add to
    // current task and remove
    for (vwit = ++lws->begin(); vwit != lws->end();) {
      if (compareTaskSets((*lws->begin()).second, (*vwit).second)) {
        task->addWriteVariable((*vwit).first);
        lws->erase(vwit++);
      } else {
        ++vwit;
      }
    }
    // Set name as names of variables
    // task->setName(concatStringSet(task->getWriteVariables()));
    task->setName((*lws->begin()).first);
    // Add edges from each writing vector to task
    for (sit = (*lws->begin()).second.begin();
        sit != (*lws->begin()).second.end(); ++sit)
      graph_->addEdge((*sit), vertex, "", Dependency::data);

    lws->erase(lws->begin());
  }

  // Remove end vertex
  graph_->removeVertex(graph_->getVertex(graph_->endTask_));
}

void DependencyGraph::removeStateDependencies() {
  EdgeIterator eit, eit_end;
  std::set<Edge> edgesToRemove;
  std::set<Edge>::iterator etrit;

  for (boost::tie(eit, eit_end) = boost::edges(*graph_->graph_);
      eit != eit_end; ++eit) {
    // Only if edge dependency is state or condition
    Dependency * d = graph_->getDependency((Edge)*eit);
    if (d->getDependencyType() == Dependency::state)
      edgesToRemove.insert((Edge)*eit);
  }
  for (etrit = edgesToRemove.begin(); etrit != edgesToRemove.end(); ++etrit)
    graph_->removeDependency(*etrit);
}

void DependencyGraph::transformConditionalStatesToConditions(
    boost::ptr_vector<XVariable> * variables) {
  std::pair<VertexIterator, VertexIterator> vp;
  boost::graph_traits<Graph>::out_edge_iterator oei, oei_end;
  boost::ptr_vector<XVariable>::iterator it;
  size_t count = 0;

  // For each vertex
  for (vp = boost::vertices(*graph_->graph_); vp.first != vp.second; ++vp.first) {
    // If out edges is larger than 1 and a state task
    if (boost::out_degree(*vp.first, *graph_->graph_) > 1 &&
        graph_->getTask(*vp.first)->getTaskType() == Task::xstate) {
      Task * t = graph_->getTask(*vp.first);
      // Change task type to a condition
      t->setTaskType(Task::xcondition);
      t->setName(boost::lexical_cast<std::string>(count++));
      t->setPriorityLevel(5);
      // Conditions read all variables (assume to help with splitting)
      for (it = variables->begin(); it != variables->end(); ++it) {
        t->addReadWriteVariable((*it).getName());
      }
    }
  }
}

void DependencyGraph::setStartTask(Task * task) {
  graph_->startTask_ = task;
}

void DependencyGraph::contractVertices(
    Task::TaskType taskType, Dependency::DependencyType dependencyType) {
  VertexIterator vi, vi_end;
  boost::graph_traits<Graph>::out_edge_iterator oei, oei_end;
  boost::graph_traits<Graph>::in_edge_iterator iei, iei_end;
  std::vector<Vertex> vertexToDelete;
  std::vector<Vertex>::iterator vit;
  // For each variable vertex
  for (boost::tie(vi, vi_end) = boost::vertices(*graph_->graph_);
      vi != vi_end; ++vi) {
    // If vertex is a variable
    if (graph_->getTask(*vi)->getTaskType() == taskType) {
      // Add an edge from all vertex sources to all vertex targets
      for (boost::tie(iei, iei_end) = boost::in_edges(*vi, *graph_->graph_);
          iei != iei_end; ++iei)
        for (boost::tie(oei, oei_end) = boost::out_edges(*vi, *graph_->graph_);
            oei != oei_end; ++oei)
          graph_->addEdge(boost::source((Edge)*iei, *graph_->graph_),
              boost::target((Edge)*oei, *graph_->graph_),
              "", dependencyType);
      // Add vertex to delete list (as cannot delete vertex within
      // an iterator)
      vertexToDelete.push_back(*vi);
    }
  }
  // Delete vertices in delete list
  graph_->removeVertices(&vertexToDelete);
}

void DependencyGraph::contractStateVertices() {
  // Change startVertex to the only and direct function under the current
  // state start vertex
  boost::graph_traits<Graph>::out_edge_iterator oei, oei_end;
  // If startTask a state (not a state converted to a condition
  // Todo add assert here for startTask_ != NULL
  // because if the model has not been validated then it is not set

  if (graph_->startTask_ == 0) throw flame::exceptions::flame_model_exception(
      "Model graph start task not initialised");

  if (graph_->startTask_->getTaskType() == Task::xstate)
    for (boost::tie(oei, oei_end) =
        boost::out_edges(graph_->getVertex(graph_->startTask_), *graph_->graph_);
        oei != oei_end; ++oei)
      graph_->startTask_ = graph_->getTask(boost::target((Edge)*oei, *graph_->graph_));


  // Contract state tasks and replace with state dependency
  contractVertices(Task::xstate, Dependency::state);
}

void DependencyGraph::contractVariableVertices() {
  // Contract variable tasks and replace with variable dependency
  contractVertices(Task::xvariable, Dependency::variable);
}

void DependencyGraph::removeRedundantDependencies() {
  size_t ii;
  // The resultant transitive reduction graph
  Graph * trgraph = new Graph;
  std::vector<TaskPtr> * trvertex2task =
      new std::vector<TaskPtr>(graph_->vertex2task_->size());

  // Create a map to get a property of a graph, in this case the vertex index
  typedef boost::property_map<Graph, boost::vertex_index_t>::const_type
      VertexIndexMap;
  VertexIndexMap index_map = get(boost::vertex_index, *graph_->graph_);
  // A vector of vertices to hold trgraph vertices
  std::vector<Vertex> to_tc_vec(boost::num_vertices(*graph_->graph_));
  // Property map iterator
  // Iterator: Vertex *
  // OffsetMap: VertexIndexMap
  // Value type of iterator: Vertex
  // Reference type of iterator: Vertex&
  boost::iterator_property_map<Vertex *, VertexIndexMap, Vertex, Vertex&>
  g_to_tc_map(&to_tc_vec[0], index_map);

  boost::transitive_reduction(*graph_->graph_, *trgraph, g_to_tc_map, index_map);

  // Create new vertex task mapping for trgraph
  for (ii = 0; ii < boost::num_vertices(*graph_->graph_); ++ii)
    trvertex2task->at(to_tc_vec[ii]) = graph_->vertex2task_->at(ii);

  // Make graph_ point to trgraph
  delete graph_->graph_;
  graph_->graph_ = trgraph;
  // Make vertex2task_ point to trvertex2task
  delete graph_->vertex2task_;
  graph_->vertex2task_ = trvertex2task;
  // Clear edge2dependency_ as edges no longer valid
  EdgeMap::iterator eit;
  for (eit = graph_->edge2dependency_->begin(); eit != graph_->edge2dependency_->end(); ++eit)
    delete ((*eit).second);
  graph_->edge2dependency_->clear();
}

Vertex DependencyGraph::getMessageVertex(std::string name,
    Task::TaskType type) {
  size_t ii;
  // For each task
  for (ii = 0; ii < graph_->vertex2task_->size(); ++ii)
    // If find message and type then return
    if (graph_->vertex2task_->at(ii)->getName() == name &&
        graph_->vertex2task_->at(ii)->getTaskType() == type)
      return ii;
  // Otherwise create new vertex and return
  Task * t = new Task(name, name, type);
  Vertex v = graph_->addVertex(t);
  return v;
}

void DependencyGraph::changeMessageTasksToSync() {
  boost::graph_traits<Graph>::out_edge_iterator oei, oei_end;
  boost::graph_traits<Graph>::in_edge_iterator iei, iei_end;
  std::vector<Vertex> vertexToDelete;
  std::set<Edge> edgesToRemove;
  std::set<Edge>::iterator etrit;
  size_t ii;
  // For each task
  for (ii = 0; ii < graph_->vertex2task_->size(); ++ii) {
    Task * t = graph_->vertex2task_->at(ii).get();
    // If message task
    if (t->getTaskType() == Task::xmessage) {
      // Create start and finish syncs
      Vertex s = getMessageVertex(t->getName(), Task::xmessage_sync);
      // For each incoming edge to message add to start
      for (boost::tie(iei, iei_end) = boost::in_edges(ii, *graph_->graph_);
          iei != iei_end; ++iei) {
        add_edge(boost::source(*iei, *graph_->graph_), s, *graph_->graph_);
        edgesToRemove.insert(*iei);
      }
      // For each out going edge to message add to finish
      for (boost::tie(oei, oei_end) = boost::out_edges(ii, *graph_->graph_);
          oei != oei_end; ++oei) {
        add_edge(s, boost::target(*oei, *graph_->graph_), *graph_->graph_);
        edgesToRemove.insert(*oei);
      }
      // delete message task
      vertexToDelete.push_back(ii);
    }
  }
  // Delete edges
  for (etrit = edgesToRemove.begin(); etrit != edgesToRemove.end(); ++etrit)
    graph_->removeDependency(*etrit);
  // Delete vertices in delete list
  graph_->removeVertices(&vertexToDelete);
}

void DependencyGraph::importStateGraph(StateGraph * stateGraph) {
  std::vector<TaskPtr> * v2t = stateGraph->getVertexTaskMap();
  size_t ii;
  std::map<Vertex, Vertex> import2new;
  EdgeIterator eit, end;
  EdgeMap * edgeDependencyMap = stateGraph->getEdgeDependencyMap();

  // For each task vertex map
  for (ii = 0; ii < v2t->size(); ++ii) {
    // Add vertex to current graph
    Vertex v = graph_->addVertex(v2t->at(ii));
    // Add to vertex to vertex map
    import2new.insert(std::make_pair(ii, v));
    // If task is an init agent then add edge
    if (v2t->at(ii)->getTaskType() == Task::start_agent)
      add_edge(graph_->getVertex(graph_->startTask_), v, *graph_->graph_);
    // If task is a data output task then add edge
    if (v2t->at(ii)->getTaskType() == Task::io_pop_write)
      add_edge(v, graph_->getVertex(graph_->endTask_), *graph_->graph_);
    // If start task make start task
    if (v2t->at(ii)->startTask()) graph_->startTask_ = v2t->at(ii).get();
    // If end task add to end tasks
    if (v2t->at(ii)->endTask()) graph_->endTasks_.insert(v2t->at(ii).get());
  }
  // For each edge
  for (boost::tie(eit, end) = boost::edges(*(stateGraph->getGraph()));
      eit != end; ++eit) {
    // Add edge using vertex to vertex map
    Vertex s = boost::source(*eit, *(stateGraph->getGraph()));
    Vertex t = boost::target(*eit, *(stateGraph->getGraph()));
    Vertex ns = (*(import2new.find(s))).second;
    Vertex nt = (*(import2new.find(t))).second;
    EdgeMap::iterator it = edgeDependencyMap->find(*eit);
    Dependency * d = it->second;
    graph_->addEdge(ns, nt, d->getName(), d->getDependencyType());
  }
}

void DependencyGraph::import(DependencyGraph * graph) {
  std::vector<TaskPtr> * v2t = graph->getVertexTaskMap();
  size_t ii;
  std::map<Vertex, Vertex> import2new;
  EdgeIterator eit, end;

  // For each task vertex map
  for (ii = 0; ii < v2t->size(); ++ii) {
    // Add vertex to current graph
    Vertex v = graph_->addVertex(v2t->at(ii));
    // Add to vertex to vertex map
    import2new.insert(std::make_pair(ii, v));
    // If task is an init agent then add edge
    if (v2t->at(ii)->getTaskType() == Task::start_agent)
      add_edge(graph_->getVertex(graph_->startTask_), v, *graph_->graph_);
    // If task is a data output task then add edge
    if (v2t->at(ii)->getTaskType() == Task::io_pop_write)
      add_edge(v, graph_->getVertex(graph_->endTask_), *graph_->graph_);
  }
  // For each edge
  for (boost::tie(eit, end) = boost::edges(*(graph->getGraph()));
      eit != end; ++eit) {
    // Add edge using vertex to vertex map
    Vertex s = boost::source(*eit, *(graph->getGraph()));
    Vertex t = boost::target(*eit, *(graph->getGraph()));
    Vertex ns = (*(import2new.find(s))).second;
    Vertex nt = (*(import2new.find(t))).second;
    add_edge(ns, nt, *graph_->graph_);
  }
}

void DependencyGraph::importGraphs(std::set<DependencyGraph*> graphs) {
  std::set<DependencyGraph*>::iterator it;

  // Add start task
  Task * t = new Task(graph_->name_, "Start", Task::start_model);
  graph_->addVertex(t);
  graph_->startTask_ = t;
  // Add finish task
  t = new Task(graph_->name_, "Finish", Task::finish_model);
  graph_->addVertex(t);
  graph_->endTask_ = t;

  for (it = graphs.begin(); it != graphs.end(); ++it)
    import((*it));

  // Contract start agents
  contractVertices(Task::start_agent, Dependency::blank);
  // Add message sync tasks
  changeMessageTasksToSync();
  // Add message clear tasks
  addMessageClearTasks();
}

void DependencyGraph::addMessageClearTasks() {
  VertexIterator vi, vi_end;
  boost::graph_traits<Graph>::out_edge_iterator oei, oei_end;

  // For each variable vertex
  for (boost::tie(vi, vi_end) = boost::vertices(*graph_->graph_);
      vi != vi_end; ++vi) {
    Task * t = graph_->getTask((*vi));
    // For each message sync task
    if (t->getTaskType() == Task::xmessage_sync) {
      // Create message clear task
      Task * task = new Task(t->getParentName(),
          t->getName(), Task::xmessage_clear);
      Vertex clearV = graph_->addVertex(task);
      // Get target tasks
      for (boost::tie(oei, oei_end) =
          boost::out_edges((*vi), *graph_->graph_); oei != oei_end; ++oei) {
        // Add edge from target tasks to clear task
        add_edge(boost::target((Edge)*oei, *graph_->graph_), clearV, *graph_->graph_);
      }
    }
  }
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

std::pair<int, std::string> DependencyGraph::checkCyclicDependencies() {
  // error message
  std::string error_msg;
  // visitor cycle detector for use with depth_first_search
  cycle_detector vis;

  try {
    // Depth first search applied to graph
    boost::depth_first_search(*graph_->graph_, visitor(vis));
    // If cyclic dependency is caught
  } catch(const has_cycle& err) {
    // Find associated dependency
    Dependency * d = graph_->getDependency(err.edge());
    // Find associated tasks
    Task * t1 = graph_->getTask(boost::source(err.edge(), *graph_->graph_));
    Task * t2 = graph_->getTask(boost::target(err.edge(), *graph_->graph_));
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

std::pair<int, std::string> DependencyGraph::checkFunctionConditions() {
  // error message
  std::string error_msg;
  std::pair<VertexIterator, VertexIterator> vp;
  // For each vertex
  for (vp = boost::vertices(*graph_->graph_); vp.first != vp.second; ++vp.first) {
    // If state
    if (graph_->getTask(*vp.first)->getTaskType() == Task::xstate) {
      // If out edges is larger than 1
      if (boost::out_degree(*vp.first, *graph_->graph_) > 1) {
        boost::graph_traits<Graph>::out_edge_iterator oei, oei_end;
        // For each out edge
        for (boost::tie(oei, oei_end) =
            boost::out_edges(*vp.first, *graph_->graph_);
            oei != oei_end; ++oei) {
          Task * t = graph_->getTask(boost::target((Edge)*oei, *graph_->graph_));
          // If condition is null then return an error
          if (!t->hasCondition()) {
            error_msg.append("Error: Function '");
            error_msg.append(t->getName());
            error_msg.append("' from a state with more than one out ");
            error_msg.append("going function does not have a condition.\n");
            return std::make_pair(1, error_msg);
          }
        }
      }
    }
  }

  return std::make_pair(0, error_msg);
}

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

struct graph_writer {
    void operator()(std::ostream& /*out*/) const {
      // out << "node [shape = rect]" << std::endl;
    }
};

void DependencyGraph::writeGraphviz(const std::string& fileName) const {
  std::fstream graphfile;
  graphfile.open(fileName.c_str(), std::fstream::out);

  boost::write_graphviz(graphfile, *graph_->graph_,
      vertex_label_writer(graph_->vertex2task_),
      edge_label_writer(graph_->edge2dependency_,
          edge_label_writer::arrowForward),
          graph_writer());

  graphfile.clear();
}

TaskIdSet DependencyGraph::getAgentTasks() const {
  std::pair<VertexIterator, VertexIterator> vp;
  TaskIdSet tasks;

  // For each vertex
  for (vp = boost::vertices(*graph_->graph_); vp.first != vp.second; ++vp.first) {
    Task * t = graph_->getTask(*vp.first);
    Task::TaskType type = t->getTaskType();

    // If agent task
    if (type == Task::xfunction || type == Task::xcondition)
      tasks.insert(*vp.first);
  }

  return tasks;
}

TaskIdSet DependencyGraph::getAgentIOTasks() const {
  std::pair<VertexIterator, VertexIterator> vp;
  TaskIdSet tasks;

  // For each vertex
  for (vp = boost::vertices(*graph_->graph_); vp.first != vp.second; ++vp.first)
    // If data task
    if (graph_->getTask(*vp.first)->getTaskType() == Task::io_pop_write)
      tasks.insert(*vp.first);

  return tasks;
}

TaskId DependencyGraph::getInitIOTask() const {
  std::pair<VertexIterator, VertexIterator> vp;

  // For each vertex
  for (vp = boost::vertices(*graph_->graph_); vp.first != vp.second; ++vp.first)
    // If start data task
    if (graph_->getTask(*vp.first)->getTaskType() == Task::start_model)
      return *vp.first;

  throw flame::exceptions::flame_model_exception(
        "Init IO Task does not exist");
}

TaskId DependencyGraph::getFinIOTask() const {
  std::pair<VertexIterator, VertexIterator> vp;

  // For each vertex
  for (vp = boost::vertices(*graph_->graph_); vp.first != vp.second; ++vp.first)
    // If finish data task
    if (graph_->getTask(*vp.first)->getTaskType() == Task::finish_model)
      return *vp.first;

  throw flame::exceptions::flame_model_exception(
        "Init IO Task does not exist");
}

TaskIdSet DependencyGraph::getMessageBoardSyncTasks() const {
  std::pair<VertexIterator, VertexIterator> vp;
  TaskIdSet tasks;

  // For each vertex
  for (vp = boost::vertices(*graph_->graph_); vp.first != vp.second; ++vp.first)
    // if message sync task
    if (graph_->getTask(*vp.first)->getTaskType() == Task::xmessage_sync)
      tasks.insert(*vp.first);

  return tasks;
}

TaskIdSet DependencyGraph::getMessageBoardClearTasks() const {
  std::pair<VertexIterator, VertexIterator> vp;
  TaskIdSet tasks;

  // For each vertex
  for (vp = boost::vertices(*graph_->graph_); vp.first != vp.second; ++vp.first) {
    // if message clear task
    if (graph_->getTask(*vp.first)->getTaskType() == Task::xmessage_clear)
      tasks.insert(*vp.first);
  }

  return tasks;
}

TaskIdMap DependencyGraph::getTaskDependencies() const {
  boost::graph_traits<Graph>::edge_iterator iei, iei_end;
  TaskIdMap dependencies;

  // For each edge
  for (boost::tie(iei, iei_end) = boost::edges(*graph_->graph_);
      iei != iei_end; ++iei) {
    // Get the source
    TaskId source = boost::source((Edge)*iei, *graph_->graph_);
    // Get the target
    TaskId target = boost::target((Edge)*iei, *graph_->graph_);
    // Add dependency
    dependencies.insert(std::pair<TaskId, TaskId>(target, source));
  }

  return dependencies;
}

std::string DependencyGraph::getTaskName(TaskId id) const {
  return graph_->getTask(id)->getTaskName();
}

std::string DependencyGraph::getTaskAgentName(TaskId id) const {
  return graph_->getTask(id)->getParentName();
}

std::string DependencyGraph::getTaskFunctionName(TaskId id) const {
  return graph_->getTask(id)->getName();
}

StringSet DependencyGraph::getTaskReadOnlyVariables(TaskId id) const {
  return graph_->getTask(id)->getReadOnlyVariablesConst();
}

StringSet DependencyGraph::getTaskWriteVariables(TaskId id) const {
  return graph_->getTask(id)->getWriteVariablesConst();
}

StringSet DependencyGraph::getTaskOutputMessages(TaskId id) const {
  return graph_->getTask(id)->getOutputMessagesConst();
}

StringSet DependencyGraph::getTaskInputMessages(TaskId id) const {
  return graph_->getTask(id)->getInputMessagesConst();
}

#ifdef TESTBUILD
bool DependencyGraph::dependencyExists(std::string name1, std::string name2) {
  int v1 = -1, v2 = -1;
  size_t ii;

  // For each task find vertex of task names
  for (ii = 0; ii < graph_->vertex2task_->size(); ++ii) {
    if (graph_->vertex2task_->at(ii)->getName() == name1) v1 = ii;
    if (graph_->vertex2task_->at(ii)->getName() == name2) v2 = ii;
  }
  // If either vertex not found then return false
  if (v1 == -1 || v2 == -1) return false;
  // Check for edge between vertices
  std::pair<Edge, bool> p = boost::edge((Vertex)v1, (Vertex)v2, *graph_->graph_);
  return p.second;
}

Vertex DependencyGraph::addTestVertex(Task * t) {
  return graph_->addVertex(t);
}

void DependencyGraph::addTestEdge(Vertex to, Vertex from, std::string name,
    Dependency::DependencyType type) {
  graph_->addEdge(to, from, name, type);
}

void DependencyGraph::setTestStartTask(Task * task) {
  setStartTask(task);
}

void DependencyGraph::addTestEndTask(Task * task) {
  graph_->endTasks_.insert(task);
}
#endif

}}   // namespace flame::model
