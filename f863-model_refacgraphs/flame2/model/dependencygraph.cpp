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

DependencyGraph::DependencyGraph() {}

std::vector<TaskPtr> * DependencyGraph::getVertexTaskMap() {
  return graph_.getVertexTaskMap();
}

void DependencyGraph::setName(std::string name) {
  name_ = name;
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
  contractVariableVertices();
#endif

#ifdef OUTPUT_GRAPHS
  writeGraphviz(name_ + "_4.dot");
#endif
  // Remove redundant dependencies
  graph_.removeRedundantDependencies();
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
    graph_.addEdge(*it, v, "Condition", Dependency::condition);
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
  VarMapToVertices * lws = graph_.getEndTask()->getLastWrites();
  size_t count = 0;

  while (!lws->empty()) {
    // Create new io write task
    Task * task = new Task(name_,
        boost::lexical_cast<std::string>(++count), Task::io_pop_write);
    Vertex vertex = graph_.addVertex(task);
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

void DependencyGraph::transformConditionalStatesToConditions(
    boost::ptr_vector<XVariable> * variables) {
  std::pair<VertexIterator, VertexIterator> vp;
  boost::graph_traits<Graph>::out_edge_iterator oei, oei_end;
  boost::ptr_vector<XVariable>::iterator it;
  size_t count = 0;

  // For each vertex
  for (vp = graph_.getVertices(); vp.first != vp.second; ++vp.first) {
    // If out edges is larger than 1 and a state task
    if (graph_.getVertexOutDegree(*vp.first) > 1 &&
        graph_.getTask(*vp.first)->getTaskType() == Task::xstate) {
      Task * t = graph_.getTask(*vp.first);
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

void DependencyGraph::contractVertices(
    Task::TaskType taskType, Dependency::DependencyType dependencyType) {
  VertexIterator vi, vi_end;
  OutEdgeIterator oei, oei_end;
  InEdgeIterator iei, iei_end;
  std::vector<Vertex> vertexToDelete;
  std::vector<Vertex>::iterator vit;
  // For each variable vertex
  for (boost::tie(vi, vi_end) = graph_.getVertices();
      vi != vi_end; ++vi) {
    // If vertex is a variable
    if (graph_.getTask(*vi)->getTaskType() == taskType) {
      // Add an edge from all vertex sources to all vertex targets
      for (boost::tie(iei, iei_end) = graph_.getVertexInEdges(*vi);
          iei != iei_end; ++iei)
        for (boost::tie(oei, oei_end) = graph_.getVertexOutEdges(*vi);
            oei != oei_end; ++oei)
          graph_.addEdge(graph_.getEdgeSource(*iei),
              graph_.getEdgeTarget(*oei), "", dependencyType);
      // Add vertex to delete list (as cannot delete vertex within
      // an iterator)
      vertexToDelete.push_back(*vi);
    }
  }
  // Delete vertices in delete list
  graph_.removeVertices(&vertexToDelete);
}

void DependencyGraph::contractStateVertices() {
  // Change startVertex to the only and direct function under the current
  // state start vertex
  boost::graph_traits<Graph>::out_edge_iterator oei, oei_end;
  // If startTask a state (not a state converted to a condition
  // Todo add assert here for startTask_ != NULL
  // because if the model has not been validated then it is not set

  if (graph_.getStartTask() == 0) throw flame::exceptions::flame_model_exception(
      "Model graph start task not initialised");

  if (graph_.getStartTask()->getTaskType() == Task::xstate)
    for (boost::tie(oei, oei_end) =
        graph_.getVertexOutEdges(graph_.getVertex(graph_.getStartTask()));
        oei != oei_end; ++oei)
      graph_.setStartTask(graph_.getTask(graph_.getEdgeTarget(*oei)));


  // Contract state tasks and replace with state dependency
  contractVertices(Task::xstate, Dependency::state);
}

void DependencyGraph::contractVariableVertices() {
  // Contract variable tasks and replace with variable dependency
  contractVertices(Task::xvariable, Dependency::variable);
}

Vertex DependencyGraph::getMessageVertex(std::string name,
    Task::TaskType type) {
  size_t ii;
  // For each task
  for (ii = 0; ii < graph_.getVertexTaskMap()->size(); ++ii)
    // If find message and type then return
    if (graph_.getVertexTaskMap()->at(ii)->getName() == name &&
        graph_.getVertexTaskMap()->at(ii)->getTaskType() == type)
      return ii;
  // Otherwise create new vertex and return
  Task * t = new Task(name, name, type);
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
  for (ii = 0; ii < graph_.getVertexTaskMap()->size(); ++ii) {
    Task * t = graph_.getVertexTaskMap()->at(ii).get();
    // If message task
    if (t->getTaskType() == Task::xmessage) {
      // Create start and finish syncs
      Vertex s = getMessageVertex(t->getName(), Task::xmessage_sync);
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
  std::vector<TaskPtr> * v2t = stateGraph->getVertexTaskMap();
  size_t ii;
  std::map<Vertex, Vertex> import2new;
  EdgeIterator eit, end;
  EdgeMap * edgeDependencyMap = stateGraph->getEdgeDependencyMap();

  // For each task vertex map
  for (ii = 0; ii < v2t->size(); ++ii) {
    // Add vertex to current graph
    Vertex v = graph_.addVertex(v2t->at(ii));
    // Add to vertex to vertex map
    import2new.insert(std::make_pair(ii, v));
    // If task is an init agent then add edge
    if (v2t->at(ii)->getTaskType() == Task::start_agent)
      graph_.addEdge(graph_.getVertex(graph_.getStartTask()), v, "", Dependency::blank);
    // If task is a data output task then add edge
    if (v2t->at(ii)->getTaskType() == Task::io_pop_write)
      graph_.addEdge(v, graph_.getVertex(graph_.getEndTask()), "", Dependency::blank);
    // If start task make start task
    if (v2t->at(ii)->startTask()) graph_.setStartTask(v2t->at(ii).get());
    // If end task add to end tasks
    if (v2t->at(ii)->endTask()) graph_.addEndTask(v2t->at(ii).get());
  }
  // For each edge
  for (boost::tie(eit, end) = stateGraph->getEdges(); //boost::edges(*(stateGraph->getGraph()));
      eit != end; ++eit) {
    // Add edge using vertex to vertex map
    Vertex s = stateGraph->getEdgeSource(*eit); //boost::source(*eit, *(stateGraph->getGraph()));
    Vertex t = stateGraph->getEdgeTarget(*eit); //boost::target(*eit, *(stateGraph->getGraph()));
    Vertex ns = (*(import2new.find(s))).second;
    Vertex nt = (*(import2new.find(t))).second;
    EdgeMap::iterator it = edgeDependencyMap->find(*eit);
    Dependency * d = it->second;
    graph_.addEdge(ns, nt, d->getName(), d->getDependencyType());
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
    Vertex v = graph_.addVertex(v2t->at(ii));
    // Add to vertex to vertex map
    import2new.insert(std::make_pair(ii, v));
    // If task is an init agent then add edge
    if (v2t->at(ii)->getTaskType() == Task::start_agent)
      graph_.addEdge(graph_.getVertex(graph_.getStartTask()), v, "", Dependency::blank);
    // If task is a data output task then add edge
    if (v2t->at(ii)->getTaskType() == Task::io_pop_write)
      graph_.addEdge(v, graph_.getVertex(graph_.getEndTask()), "", Dependency::blank);
  }
  // For each edge
  for (boost::tie(eit, end) = graph->getGraph()->getEdges();
      eit != end; ++eit) {
    // Add edge using vertex to vertex map
    Vertex s = graph->getGraph()->getEdgeSource(*eit);
    Vertex t = graph->getGraph()->getEdgeTarget(*eit);
    Vertex ns = (*(import2new.find(s))).second;
    Vertex nt = (*(import2new.find(t))).second;
    graph_.addEdge(ns, nt, "", Dependency::blank);
  }
}

void DependencyGraph::importGraphs(std::set<DependencyGraph*> graphs) {
  std::set<DependencyGraph*>::iterator it;

  // Add start task
  Task * t = new Task(name_, "Start", Task::start_model);
  graph_.addVertex(t);
  graph_.setStartTask(t);
  // Add finish task
  t = new Task(name_, "Finish", Task::finish_model);
  graph_.addVertex(t);
  graph_.setEndTask(t);

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
  for (boost::tie(vi, vi_end) = graph_.getVertices();
      vi != vi_end; ++vi) {
    Task * t = graph_.getTask((*vi));
    // For each message sync task
    if (t->getTaskType() == Task::xmessage_sync) {
      // Create message clear task
      Task * task = new Task(t->getParentName(),
          t->getName(), Task::xmessage_clear);
      Vertex clearV = graph_.addVertex(task);
      // Get target tasks
      for (boost::tie(oei, oei_end) =
          graph_.getVertexOutEdges(*vi); oei != oei_end; ++oei) {
        // Add edge from target tasks to clear task
        graph_.addEdge(graph_.getEdgeTarget(*oei), clearV, "", Dependency::blank);
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
    if (graph_.getTask(*vp.first)->getTaskType() == Task::xstate) {
      // If out edges is larger than 1
      if (graph_.getVertexOutDegree(*vp.first) > 1) {
        boost::graph_traits<Graph>::out_edge_iterator oei, oei_end;
        // For each out edge
        for (boost::tie(oei, oei_end) =
            graph_.getVertexOutEdges(*vp.first);
            oei != oei_end; ++oei) {
          Task * t = graph_.getTask(graph_.getEdgeTarget(*oei));
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

TaskIdSet DependencyGraph::getAgentTasks() const {
  std::pair<VertexIterator, VertexIterator> vp;
  TaskIdSet tasks;

  // For each vertex
  for (vp = graph_.getVertices(); vp.first != vp.second; ++vp.first) {
    Task * t = graph_.getTask(*vp.first);
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
  for (vp = graph_.getVertices(); vp.first != vp.second; ++vp.first)
    // If data task
    if (graph_.getTask(*vp.first)->getTaskType() == Task::io_pop_write)
      tasks.insert(*vp.first);

  return tasks;
}

TaskId DependencyGraph::getInitIOTask() const {
  std::pair<VertexIterator, VertexIterator> vp;

  // For each vertex
  for (vp = graph_.getVertices(); vp.first != vp.second; ++vp.first)
    // If start data task
    if (graph_.getTask(*vp.first)->getTaskType() == Task::start_model)
      return *vp.first;

  throw flame::exceptions::flame_model_exception(
        "Init IO Task does not exist");
}

TaskId DependencyGraph::getFinIOTask() const {
  std::pair<VertexIterator, VertexIterator> vp;

  // For each vertex
  for (vp = graph_.getVertices(); vp.first != vp.second; ++vp.first)
    // If finish data task
    if (graph_.getTask(*vp.first)->getTaskType() == Task::finish_model)
      return *vp.first;

  throw flame::exceptions::flame_model_exception(
        "Init IO Task does not exist");
}

TaskIdSet DependencyGraph::getMessageBoardSyncTasks() const {
  std::pair<VertexIterator, VertexIterator> vp;
  TaskIdSet tasks;

  // For each vertex
  for (vp = graph_.getVertices(); vp.first != vp.second; ++vp.first)
    // if message sync task
    if (graph_.getTask(*vp.first)->getTaskType() == Task::xmessage_sync)
      tasks.insert(*vp.first);

  return tasks;
}

TaskIdSet DependencyGraph::getMessageBoardClearTasks() const {
  std::pair<VertexIterator, VertexIterator> vp;
  TaskIdSet tasks;

  // For each vertex
  for (vp = graph_.getVertices(); vp.first != vp.second; ++vp.first) {
    // if message clear task
    if (graph_.getTask(*vp.first)->getTaskType() == Task::xmessage_clear)
      tasks.insert(*vp.first);
  }

  return tasks;
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
    dependencies.insert(std::pair<TaskId, TaskId>(target, source));
  }

  return dependencies;
}

std::string DependencyGraph::getTaskName(TaskId id) const {
  return graph_.getTask(id)->getTaskName();
}

std::string DependencyGraph::getTaskAgentName(TaskId id) const {
  return graph_.getTask(id)->getParentName();
}

std::string DependencyGraph::getTaskFunctionName(TaskId id) const {
  return graph_.getTask(id)->getName();
}

StringSet DependencyGraph::getTaskReadOnlyVariables(TaskId id) const {
  return graph_.getTask(id)->getReadOnlyVariablesConst();
}

StringSet DependencyGraph::getTaskWriteVariables(TaskId id) const {
  return graph_.getTask(id)->getWriteVariablesConst();
}

StringSet DependencyGraph::getTaskOutputMessages(TaskId id) const {
  return graph_.getTask(id)->getOutputMessagesConst();
}

StringSet DependencyGraph::getTaskInputMessages(TaskId id) const {
  return graph_.getTask(id)->getInputMessagesConst();
}

#ifdef TESTBUILD
bool DependencyGraph::dependencyExists(std::string name1, std::string name2) {
  int v1 = -1, v2 = -1;
  size_t ii;

  // For each task find vertex of task names
  for (ii = 0; ii < graph_.getVertexTaskMap()->size(); ++ii) {
    if (graph_.getVertexTaskMap()->at(ii)->getName() == name1) v1 = ii;
    if (graph_.getVertexTaskMap()->at(ii)->getName() == name2) v2 = ii;
  }
  // If either vertex not found then return false
  if (v1 == -1 || v2 == -1) return false;
  // Check for edge between vertices
  return graph_.edgeExists(v1, v2);
}

Vertex DependencyGraph::addTestVertex(Task * t) {
  return graph_.addVertex(t);
}

void DependencyGraph::addTestEdge(Vertex to, Vertex from, std::string name,
    Dependency::DependencyType type) {
  graph_.addEdge(to, from, name, type);
}

void DependencyGraph::setTestStartTask(Task * task) {
  graph_.setStartTask(task);
}

void DependencyGraph::addTestEndTask(Task * task) {
  graph_.addEndTask(task);
}
#endif

}}   // namespace flame::model
