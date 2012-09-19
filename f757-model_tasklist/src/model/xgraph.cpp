/*!
 * \file src/model/xgraph.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XGraph: holds graph information
 */
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/transitive_reduction.hpp>
#include <boost/tuple/tuple.hpp>
#include <string>
#include <vector>
#include <set>
#include <utility>
#include <exception>
#include <functional>  // For greater<>
#include <algorithm>   // For sort
#include "./xgraph.hpp"
#include "./xcondition.hpp"
#include "./xfunction.hpp"
#include "./task.hpp"

namespace flame { namespace model {

XGraph::XGraph() {
    graph_ = new Graph;
    vertex2task_ = new std::vector<Task *>;
    edge2dependency_ = new EdgeMap;
}

XGraph::~XGraph() {
    /* Free tasks and dependencies memory */
    std::vector<Task *>::iterator vit;
    for (vit = vertex2task_->begin(); vit != vertex2task_->end(); vit++)
        delete (*vit);
    EdgeMap::iterator eit;
    for (eit = edge2dependency_->begin(); eit != edge2dependency_->end(); ++eit)
        delete ((*eit).second);
    // Free graph
    delete graph_;
}

int XGraph::generateDependencyGraph(std::vector<XVariable*> * variables) {
    write_graphviz("test1.dot");

    // Add condition vertices
    addConditionVertices();

    // Contract state vertices
    contractStateVertices();

    write_graphviz("test2.dot");

    // Add condition dependencies
    add_condition_dependencies();
    // Add variable vertices
    add_variable_vertices_to_graph(variables);
    // Remove state dependencies
    remove_state_dependencies();

    write_graphviz("test3.dot");

    // Add data io
    AddVariableOutput(variables);

    // Contract variable vertices
    contract_variable_vertices_from_graph();

    write_graphviz("test4.dot");

    remove_redendant_dependencies();

    write_graphviz("test5.dot");

    return 0;
}

Task * XGraph::addStateToGraph(std::string name, std::string startState) {
    // Check if state has already been added
    std::vector<Task *>::iterator vit;
    for (vit = vertex2task_->begin(); vit != vertex2task_->end(); ++vit)
        if ((*vit)->getTaskType() == Task::xstate &&
                (*vit)->getName() == name) return (*vit);

    // Add state as a task to the task list
    Task * task = new Task;
    task->setName(name);
    task->setTaskType(Task::xstate);
    Vertex v = addVertex(task);
    if (name == startState) startVertex_ = v;
    return task;
}

Task * XGraph::addMessageToGraph(std::string name) {
    // Check if state has already been added
    std::vector<Task *>::iterator vit;
    for (vit = vertex2task_->begin(); vit != vertex2task_->end(); ++vit)
        if ((*vit)->getTaskType() == Task::xmessage &&
                (*vit)->getName() == name) return (*vit);

    // Add state as a task to the task list
    Task * task = new Task;
    task->setName(name);
    task->setTaskType(Task::xmessage);
    addVertex(task);
    return task;
}

int XGraph::generateStateGraph(std::vector<XFunction*> functions,
        std::string startState) {
    std::vector<XFunction*>::iterator fit;
    std::set<std::string>::iterator sit;
    std::vector<std::string>::iterator sitv;
    Dependency * d;
    // First add function tasks as the order of these should not change
    // when vertices are deleted
    for (fit = functions.begin(); fit != functions.end(); fit++) {
        // Add function as a task to the task list
        Task * functionTask = new Task;
        // task->setParentName(getName());
        functionTask->setName((*fit)->getName());
        functionTask->setTaskType(Task::xfunction);
        functionTask->setPriorityLevel(5);
        // Associate task with function
        addVertex(functionTask);
        // Add states
        Task * currentState =
                addStateToGraph((*fit)->getCurrentState(), startState);
        Task * nextState =
                addStateToGraph((*fit)->getNextState(), startState);
        d = new Dependency;
        d->setDependencyType(Dependency::state);
        addEdge(currentState, functionTask, d);
        d = new Dependency;
        d->setDependencyType(Dependency::state);
        addEdge(functionTask, nextState, d);
        // If transition has a condition, add condition variables
        // to current state variable reads
        if ((*fit)->getCondition()) {
            functionTask->setHasCondition(true);
            for (sit = (*fit)->getCondition()->getReadOnlyVariables()->begin();
                sit != (*fit)->getCondition()->getReadOnlyVariables()->end(); sit++)
                currentState->addReadVariable(*sit);
        }
        // Add variable read writes
        for (sitv = (*fit)->getReadOnlyVariables()->begin();
                sitv != (*fit)->getReadOnlyVariables()->end(); sitv++)
            functionTask->addReadVariable(*sitv);
        for (sitv = (*fit)->getReadWriteVariables()->begin();
                sitv != (*fit)->getReadWriteVariables()->end(); sitv++) {
            functionTask->addReadVariable(*sitv);
            functionTask->addWriteVariable(*sitv);
        }
        // Add communication
        std::vector<XIOput*>::iterator ioput;
        std::vector<XMessage*>::iterator message;
        /* Find outputting functions */
        for (ioput = (*fit)->getOutputs()->begin();
             ioput != (*fit)->getOutputs()->end(); ++ioput) {
            Task * message = addMessageToGraph((*ioput)->getMessageName());
            /* Add communication dependency */
            Dependency * d = new Dependency;
            d->setName((*ioput)->getMessageName());
            d->setDependencyType(Dependency::communication);
            addEdge(functionTask, message, d);
        }

        /* Find inputting functions */
        for (ioput = (*fit)->getInputs()->begin();
             ioput != (*fit)->getInputs()->end(); ++ioput) {
            Task * message = addMessageToGraph((*ioput)->getMessageName());
            /* Add communication dependency */
            Dependency * d = new Dependency;
            d->setName((*ioput)->getMessageName());
            d->setDependencyType(Dependency::communication);
            addEdge(message, functionTask, d);
        }
    }

    return 0;
}

Vertex XGraph::addVertex(Task * t) {
    Vertex v = add_vertex(*graph_);
    vertex2task_->push_back(t);
    return v;
}

void XGraph::removeTask(Vertex v) {
    boost::graph_traits<Graph>::out_edge_iterator oei, oei_end;
    boost::graph_traits<Graph>::in_edge_iterator iei, iei_end;
    std::set<Edge>::iterator eit;
    // Remove edges of vertex
    std::set<Edge> edgesToRemove;
    for (boost::tie(iei, iei_end) = boost::in_edges(v, *graph_);
            iei != iei_end; ++iei)
        edgesToRemove.insert((Edge)*iei);
    for (boost::tie(oei, oei_end) = boost::out_edges(v, *graph_);
            oei != oei_end; ++oei)
        edgesToRemove.insert((Edge)*oei);
    for (eit = edgesToRemove.begin(); eit != edgesToRemove.end(); eit++)
            removeDependency(*eit);

    // Free task
    delete vertex2task_->at(v);
    // Remove task from mapping
    vertex2task_->erase(vertex2task_->begin() + v);
    // Remove edge from graph
    boost::remove_vertex(v, *graph_);
}

// Tasks removed largest first so that indexes are not changed
void XGraph::removeTasks(std::vector<Vertex> * tasks) {
    std::vector<Vertex>::iterator vit;
    // Sort vertices largest first
    std::sort(tasks->begin(), tasks->end(), std::greater<size_t>());
    // Remove tasks in order
    for (vit = tasks->begin(); vit != tasks->end(); ++vit)
        removeTask((*vit));
}

void XGraph::removeDependency(Edge e) {
    edge2dependency_->erase(edge2dependency_->find(e));
    // Remove edge from graph
    boost::remove_edge(e, *graph_);
}

Edge XGraph::addEdge(Vertex to, Vertex from, Dependency * d) {
    std::pair<Edge, bool> e = add_edge(to, from, *graph_);
    edge2dependency_->insert(std::make_pair(e.first, d));
    return e.first;
}

void XGraph::addEdge(Task * to, Task * from, Dependency * d) {
    addEdge(getVertex(to), getVertex(from), d);
}

void XGraph::addEdge(Task * to, Task * from, Dependency::DependencyType type) {
    Dependency * d = new Dependency;
    d->setDependencyType(type);
    addEdge(to, from, d);
}

Vertex XGraph::getVertex(Task * t) {
    std::vector<Task *>::iterator it;
    size_t ii;

    for (ii = 0; ii < vertex2task_->size(); ii++)
        if (vertex2task_->at(ii) == t) return ii;
    return 0;
}

Task * XGraph::getTask(Vertex v) {
    return vertex2task_->at(v);
}

Dependency * XGraph::getDependency(Edge e) {
    return edge2dependency_->find(e)->second;
}

struct func_layer : public boost::default_bfs_visitor {
    explicit func_layer(std::vector<Vertex> * bfv) :
            breadth_first_vertices_(bfv) {}
    // Vertex discovered when traversing out edges
    void discover_vertex(Vertex u, const Graph & g) const {
        // Add vertex to end of verticies vector
        breadth_first_vertices_->push_back(u);
    //    std::cout << "Discover: " << u << std::endl;
    }
    // Vertex finished when all of its out edge vertices traversed
    void finish_vertex(Vertex u, const Graph & g) const {
    //    std::cout << "Finish:   " << u << std::endl;
    }
  protected:
    std::vector<Vertex> * breadth_first_vertices_;
};

bool containsVariableName(std::set<std::string> variables,
        std::string variable) {
    std::set<std::string>::iterator it;
    for (it = variables.begin(); it != variables.end(); it++)
        if ((*it) == variable) return true;
    return false;
}

void XGraph::discover_last_variable_writes(std::string variable,
        Vertex vertex,
        std::set<Vertex> * finished,
        std::set<Vertex> * writing) {
    boost::graph_traits<Graph>::in_edge_iterator iei, iei_end;

    // If vertex has already been seen then skip
    if (finished->find(vertex) == finished->end()) {
        for (boost::tie(iei, iei_end) = boost::in_edges(vertex, *graph_);
                            iei != iei_end; ++iei) {
            // Only if edge dependency is state or condition
            Dependency * d = getDependency((Edge)*iei);
            if (d->getDependencyType() == Dependency::state ||
                    d->getDependencyType() == Dependency::condition ||
                    d->getDependencyType() == Dependency::init) {
                // Get source vertex of the in edge
                Vertex v = boost::source((Edge)*iei, *graph_);

                if (containsVariableName(getTask(v)->getWriteVariables(),
                        variable))
                    // If vertex writes variable then
                    // add vertex to writing vertices
                    writing->insert(v);
                else
                    // If vertex does not write variable
                    // then try higher vertices
                    discover_last_variable_writes(
                            variable, v, finished, writing);
            }
        }
        // Add vertex to finished list
        finished->insert(vertex);
    }
}

void XGraph::add_variable_vertices_to_graph(
        std::vector<XVariable*> * variables) {
    std::vector<Vertex> breadth_first_vertices;
    std::vector<Vertex>::iterator vit;
    std::set<Vertex>::iterator sit;
    std::set<std::string>::iterator varit;
    std::pair<VertexIterator, VertexIterator> vp;

    std::set<Vertex> finished;
    std::set<Vertex> writing;

    // Add init function to provide first writes of all variables
    Task * initTask = new Task;
    initTask->setName("init");
    initTask->setTaskType(Task::init_agent);
    Vertex initVertex = addVertex(initTask);
    // Add edge from init to start vertex
    Dependency * d = new Dependency;
    d->setDependencyType(Dependency::init);
    addEdge(initVertex, startVertex_, d);
    std::vector<XVariable*>::iterator i;
    for (i = variables->begin(); i != variables->end(); i++)
        initTask->addWriteVariable((*i)->getName());

    func_layer vis(&breadth_first_vertices);
    boost::breadth_first_search(*graph_, initVertex, boost::visitor(vis));

    // For each vertex in depth order
    for (vit = breadth_first_vertices.begin();
         vit != breadth_first_vertices.end(); vit++) {
        // If vertex is a function or condition
        if (getTask(*vit)->getTaskType() == Task::xfunction ||
                getTask(*vit)->getTaskType() == Task::xcondition ||
                getTask(*vit)->getTaskType() == Task::init_agent) {
            std::set<std::string> rov =
                getTask(*vit)->getReadVariables();
            std::set<std::string> rwv =
                getTask(*vit)->getWriteVariables();

            // For writing variables create new vertex and add edge
            for (varit = rwv.begin(); varit != rwv.end(); varit++) {
                // Add out going edge to new variable vertex
                Task * varTask = new Task;
                varTask->setName((*varit));
                varTask->setTaskType(Task::xvariable);
                Vertex variableVertex = addVertex(varTask);
                Dependency * d = new Dependency;
                d->setName((*varit));
                d->setDependencyType(Dependency::variable);
                addEdge(*vit, variableVertex, d);
            }

            // Link reading variables to current variable vertex
            for (varit = rov.begin(); varit != rov.end(); varit++) {
                finished.clear();
                writing.clear();

                discover_last_variable_writes(*varit, *vit,
                        &finished, &writing);

                // For each writing vertex
                for (sit = writing.begin(); sit != writing.end(); ++sit) {
                    // Find out edge for variable
                    boost::graph_traits<Graph>::out_edge_iterator
                        oei, oei_end;
                    for (boost::tie(oei, oei_end) =
                            boost::out_edges(*sit, *graph_);
                                oei != oei_end; ++oei) {
                        Vertex v2 = boost::target((Edge)*oei, *graph_);
                        if (getTask(v2)->getTaskType() == Task::xvariable &&
                                getTask(v2)->getName() == (*varit)) {
                            Dependency * d = new Dependency;
                            d->setName((*varit));
                            d->setDependencyType(Dependency::variable);
                            addEdge(v2, (*vit), d);
                        }
                    }
                }
            }
        }
    }

    // Remove init vertex
    removeTask(initVertex);
}

void XGraph::discover_conditions(Vertex vertex, Vertex current,
        std::set<Vertex> * conditions) {
    boost::graph_traits<Graph>::in_edge_iterator iei, iei_end;

    // For each in edge
    for (boost::tie(iei, iei_end) = boost::in_edges(current, *graph_);
        iei != iei_end; ++iei) {
        // Get source vertex of the in edge
        Vertex v = boost::source((Edge)*iei, *graph_);
        // If vertex is a condition then depend on it
        if (getTask(v)->getTaskType() == Task::xcondition)
            conditions->insert(v);

        discover_conditions(vertex, v, conditions);
    }
}

void XGraph::add_condition_dependencies() {
    std::pair<VertexIterator, VertexIterator> vp;
    std::set<Vertex> conditions;
    std::set<Vertex>::iterator cit;
    // For each function/condition vertex
    for (vp = boost::vertices(*graph_); vp.first != vp.second; ++vp.first) {
        if (getTask(*vp.first)->getTaskType() == Task::xfunction ||
                getTask(*vp.first)->getTaskType() == Task::xcondition) {
            conditions.clear();
            discover_conditions(*vp.first, *vp.first, &conditions);
            // Add edge for each condition vertex found
            for (cit = conditions.begin(); cit != conditions.end(); cit++)
                addEdge(getTask(*cit),
                        getTask(*vp.first), Dependency::condition);
        }
    }
}

void XGraph::AddVariableOutput(std::vector<XVariable*> * variables) {
    // For each function that last writes a variable add dependency
    // to the data output of that variable
    std::pair<VertexIterator, VertexIterator> vp;
    std::vector<XVariable*>::iterator i;
    std::set<std::string>::iterator varit;
    std::vector<Vertex> vertexToDelete;
    // For each variable
    for (i = variables->begin(); i != variables->end(); i++) {
        // Create node
        Task * varTask = new Task;
        varTask->setName((*i)->getName());
        varTask->setTaskType(Task::io_pop_write);
        Vertex variableVertex = addVertex(varTask);
        bool isWritten = false;
        // Add dependency on every writing function
        for (vp = boost::vertices(*graph_); vp.first != vp.second; ++vp.first) {
            if (getTask(*vp.first)->getTaskType() == Task::xfunction) {
                std::set<std::string> rwv =
                    getTask(*vp.first)->getWriteVariables();

                // For writing variables create new vertex and add edge
                for (varit = rwv.begin(); varit != rwv.end(); varit++) {
                    if (*varit == (*i)->getName()) {
                        Dependency * d = new Dependency;
                        d->setName((*varit));
                        d->setDependencyType(Dependency::data);
                        addEdge(*vp.first, variableVertex, d);
                        isWritten = true;
                    }
                }
            }
        }
        // If variable not written then remove node from graph
        if (!isWritten) vertexToDelete.push_back(variableVertex);
    }
    // Delete vertices in delete list
    removeTasks(&vertexToDelete);
}

void XGraph::remove_state_dependencies() {
    EdgeIterator eit, eit_end;
    std::set<Edge> edgesToRemove;
    std::set<Edge>::iterator etrit;

    for (boost::tie(eit, eit_end) = boost::edges(*graph_);
            eit != eit_end; ++eit) {
        // Only if edge dependency is state or condition
        Dependency * d = getDependency((Edge)*eit);
        if (d->getDependencyType() == Dependency::state)
            edgesToRemove.insert((Edge)*eit);
    }
    for (etrit = edgesToRemove.begin(); etrit != edgesToRemove.end(); etrit++)
        removeDependency(*etrit);
}

struct add_branch_vertices : public boost::default_bfs_visitor {
    // Vertex discovered when traversing out edges
    void discover_vertex(Vertex u, const Graph & g) const {
        std::cout << "Discover: " << u << std::endl;
    }
    // Vertex finished when all of its out edge vertices traversed
    void finish_vertex(Vertex u, const Graph & g) const {
        std::cout << "Finish:   " << u << std::endl;
    }
};

void XGraph::addConditionVertices() {
    std::pair<VertexIterator, VertexIterator> vp;
    boost::graph_traits<Graph>::out_edge_iterator oei, oei_end;
    // For each vertex
    for (vp = boost::vertices(*graph_); vp.first != vp.second; ++vp.first) {
        // If out edges is larger than 1 and an xfunction task
        if (boost::out_degree(*vp.first, *graph_) > 1 &&
                getTask(*vp.first)->getTaskType() == Task::xstate) {
            getTask(*vp.first)->setTaskType(Task::xcondition);
            getTask(*vp.first)->setName("Condition");
        }
    }
}

void XGraph::setStartVector(Vertex sv) {
    startVertex_ = sv;
}

void XGraph::contractVertices(
        Task::TaskType taskType, Dependency::DependencyType dependencyType) {
    VertexIterator vi, vi_end;
    boost::graph_traits<Graph>::out_edge_iterator oei, oei_end;
    boost::graph_traits<Graph>::in_edge_iterator iei, iei_end;
    std::vector<Vertex> vertexToDelete;
    std::vector<Vertex>::iterator vit;
    // For each variable vertex
    for (boost::tie(vi, vi_end) = boost::vertices(*graph_);
            vi != vi_end; ++vi) {
        // If vertex is a variable
        if (getTask(*vi)->getTaskType() == taskType) {
            // Add an edge from all vertex sources to all vertex targets
            for (boost::tie(iei, iei_end) = boost::in_edges(*vi, *graph_);
                    iei != iei_end; ++iei)
                for (boost::tie(oei, oei_end) = boost::out_edges(*vi, *graph_);
                        oei != oei_end; ++oei)
                    addEdge(getTask(boost::source((Edge)*iei, *graph_)),
                            getTask(boost::target((Edge)*oei, *graph_)),
                            dependencyType);
            // Add vertex to delete list (as cannot delete vertex within
            // an iterator)
            vertexToDelete.push_back(*vi);
        }
    }
    // Delete vertices in delete list
    removeTasks(&vertexToDelete);
}

void XGraph::contractStateVertices() {
    // Contract state tasks and replace with state dependency
    contractVertices(Task::xstate, Dependency::state);
}

void XGraph::contract_variable_vertices_from_graph() {
    // Contract variable tasks and replace with variable dependency
    contractVertices(Task::xvariable, Dependency::variable);
}

void XGraph::remove_redendant_dependencies() {
    size_t ii;
    // The resultant transitive reduction graph
    Graph * trgraph = new Graph;
    std::vector<Task *> * trvertex2task =
            new std::vector<Task *>(vertex2task_->size());

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

    transitive_reduction(*graph_, *trgraph, g_to_tc_map, index_map);

    // Create new vertex task mapping for trgraph
    for (ii = 0; ii < boost::num_vertices(*graph_); ii++)
        trvertex2task->at(to_tc_vec[ii]) = vertex2task_->at(ii);

    // Make graph_ point to trgraph
    delete graph_;
    graph_ = trgraph;
    // Make vertex2task_ point to trvertex2task
    delete vertex2task_;
    vertex2task_ = trvertex2task;
    // Clear edge2dependency_ as edges no longer valid
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

int XGraph::checkCyclicDependencies() {
    // Visitor cycle detector for use with depth_first_search
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
        std::fprintf(stderr,
                    "Error: cycle detected %s -> %s -> %s\n",
                    t1->getFullName().c_str(),
                    d->getName().c_str(),
                    t2->getFullName().c_str());
        return 1;
    }

    return 0;
}

int XGraph::checkFunctionConditions() {
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
                        std::fprintf(stderr,
            "Error: Function '%s' from a state with more than one %s\n",
                        t->getName().c_str(),
                        "out going function does not have a condition.");
                        return 1;
                    }
                }
            }
        }
    }

    return 0;
}

