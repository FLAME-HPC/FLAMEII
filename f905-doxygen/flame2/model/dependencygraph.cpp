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
#include "model_task.hpp"
#include "data_dependency_analyser.hpp"

namespace flame { namespace model {

DependencyGraph::DependencyGraph() {}

void DependencyGraph::setName(std::string name) {
  name_ = name;
}

void DependencyGraph::generateDependencyGraph(
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
  DataDependencyAnalyser dda = DataDependencyAnalyser(&graph_, name_);
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
  contractVertices(Task::xvariable, Dependency::variable);
#endif

#ifdef OUTPUT_GRAPHS
  writeGraphviz(name_ + "_4.dot");
#endif
  // Remove redundant dependencies
  graph_.removeRedundantDependencies();
#ifdef OUTPUT_GRAPHS
  writeGraphviz(name_ + "_5.dot");
#endif
}

bool DependencyGraph::setContains(
    std::set<std::string>* a, std::set<std::string>* find_in_a) {
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

#ifdef GROUP_WRITE_VARS
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
#endif

void DependencyGraph::AddVariableOutput() {
  // For each function that last writes a variable add dependency
  // to the data output of that variable
  VarMapToVertices::iterator vwit;
  std::set<size_t>::iterator sit;
  VarMapToVertices * lws = graph_.getEndTask()->getLastWrites();

  while (!lws->empty()) {
    // Create new io write task
    std::string var = (*lws->begin()).first;
    ModelTask * task = new ModelTask(name_, var, ModelTask::io_pop_write);
    task->addWriteVariable(var);
    Vertex vertex = graph_.addVertex(task);
#ifdef GROUP_WRITE_VARS
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
#endif
    // Add edges from each writing vector to task
    for (sit = (*lws->begin()).second.begin();
        sit != (*lws->begin()).second.end(); ++sit)
      graph_.addEdge((*sit), vertex, "", Dependency::data);

    lws->erase(lws->begin());
  }

  // Remove end vertex
  graph_.removeVertex(graph_.getVertex(graph_.getEndTask()));
}

void DependencyGraph::removeStateDependencies() {
  EdgeIterator eit, eit_end;
  std::set<Edge> edgesToRemove;
  std::set<Edge>::iterator etrit;

  for (boost::tie(eit, eit_end) = graph_.getEdges();
      eit != eit_end; ++eit) {
    // Only if edge dependency is state or condition
    Dependency * d = graph_.getDependency((Edge)*eit);
    if (d->getDependencyType() == Dependency::state)
      edgesToRemove.insert((Edge)*eit);
  }
  for (etrit = edgesToRemove.begin(); etrit != edgesToRemove.end(); ++etrit)
    graph_.removeDependency(*etrit);
}

Vertex DependencyGraph::copyVertexIncludingEdges(Vertex in) {
  OutEdgeIterator oei, oei_end;
  InEdgeIterator iei, iei_end;

  // get state model task
  ModelTask * t = graph_.getTask(in);
  // create new condition vertex to replace state vertex
  ModelTask * task = new ModelTask(t->getParentName(),
      t->getName(), t->getTaskType());
  Vertex vertex = graph_.addVertex(task);
  // copy edges
  for (boost::tie(iei, iei_end) = graph_.getVertexInEdges(in);
      iei != iei_end; ++iei) graph_.addEdge(graph_.getEdgeSource(*iei),
          vertex, "", Dependency::condition);
  for (boost::tie(oei, oei_end) = graph_.getVertexOutEdges(in);
      oei != oei_end; ++oei) graph_.addEdge(vertex,
          graph_.getEdgeTarget(*oei), "", Dependency::condition);
  return vertex;
}

void DependencyGraph::transformConditionalStatesToConditions(
    boost::ptr_vector<XVariable> * variables) {
  std::pair<VertexIterator, VertexIterator> vp;
  boost::ptr_vector<XVariable>::iterator it;
  size_t count = 0;
  std::vector<Vertex> verticiesToRemove;

  // for each vertex
  for (vp = graph_.getVertices(); vp.first != vp.second; ++vp.first) {
    // if out edges is larger than 1 and a state task
    if (graph_.getVertexOutDegree(*vp.first) > 1 &&
        graph_.getTask(*vp.first)->getTaskType() == ModelTask::xstate) {
      // copy state vertex
      Vertex vertex = copyVertexIncludingEdges(*vp.first);
      // update new vertex as a condition
      ModelTask * task = graph_.getTask(vertex);
      task->setName(boost::lexical_cast<std::string>(count++));
      task->setTaskType(ModelTask::xcondition);
      task->setPriorityLevel(5);
      // conditions read all variables (assume to help with splitting)
      for (it = variables->begin(); it != variables->end(); ++it)
        task->addReadWriteVariable((*it).getName());
      // add state vertex to list of vertices to remove
      verticiesToRemove.push_back(*vp.first);
    }
  }
  // remove vertices from graph
  graph_.removeVertices(&verticiesToRemove);
}

void DependencyGraph::contractVertices(
    ModelTask::TaskType taskType, Dependency::DependencyType dependencyType) {
  VertexIterator vi, vi_end;
  OutEdgeIterator oei, oei_end;
  InEdgeIterator iei, iei_end;
  std::vector<Vertex> vertexToDelete;
  std::vector<Vertex>::iterator vit;
  // for each vertex
  for (boost::tie(vi, vi_end) = graph_.getVertices();
      vi != vi_end; ++vi) {
    // if vertex has required type
    if (graph_.getTask(*vi)->getTaskType() == taskType) {
      // add an edge from all vertex sources to all vertex targets
      for (boost::tie(iei, iei_end) = graph_.getVertexInEdges(*vi);
          iei != iei_end; ++iei)
        for (boost::tie(oei, oei_end) = graph_.getVertexOutEdges(*vi);
            oei != oei_end; ++oei)
          graph_.addEdge(graph_.getEdgeSource(*iei),
              graph_.getEdgeTarget(*oei), "", dependencyType);
      // add vertex to delete list (as cannot delete vertex within
      // an iterator)
      vertexToDelete.push_back(*vi);
    }
  }
  // delete vertices in delete list
  graph_.removeVertices(&vertexToDelete);
}

void DependencyGraph::contractStateVertices() {
  // change startVertex to the only and direct function under the current
  // state start vertex
  boost::graph_traits<Graph>::out_edge_iterator oei, oei_end;

  // if startTask a state (not a state converted to a condition
  // because if the model has not been validated then it is not set

  if (graph_.getStartTask() == 0)
    throw flame::exceptions::flame_model_exception(
      "Model graph start task not initialised");

  if (graph_.getStartTask()->getTaskType() == ModelTask::xstate)
    for (boost::tie(oei, oei_end) =
        graph_.getVertexOutEdges(graph_.getVertex(graph_.getStartTask()));
        oei != oei_end; ++oei)
      graph_.setStartTask(graph_.getTask(graph_.getEdgeTarget(*oei)));

  // Contract state tasks and replace with state dependency
  contractVertices(ModelTask::xstate, Dependency::state);
}

Vertex DependencyGraph::getMessageVertex(std::string name,
    ModelTask::TaskType type) {
  size_t ii;
  // For each task
  for (ii = 0; ii < graph_.getTaskCount(); ++ii)
    // If find message and type then return
    if (graph_.getTask(ii)->getName() == name &&
        graph_.getTask(ii)->getTaskType() == type)
      return ii;
  // Otherwise create new vertex and return
  ModelTask * t = new ModelTask(name, name, type);
  Vertex v = graph_.addVertex(t);
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
  for (ii = 0; ii < graph_.getTaskCount(); ++ii) {
    ModelTask * t = graph_.getTask(ii);
    // If message task
    if (t->getTaskType() == ModelTask::xmessage) {
      // Create start and finish syncs
      Vertex s = getMessageVertex(t->getName(), ModelTask::xmessage_sync);
      // For each incoming edge to message add to start
      for (boost::tie(iei, iei_end) = graph_.getVertexInEdges(ii);
          iei != iei_end; ++iei) {
        graph_.addEdge(graph_.getEdgeSource(*iei), s, "", Dependency::blank);
        edgesToRemove.insert(*iei);
      }
      // For each out going edge to message add to finish
      for (boost::tie(oei, oei_end) = graph_.getVertexOutEdges(ii);
          oei != oei_end; ++oei) {
        graph_.addEdge(s, graph_.getEdgeTarget(*oei), "", Dependency::blank);
        edgesToRemove.insert(*oei);
      }
      // delete message task
      vertexToDelete.push_back(ii);
    }
  }
  // Delete edges
  for (etrit = edgesToRemove.begin(); etrit != edgesToRemove.end(); ++etrit)
    graph_.removeDependency(*etrit);
  // Delete vertices in delete list
  graph_.removeVertices(&vertexToDelete);
}

void DependencyGraph::importStateGraph(StateGraph * stateGraph) {
  size_t ii;
  std::map<Vertex, Vertex> import2new;
  EdgeIterator eit, end;
  EdgeMap * edgeDependencyMap = stateGraph->getEdgeDependencyMap();
  const TaskList * tasklist = stateGraph->getTaskList();

  // For each task vertex map
  for (ii = 0; ii < tasklist->getTaskCount(); ++ii) {
    // get task
    ModelTask * t = tasklist->getTask(ii);
    // Add vertex to current graph
    Vertex v = graph_.addVertex(tasklist->getTaskPtr(ii));
    // Add to vertex to vertex map
    import2new.insert(std::make_pair(ii, v));
    // If task is an init agent then add edge
    if (t->getTaskType() == ModelTask::start_agent)
      graph_.addEdge(
          graph_.getVertex(graph_.getStartTask()), v, "", Dependency::blank);
    // If task is a data output task then add edge
    if (t->getTaskType() == ModelTask::io_pop_write)
      graph_.addEdge(
          v, graph_.getVertex(graph_.getEndTask()), "", Dependency::blank);
    // If start task make start task
    if (t->startTask()) graph_.setStartTask(t);
    // If end task add to end tasks
    if (t->endTask()) graph_.addEndTask(t);
  }
  // For each edge
  for (boost::tie(eit, end) = stateGraph->getEdges();
      eit != end; ++eit) {
    // Add edge using vertex to vertex map
    Vertex s = stateGraph->getEdgeSource(*eit);
    Vertex t = stateGraph->getEdgeTarget(*eit);
    Vertex ns = (*(import2new.find(s))).second;
    Vertex nt = (*(import2new.find(t))).second;
    Dependency * d = edgeDependencyMap->find(*eit)->second;
    graph_.addEdge(ns, nt, d->getName(), d->getDependencyType());
  }
}

void DependencyGraph::import(DependencyGraph * graph) {
  size_t ii;
  std::map<Vertex, Vertex> import2new;

  const TaskList * tasklist = graph->getTaskList();
  // For each task vertex map
  for (ii = 0; ii < tasklist->getTaskCount(); ++ii) {
    // Add vertex to current graph
    Vertex v = graph_.addVertex(tasklist->getTaskPtr(ii));
    // Add to vertex to vertex map
    import2new.insert(std::make_pair(ii, v));
    // If task is an init agent then add edge
    if (tasklist->getTask(ii)->getTaskType() == ModelTask::start_agent)
      graph_.addEdge(
          graph_.getVertex(graph_.getStartTask()), v, "", Dependency::blank);
    // If task is a data output task then add edge
    if (tasklist->getTask(ii)->getTaskType() == ModelTask::io_pop_write)
      graph_.addEdge(
          v, graph_.getVertex(graph_.getEndTask()), "", Dependency::blank);
  }
  // For each edge
  TaskIdMap dependencies = graph->getTaskDependencies();
  TaskIdMap::iterator mit;
  for (mit = dependencies.begin(); mit != dependencies.end(); ++mit)
    // Add edge using vertex to vertex map
    graph_.addEdge(
        (*(import2new.find((*mit).first))).second,
        (*(import2new.find((*mit).second))).second,
        "", Dependency::blank);
}

void DependencyGraph::importGraphs(std::set<DependencyGraph*> graphs) {
  std::set<DependencyGraph*>::iterator it;

  // Add start task
  ModelTask * t = new ModelTask(name_, "Start", ModelTask::start_model);
  graph_.addVertex(t);
  graph_.setStartTask(t);
  // Add finish task
  t = new ModelTask(name_, "Finish", ModelTask::finish_model);
  graph_.addVertex(t);
  graph_.setEndTask(t);

  for (it = graphs.begin(); it != graphs.end(); ++it)
    import((*it));

  // Contract start agents
  contractVertices(ModelTask::start_agent, Dependency::blank);
  // Add message sync tasks
  changeMessageTasksToSync();
  // Add message clear tasks
  addMessageClearTasks();
}

void DependencyGraph::addMessageClearTasks() {
  VertexIterator vi, vi_end;
  boost::graph_traits<Graph>::out_edge_iterator oei, oei_end;

  // For each variable vertex
  for (boost::tie(vi, vi_end) = graph_.getVertices();
      vi != vi_end; ++vi) {
    ModelTask * t = graph_.getTask((*vi));
    // For each message sync task
    if (t->getTaskType() == ModelTask::xmessage_sync) {
      // Create message clear task
      ModelTask * task = new ModelTask(t->getParentName(),
          t->getName(), ModelTask::xmessage_clear);
      Vertex clearV = graph_.addVertex(task);
      // Get target tasks
      for (boost::tie(oei, oei_end) =
          graph_.getVertexOutEdges(*vi); oei != oei_end; ++oei) {
        // Add edge from target tasks to clear task
        graph_.addEdge(
            graph_.getEdgeTarget(*oei), clearV, "", Dependency::blank);
      }
    }
  }
}

std::pair<int, std::string> DependencyGraph::checkCyclicDependencies() {
  return graph_.checkCyclicDependencies();
}

std::pair<int, std::string> DependencyGraph::checkFunctionConditions() {
  // error message
  std::string error_msg;
  std::pair<VertexIterator, VertexIterator> vp;
  // For each vertex
  for (vp = graph_.getVertices(); vp.first != vp.second; ++vp.first) {
    // If state
    if (graph_.getTask(*vp.first)->getTaskType() == ModelTask::xstate) {
      // If out edges is larger than 1
      if (graph_.getVertexOutDegree(*vp.first) > 1) {
        boost::graph_traits<Graph>::out_edge_iterator oei, oei_end;
        // For each out edge
        for (boost::tie(oei, oei_end) =
            graph_.getVertexOutEdges(*vp.first);
            oei != oei_end; ++oei) {
          ModelTask * t = graph_.getTask(graph_.getEdgeTarget(*oei));
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

void DependencyGraph::writeGraphviz(const std::string& fileName) const {
  graph_.writeGraphviz(fileName);
}

TaskIdMap DependencyGraph::getTaskDependencies() const {
  boost::graph_traits<Graph>::edge_iterator iei, iei_end;
  TaskIdMap dependencies;

  // For each edge
  for (boost::tie(iei, iei_end) = graph_.getEdges();
      iei != iei_end; ++iei) {
    // Get the source
    Edge e = (Edge)*iei;
    TaskId source = graph_.getEdgeSource(e);
    // Get the target
    TaskId target = graph_.getEdgeTarget(e);
    // Add dependency
    dependencies.insert(std::pair<TaskId, TaskId>(source, target));
  }

  return dependencies;
}

const TaskList * DependencyGraph::getTaskList() const {
  return graph_.getTaskList();
}

#ifdef TESTBUILD
bool DependencyGraph::dependencyExists(
    ModelTask::TaskType type1, std::string name1,
    ModelTask::TaskType type2, std::string name2) {
  int v1 = -1, v2 = -1;
  size_t ii;

  // For each task find vertex of task names
  for (ii = 0; ii < graph_.getTaskCount(); ++ii) {
    ModelTask * t = graph_.getTask(ii);
    if (t->getTaskType() == type1 && t->getName() == name1) v1 = ii;
    if (t->getTaskType() == type2 && t->getName() == name2) v2 = ii;
  }
  // If either vertex not found then return false
  if (v1 == -1 || v2 == -1) return false;
  // Check for edge between vertices
  return graph_.edgeExists(v1, v2);
}

Vertex DependencyGraph::addTestVertex(ModelTask * t) {
  return graph_.addVertex(t);
}

void DependencyGraph::addTestEdge(Vertex to, Vertex from, std::string name,
    Dependency::DependencyType type) {
  graph_.addEdge(to, from, name, type);
}

void DependencyGraph::setTestStartTask(ModelTask * task) {
  graph_.setStartTask(task);
}

void DependencyGraph::addTestEndTask(ModelTask * task) {
  graph_.addEndTask(task);
}
#endif

}}   // namespace flame::model
