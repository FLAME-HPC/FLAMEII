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
#include "task.hpp"

namespace flame { namespace model {

StateGraph::StateGraph() {
  // Initialise pointers
  graph_ = new Graph;
  vertex2task_ = new std::vector<TaskPtr>;
  edge2dependency_ = new EdgeMap;
  startTask_ = 0;
}

StateGraph::~StateGraph() {
  std::vector<Task *>::iterator vit;
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

std::vector<TaskPtr> * StateGraph::getVertexTaskMap() {
  return vertex2task_;
}

EdgeMap * StateGraph::getEdgeDependencyMap() {
  return edge2dependency_;
}

Task * StateGraph::getStartTask() {
  return startTask_;
}

std::set<Task*> StateGraph::getEndTasks() {
  return endTasks_;
}

void StateGraph::setAgentName(std::string agentName) {
  agentName_ = agentName;
}

Vertex StateGraph::addVertex(Task * t) {
  // Add vertex to graph
  Vertex v = add_vertex(*graph_);
  // create new shared ptr for Task pointer
  TaskPtr ptr(t);
  // Add task to vertex task mapping
  vertex2task_->push_back(ptr);
  // Return vertex
  return v;
}

Vertex StateGraph::addVertex(TaskPtr ptr) {
  // Add vertex to graph
  Vertex v = add_vertex(*graph_);
  // Add task to vertex task mapping
  vertex2task_->push_back(ptr);
  // Return vertex
  return v;
}

void StateGraph::removeVertex(Vertex v) {
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
void StateGraph::removeVertices(std::vector<Vertex> * tasks) {
  std::vector<Vertex>::iterator vit;
  // Sort vertices largest first
  std::sort(tasks->begin(), tasks->end(), std::greater<size_t>());
  // Remove tasks in order
  for (vit = tasks->begin(); vit != tasks->end(); ++vit)
    removeVertex((*vit));
}

void StateGraph::removeDependency(Edge e) {
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

Edge StateGraph::addEdge(Vertex to, Vertex from, std::string name,
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

Vertex StateGraph::getVertex(Task * t) {
  size_t ii;
  // Find index of task in vertex task mapping
  // The index corresponds to the vertex number
  for (ii = 0; ii < vertex2task_->size(); ++ii)
    if (vertex2task_->at(ii).get() == t) return ii;
  return 0;
}

Task * StateGraph::getTask(Vertex v) const {
  int index = static_cast<int>(v);

  if (index < 0 || index >= static_cast<int>(vertex2task_->size()))
    throw flame::exceptions::flame_model_exception(
      "Task id does not exist");

  // Return task at index v
  return vertex2task_->at(v).get();
}

Dependency * StateGraph::getDependency(Edge e) {
  Dependency * d = edge2dependency_->find(e)->second;

  if (d == 0) throw flame::exceptions::flame_model_exception(
      "Model graph dependency not initialised");

  // Return dependency associated with the edge
  return d;
}

Task * StateGraph::generateStateGraphStatesAddStateToGraph(std::string name,
    std::string startState) {
  // Check if state has already been added
  std::vector<TaskPtr>::iterator vit;
  for (vit = vertex2task_->begin(); vit != vertex2task_->end(); ++vit)
    if ((*vit).get()->getTaskType() == Task::xstate &&
        (*vit).get()->getName() == name) return (*vit).get();

  // Add state as a task to the task list
  Task * task = new Task(agentName_, name, Task::xstate);
  addVertex(task);
  if (name == startState) startTask_ = task;

  return task;
}

void StateGraph::generateStateGraphStates(XFunction * function, Task * task,
    std::string startState) {
  std::set<std::string>::iterator sit;
  // Add and get state tasks
  Task * currentState =
      generateStateGraphStatesAddStateToGraph(
          function->getCurrentState(), startState);
  Task * nextState =
      generateStateGraphStatesAddStateToGraph(
          function->getNextState(), startState);
  // Add edge from current state to function
  addEdge(getVertex(currentState), getVertex(task),
      function->getCurrentState(), Dependency::state);
  // Add edge from function to next state
  addEdge(getVertex(task), getVertex(nextState),
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
    XFunction * function, Task * task) {
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

Task * StateGraph::generateStateGraphMessagesAddMessageToGraph(
    std::string name) {
  // Check if state has already been added
  std::vector<TaskPtr>::iterator vit;
  for (vit = vertex2task_->begin(); vit != vertex2task_->end(); ++vit)
    if ((*vit).get()->getTaskType() == Task::xmessage &&
        (*vit).get()->getName() == name) return (*vit).get();

  // Add state as a task to the task list
  Task * task = new Task(name, name, Task::xmessage);
  addVertex(task);
  return task;
}

void StateGraph::generateStateGraphMessages(XFunction * function, Task * task) {
  boost::ptr_vector<XIOput>::iterator ioput;
  // Find outputting functions
  for (ioput = function->getOutputs()->begin();
      ioput != function->getOutputs()->end(); ++ioput) {
    // Add output message to function task
    task->addOutputMessage((*ioput).getMessageName());
    // Add edge from function vertex to message vertex
    addEdge(getVertex(task),
        getVertex(generateStateGraphMessagesAddMessageToGraph(
            (*ioput).getMessageName())),
            (*ioput).getMessageName(), Dependency::communication);
  }
  // Find inputting functions
  for (ioput = function->getInputs()->begin();
      ioput != function->getInputs()->end(); ++ioput) {
    // Add input message to function task
    task->addInputMessage((*ioput).getMessageName());
    // Add egde from message vertex to function vertex
    addEdge(getVertex(generateStateGraphMessagesAddMessageToGraph(
        (*ioput).getMessageName())),
        getVertex(task),
        (*ioput).getMessageName(), Dependency::communication);
  }
}

int StateGraph::generateStateGraph(boost::ptr_vector<XFunction> * functions,
    std::string startState, std::set<std::string> endStates) {
  boost::ptr_vector<XFunction>::iterator fit;

  // For each transition function
  for (fit = functions->begin(); fit != functions->end(); ++fit) {
    // Add function as a task to the task list
    Task * functionTask = new Task(agentName_,
        (*fit).getName(), Task::xfunction);
    // Add vertex for task
    addVertex(functionTask);
    // Add states
    generateStateGraphStates(&(*fit), functionTask, startState);
    // Add variable read writes
    generateStateGraphVariables(&(*fit), functionTask);
    // Add communication
    generateStateGraphMessages(&(*fit), functionTask);
    // If function next state is an end state
    if (endStates.find((*fit).getNextState()) != endStates.end())
      // Add function task to end tasks list
      endTasks_.insert(functionTask);
  }

  return 0;
}

void StateGraph::setStartTask(Task * task) {
  startTask_ = task;
}

void StateGraph::importStateGraphTasks(StateGraph * graph,
    std::map<std::string, Vertex> * message2task,
    std::map<Vertex, Vertex> * import2new) {
  std::vector<TaskPtr> * v2t = graph->getVertexTaskMap();
  size_t ii;
  std::map<std::string, Vertex>::iterator it;

  // For each task vertex map
  for (ii = 0; ii < v2t->size(); ++ii) {
    // get task
    Task * t = v2t->at(ii).get();
    // if message task
    if (t->getTaskType() == Task::xmessage) {
      // get task name
      std::string name = t->getName();
      // try and find existing message task
      it = message2task->find(name);
      if (it == message2task->end()) {
        // if not found
        // add vertex to current graph
        Vertex v = addVertex(v2t->at(ii));
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
      Vertex v = addVertex(v2t->at(ii));
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
    for (boost::tie(eit, end) = boost::edges(*((*it)->getGraph()));
        eit != end; ++eit) {
      // add edge using vertex to vertex map
      Vertex s = boost::source(*eit, *((*it)->getGraph()));
      Vertex t = boost::target(*eit, *((*it)->getGraph()));
      Vertex ns = (*(import2new.find(s))).second;
      Vertex nt = (*(import2new.find(t))).second;
      add_edge(ns, nt, *graph_);
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

std::pair<int, std::string> StateGraph::checkCyclicDependencies() {
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
    Task * t1 = getTask(boost::source(err.edge(), *graph_));
    Task * t2 = getTask(boost::target(err.edge(), *graph_));
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

std::pair<int, std::string> StateGraph::checkFunctionConditions() {
  // error message
  std::string error_msg;
  std::pair<VertexIterator, VertexIterator> vp;
  // For each vertex
  for (vp = boost::vertices(*graph_); vp.first != vp.second; ++vp.first) {
    // If state
    if (getTask(*vp.first)->getTaskType() == Task::xstate) {
      // If out edges is larger than 1
      if (boost::out_degree(*vp.first, *graph_) > 1) {
        boost::graph_traits<Graph>::out_edge_iterator oei, oei_end;
        // For each out edge
        for (boost::tie(oei, oei_end) =
            boost::out_edges(*vp.first, *graph_);
            oei != oei_end; ++oei) {
          Task * t = getTask(boost::target((Edge)*oei, *graph_));
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

void StateGraph::writeGraphviz(const std::string& fileName) const {
  std::fstream graphfile;
  graphfile.open(fileName.c_str(), std::fstream::out);

  boost::write_graphviz(graphfile, *graph_,
      vertex_label_writer(vertex2task_),
      edge_label_writer(edge2dependency_,
          edge_label_writer::arrowForward),
          graph_writer());

  graphfile.clear();
}

}}   // namespace flame::model