struct vertex_label_writer {
    explicit vertex_label_writer(std::vector<Task *> * vm) : vertex2task(vm) {}
    void operator()(std::ostream& out, const Vertex& v) const {
        Task * t = vertex2task->at(v);
        out << " [label=\"" << t->getName() << "\"";
        if (t->getTaskType() == Task::xfunction)
            out << " shape=rect, style=filled, fillcolor=yellow";
        if (t->getTaskType() == Task::xcondition)
            out << " shape=invhouse, style=filled, fillcolor=yellow";
        if (t->getTaskType() == Task::init_agent)
            out << " shape=rect, style=filled, fillcolor=red";
        if (t->getTaskType() == Task::xvariable)
            out << " shape=ellipse";
        if (t->getTaskType() == Task::xmessage)
            out << " shape=parallelogram, style=filled, fillcolor=lightblue";
        if (t->getTaskType() == Task::io_pop_write)
            out << " shape=folder, style=filled, fillcolor=orange";
        out << "]";
    }
  protected:
    std::vector<Task *> * vertex2task;
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
        if (d) out << "label=\"" << d->getGraphName() << "\" ";
        if (arrowType == edge_label_writer::arrowBackward) out << "dir = back";
        out << "]";
    }
  protected:
    EdgeMap * edge2dependency;
    ArrowType arrowType;
};

