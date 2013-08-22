/*!
 * \file flame2/model/stategraph.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief StateGraph: holds state graph
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
#include "stategraph.hpp"
#include "xcondition.hpp"
#include "xfunction.hpp"
#include "model_task.hpp"

namespace flame { namespace model {

StateGraph::StateGraph() : graph_(), name_() {}

EdgeMap * StateGraph::getEdgeDependencyMap() {
  return graph_.getEdgeDependencyMap();
}

void StateGraph::setName(std::string name) {
  name_ = name;
}

ModelTask * StateGraph::generateStateGraphStatesAddStateToGraph(
    std::string name, std::string startState) {
  size_t ii;
  // Check if state has already been added
  for (ii = 0; ii < graph_.getTaskCount(); ++ii)
    if (graph_.getTask(ii)->getTaskType() == ModelTask::xstate &&
        graph_.getTask(ii)->getName() == name) return graph_.getTask(ii);

  // Add state as a task to the task list
  ModelTask * task = new ModelTask(name_, name, ModelTask::xstate);
  graph_.addVertex(task);
  if (name == startState) task->setStartTask(true);

  return task;
}

void StateGraph::generateStateGraphStates(
    XFunction * function, ModelTask * task, std::string startState) {
  std::set<std::string>::iterator sit;
  // Add and get state tasks
  ModelTask * currentState =
      generateStateGraphStatesAddStateToGraph(
          function->getCurrentState(), startState);
  ModelTask * nextState =
      generateStateGraphStatesAddStateToGraph(
          function->getNextState(), startState);
  // Add edge from current state to function
  graph_.addEdge(graph_.getVertex(currentState), graph_.getVertex(task),
      function->getCurrentState(), Dependency::state);
  // Add edge from function to next state
  graph_.addEdge(graph_.getVertex(task), graph_.getVertex(nextState),
      function->getNextState(), Dependency::state);
  // If transition has a condition
  if (function->getCondition()) {
    // Set task has a condition
    task->setHasCondition(true);
    // Get condition read variables
    std::set<std::string> * rov =
        function->getCondition()->getReadOnlyVariables();
    // For each condition read variable add to current state
    // read variables
    for (sit = rov->begin(); sit != rov->end(); ++sit)
      currentState->addReadOnlyVariable(*sit);
  }
}

void StateGraph::generateStateGraphVariables(
    XFunction * function, ModelTask * task) {
  std::vector<std::string>::iterator sitv;
  // For each read only variable
  for (sitv = function->getReadOnlyVariables()->begin();
      sitv != function->getReadOnlyVariables()->end(); ++sitv)
    // Add to task read only variables
    task->addReadOnlyVariable(*sitv);
  // For each read write variable
  for (sitv = function->getReadWriteVariables()->begin();
      sitv != function->getReadWriteVariables()->end(); ++sitv)
    // Add to task read write variables
    task->addReadWriteVariable(*sitv);
}

ModelTask * StateGraph::generateStateGraphMessagesAddMessageToGraph(
    std::string name) {
  size_t ii;
  // Check if message has already been added
  for (ii = 0; ii < graph_.getTaskCount(); ++ii)
    if (graph_.getTask(ii)->getTaskType() == ModelTask::xmessage &&
        graph_.getTask(ii)->getName() == name) return graph_.getTask(ii);

  // Add message as a task to the task list
  ModelTask * task = new ModelTask(name, name, ModelTask::xmessage);
  graph_.addVertex(task);
  return task;
}

void StateGraph::generateStateGraphMessages(
    XFunction * function, ModelTask * task) {
  boost::ptr_vector<XIOput>::iterator ioput;
  // Find outputting functions
  for (ioput = function->getOutputs()->begin();
      ioput != function->getOutputs()->end(); ++ioput) {
    // Add output message to function task
    task->addOutputMessage((*ioput).getMessageName());
    // Add edge from function vertex to message vertex
    graph_.addEdge(graph_.getVertex(task),
        graph_.getVertex(generateStateGraphMessagesAddMessageToGraph(
            (*ioput).getMessageName())),
            (*ioput).getMessageName(), Dependency::communication);
  }
  // Find inputting functions
  for (ioput = function->getInputs()->begin();
      ioput != function->getInputs()->end(); ++ioput) {
    // Add input message to function task
    task->addInputMessage((*ioput).getMessageName());
    // Add egde from message vertex to function vertex
    graph_.addEdge(graph_.getVertex(generateStateGraphMessagesAddMessageToGraph(
        (*ioput).getMessageName())),
        graph_.getVertex(task),
        (*ioput).getMessageName(), Dependency::communication);
  }
}

void StateGraph::generateStateGraph(boost::ptr_vector<XFunction> * functions,
    std::string startState, std::set<std::string> endStates) {
  boost::ptr_vector<XFunction>::iterator fit;

  // For each transition function
  for (fit = functions->begin(); fit != functions->end(); ++fit) {
    // Add function as a task to the task list
    ModelTask * functionTask = new ModelTask(name_,
        (*fit).getName(), ModelTask::xfunction);
    // Add vertex for task
    graph_.addVertex(functionTask);
    // Add states
    generateStateGraphStates(&(*fit), functionTask, startState);
    // Add variable read writes
    generateStateGraphVariables(&(*fit), functionTask);
    // Add communication
    generateStateGraphMessages(&(*fit), functionTask);
    // If function next state is an end state
    if (endStates.find((*fit).getNextState()) != endStates.end())
    functionTask->setEndTask(true);
  }
}

void StateGraph::importStateGraphTasks(StateGraph * graph,
    std::map<std::string, Vertex> * message2task,
    std::map<Vertex, Vertex> * import2new) {
  size_t ii;
  std::map<std::string, Vertex>::iterator it;

  const TaskList * tasklist = graph->getTaskList();
  // For each task vertex map
  for (ii = 0; ii < tasklist->getTaskCount(); ++ii) {
    // get task
    ModelTask * t = tasklist->getTask(ii);
    // if message task
    if (t->getTaskType() == ModelTask::xmessage) {
      // get task name
      std::string name = t->getName();
      // try and find existing message task
      it = message2task->find(name);
      if (it == message2task->end()) {
        // if not found
        // add vertex to current graph
        Vertex v = graph_.addVertex(tasklist->getTaskPtr(ii));
        // add vertex to vertex map
        import2new->insert(std::make_pair(ii, v));
        // add vertex to message map
        message2task->insert(std::make_pair(name, v));
      } else {
        // add to vertex to vertex map
        import2new->insert(std::make_pair(ii, (*it).second));
      }
    } else {
      // add vertex to current graph
      Vertex v = graph_.addVertex(tasklist->getTaskPtr(ii));
      // add to vertex to vertex map
      import2new->insert(std::make_pair(ii, v));
    }
  }
}

void StateGraph::importStateGraphs(std::set<StateGraph*> graphs) {
  // message to task map used to check existing message tasks
  std::map<std::string, Vertex> message2task;
  std::set<StateGraph*>::iterator it;
  EdgeIterator eit, end;

  // import each graph
  for (it = graphs.begin(); it != graphs.end(); ++it) {
    std::map<Vertex, Vertex> import2new;
    // import tasks
    importStateGraphTasks((*it), &message2task, &import2new);
    // import edges
    for (boost::tie(eit, end) = (*it)->getEdges();
        eit != end; ++eit) {
      // add edge using vertex to vertex map
      Vertex s = (*it)->getEdgeSource(*eit);
      Vertex t = (*it)->getEdgeTarget(*eit);
      Vertex ns = (*(import2new.find(s))).second;
      Vertex nt = (*(import2new.find(t))).second;
      graph_.addEdge(ns, nt, "", Dependency::blank);
    }
  }
}

std::pair<int, std::string> StateGraph::checkCyclicDependencies() {
  return graph_.checkCyclicDependencies();
}

std::pair<int, std::string> StateGraph::checkFunctionConditions() {
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

void StateGraph::writeGraphviz(const std::string& fileName) const {
  graph_.writeGraphviz(fileName);
}

Vertex StateGraph::getEdgeSource(Edge e) {
  return graph_.getEdgeSource(e);
}

Vertex StateGraph::getEdgeTarget(Edge e) {
  return graph_.getEdgeTarget(e);
}

std::pair<EdgeIterator, EdgeIterator> StateGraph::getEdges() {
  return graph_.getEdges();
}

const TaskList * StateGraph::getTaskList() const {
  return graph_.getTaskList();
}

}}   // namespace flame::model
