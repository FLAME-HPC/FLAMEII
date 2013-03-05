/*!
 * \file flame2/model/data_dependency_analyser.cpp
 * \author Simon Coakley
 * \date 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief DataDependencyAnalyser: analyses data dependency
 */
#include <boost/graph/topological_sort.hpp>
#include <string>
#include <vector>
#include <set>
#include "flame2/config.hpp"
#include "data_dependency_analyser.hpp"

namespace flame { namespace model {

DataDependencyAnalyser::DataDependencyAnalyser(XGraph * graph, std::string name)
  : graph_(graph), name_(name) {}

void clearVarWriteSet(std::string name,
    VarMapToVertices * lastWrites) {
  VarMapToVertices::iterator it = lastWrites->find(name);
  if (it != lastWrites->end()) (*it).second.clear();
}

std::set<size_t> * getVertexSet(
    std::string name, VarMapToVertices * lastWrites) {
  VarMapToVertices::iterator it = lastWrites->find(name);
  if (it != lastWrites->end()) return &(*it).second;
  // If name not found then add
  return &(*(lastWrites->insert(
      std::make_pair(name, std::set<size_t>())).first)).second;
}

void addVectorToVarWriteSet(std::string name, Vertex v,
    VarMapToVertices * lastWrites) {
  getVertexSet(name, lastWrites)->insert(v);
}

void copyVarWriteSets(VarMapToVertices * from, VarMapToVertices * to) {
  VarMapToVertices::iterator it;
  std::set<size_t> * vset;

  // For each var in from
  for (it = from->begin(); it != from->end(); ++it) {
    // Get associated var vertex set from to
    vset = getVertexSet((*it).first, to);
    // Insert from set to to set
    vset->insert((*it).second.begin(), (*it).second.end());
  }
}

void DataDependencyAnalyser::addStartTask(boost::ptr_vector<XVariable> * variables) {
  boost::ptr_vector<XVariable>::iterator i;
  // Add init function to provide first writes of all variables
  Task * initTask = new Task(name_, name_, Task::start_agent);
  Vertex initVertex = graph_->addVertex(initTask);
  // Add edge from init to start vertex
  graph_->addEdge(initVertex, graph_->getVertex(graph_->getStartTask()),
      "Start", Dependency::init);
  // Add all variables to init task write list
  for (i = variables->begin(); i != variables->end(); ++i) {
    initTask->addWriteVariable((*i).getName());
    addVectorToVarWriteSet((*i).getName(), initVertex,
        initTask->getLastWrites());
    addVectorToVarWriteSet((*i).getName(), initVertex,
        initTask->getLastReads());
  }
}

void DataDependencyAnalyser::addEndTask() {
  std::set<Task*>::iterator t_it;
  // Add end function to provide last writes to ioput
  graph_->setEndTask(new Task(name_, name_, Task::finish_agent));
  Vertex v = graph_->addVertex(graph_->getEndTask());
  for (t_it = graph_->getEndTasks()->begin(); t_it != graph_->getEndTasks()->end(); ++t_it)
    graph_->addEdge(graph_->getVertex(*t_it), v, "End", Dependency::init);
}

void DataDependencyAnalyser::copyWritingAndReadingVerticesFromInEdges(Vertex v,
    Task * task) {
  boost::graph_traits<Graph>::in_edge_iterator iei, iei_end;

  // For each in edge task
  boost::tie(iei, iei_end) = graph_->getVertexInEdges(v);
  for (; iei != iei_end; ++iei) {
    Task * t = graph_->getTask(graph_->getEdgeSource((Edge)*iei));
    // Copy last writing from incoming functions
    copyVarWriteSets(t->getLastWrites(), task->getLastWrites());
    // Copy last reading from incoming functions
    copyVarWriteSets(t->getLastReads(), task->getLastReads());
  }
}

void DataDependencyAnalyser::addWriteDependencies(Vertex v, Task * t) {
  std::set<std::string>::iterator varit;
  VarMapToVertices::iterator wit;
  std::set<size_t>::iterator it;

  // For each write variable
  for (varit = t->getWriteVariables()->begin();
      varit != t->getWriteVariables()->end(); ++varit) {
    // Look at last reads
    for (wit = t->getLastReads()->begin();
        wit != t->getLastReads()->end(); ++wit) {
      // If write variable equals last reads variable
      if ((*wit).first == (*varit)) {
        // For each task that reads the variable
        for (it = (*wit).second.begin();
            it != (*wit).second.end(); ++it) {
          // If not same as current task
          if (*it != v)
            // Add a dependency to it
            graph_->addEdge(*it, v, *varit, Dependency::variable);
        }
      }
    }
  }
}

void DataDependencyAnalyser::addReadDependencies(Vertex v, Task * t) {
  std::set<std::string>::iterator varit;
  VarMapToVertices::iterator wit;
  std::set<size_t>::iterator it;
  // For each read variable
#ifndef USE_VARIABLE_VERTICES
  std::set<Vertex> alreadyUsed;
#endif
  for (varit = t->getReadVariables()->begin();
      varit != t->getReadVariables()->end(); ++varit) {
    for (wit = t->getLastWrites()->begin();
        wit != t->getLastWrites()->end(); ++wit) {
      // If read variable equals last writes variable
      if ((*wit).first == (*varit)) {
        // For each task
        for (it = (*wit).second.begin();
            it != (*wit).second.end(); ++it) {
          if (*it != v) {
            // Add edge
#ifdef USE_VARIABLE_VERTICES
            graph_->addEdge(*it, v, *varit, Dependency::variable);
#else
            if (alreadyUsed.find(*it) == alreadyUsed.end()) {
              graph_->addEdge(*it, v, "Data", Dependency::variable);
              // Add vertex to ones already used
              alreadyUsed.insert(*it);
            }
#endif
          }
        }
      }
    }
  }
}

void DataDependencyAnalyser::addWritingVerticesToList(Vertex v, Task * t) {
  std::set<std::string>::iterator varit;
  // For writing variables create new vertex and add edge
  std::set<std::string> * rwv = t->getWriteVariables();
  for (varit = rwv->begin(); varit != rwv->end(); ++varit) {
    // Remove writes from lastWrites
    clearVarWriteSet((*varit), t->getLastWrites());
    // Remove reads from lastReads
    clearVarWriteSet((*varit), t->getLastReads());
#ifdef USE_VARIABLE_VERTICES
    // New vertex
    Task * task = new Task(name_, (*varit), Task::xvariable);
    Vertex varVertex = graph_->addVertex(task);
    // Edge to vertex
    graph_->addEdge(v, varVertex, (*varit), Dependency::variable);
    // Add new write
    addVectorToVarWriteSet((*varit), varVertex, t->getLastWrites());
#else
    // Add new write
    addVectorToVarWriteSet((*varit), v, t->getLastWrites());
#endif
  }
  // For reading variables
  rwv = t->getReadVariables();
  for (varit = rwv->begin(); varit != rwv->end(); ++varit) {
    // Add new read
    addVectorToVarWriteSet((*varit), v, t->getLastReads());
  }
}

void DataDependencyAnalyser::addDataDependencies(
    boost::ptr_vector<XVariable> * variables) {
  std::vector<Vertex>::reverse_iterator vit;
  std::vector<Vertex> sorted_vertices;

  // Add start and end tasks
  addStartTask(variables);
  addEndTask();

  // Create topological sorted list of vertices
  sorted_vertices = graph_->getTopologicalSortedVertices();

  // For each vertex in topological order
  // Iterate in reverse as output order is reversed
  for (vit = sorted_vertices.rbegin(); vit != sorted_vertices.rend(); ++vit) {
    // If vertex is a function, condition, or init task
    Task * task = graph_->getTask((*vit));
    if (task->getTaskType() == Task::xfunction ||
        task->getTaskType() == Task::xcondition ||
        task->getTaskType() == Task::start_agent ||
        task->getTaskType() == Task::finish_agent) {
      // Copy variable writing and reading vertices
      // from in coming vertices
      copyWritingAndReadingVerticesFromInEdges(*vit, task);
      // Add dependencies on writing vertices that this vertex reads
      addReadDependencies(*vit, task);
      // Add dependencies on reading vertices that this vertex writes
      addWriteDependencies(*vit, task);
      // Add vertices to variable writing vertices list
      // for writing variables
      addWritingVerticesToList(*vit, task);
    }
  }
}

}}   // namespace flame::model