struct graph_writer {
    void operator()(std::ostream& out) const {
        // out << "node [shape = rect]" << std::endl;
    }
};

void XGraph::write_graphviz(std::string fileName) {
    std::fstream graphfile;
    graphfile.open(fileName.c_str(), std::fstream::out);

    boost::write_graphviz(graphfile, *graph_,
            vertex_label_writer(vertex2task_),
            edge_label_writer(edge2dependency_,
                edge_label_writer::arrowForward),
            graph_writer());

    graphfile.clear();
}

#ifdef TESTBUILD
void XGraph::testBoostGraphLibrary() {
    std::pair<Edge, bool> e1, e2, e3;
    std::map<Edge, int> edge2int;

    e1 = add_edge(0, 1, *graph_);
    std::cout << "Add edge " << e1.first << " " << e1.second << std::endl;
    edge2int.insert(std::make_pair(e1.first, 6));
    e2 = add_edge(0, 1, *graph_);
    std::cout << "Add edge " << e2.first << " " << e2.second << std::endl;
    edge2int.insert(std::make_pair(e2.first, 7));
    e3 = add_edge(0, 1, *graph_);
    std::cout << "Add edge " << e3.first << " " << e3.second << std::endl;
    edge2int.insert(std::make_pair(e3.first, 8));

    //printf("Edge %d\n", e.first);

    boost::graph_traits<Graph>::edge_iterator iei, iei_end;
    for (boost::tie(iei, iei_end) = boost::edges(*graph_); iei != iei_end; ++iei)
        std::cout << (*iei) << " " << edge2int.find((Edge)(*iei))->second << std::endl;

    std::cout << "Remove " << e2.first << " " << edge2int.find((Edge)e2.first)->second << std::endl;
    boost::remove_edge(e2.first, *graph_);

    for (boost::tie(iei, iei_end) = boost::edges(*graph_); iei != iei_end; ++iei)
        std::cout << (*iei) << " " << edge2int.find((Edge)(*iei))->second << std::endl;

    graph_->clear();
}
#endif

}}   // namespace flame::model
