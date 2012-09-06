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
#include <string>
#include <vector>
#include <set>
#include <utility>
#include <exception>
#include "./xgraph.hpp"
#include "./xcondition.hpp"
#include "./xfunction.hpp"
#include "./task.hpp"

namespace flame { namespace model {

XGraph::~XGraph() {
    /* Free tasks and dependencies memory */

    /* Free condition functions */
}

void XGraph::add_function_task_to_graph(XFunction * function) {
    // Add function as a task to the task list
    Task * task = new Task;
    // task->setParentName(getName());
    task->setName(function->getName());
    task->setTaskType(Task::xfunction);
    task->setPriorityLevel(5);
    task->setFunction(function);
    addVertex(task);
    // Associate task with function
    function->setTask(task);
}

Vertex XGraph::addVertex(Task * t) {
    Vertex v = add_vertex(graph_);
    vertex2task_.insert(std::make_pair(v, t));
    return v;
}

Edge XGraph::addEdge(Vertex to, Vertex from, Dependency * d) {
    std::pair<Edge, bool> e =
        add_edge(to, from, graph_);
    edge2dependency_.insert(std::make_pair(e.first, d));
    return e.first;
}

void XGraph::addEdge(Task * to, Task * from, Dependency * d) {
    std::pair<Edge, bool> e =
        add_edge(getVertex(to), getVertex(from), graph_);
    edge2dependency_.insert(std::make_pair(e.first, d));
}

Vertex XGraph::getVertex(Task * t) {
    VertexMap::iterator it;
    for (it = vertex2task_.begin(); it != vertex2task_.end(); ++it) {
        if ((*it).second == t) return (*it).first;
    }
    return 0;
}

Task * XGraph::getTask(Vertex v) {
    return vertex2task_.find(v)->second;
}

Dependency * XGraph::getDependency(Edge e) {
    return edge2dependency_.find(e)->second;
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

bool containsVariableName(std::vector<XVariable*> * variables, XVariable * variable) {
    std::vector<XVariable*>::iterator it;
    for (it = variables->begin(); it != variables->end(); it++)
        if ((*it)->getName() == variable->getName()) return true;
    return false;
}

void XGraph::discover_last_variable_writes(XVariable * variable,
        Vertex vertex,
        std::set<Vertex> * finished,
        std::set<Vertex> * writing) {
    boost::graph_traits<Graph>::in_edge_iterator iei, iei_end;

    // If vertex has already been seen then skip
    if(finished->find(vertex) == finished->end()) {
        for (boost::tie(iei, iei_end) = boost::in_edges(vertex, graph_);
                            iei != iei_end; ++iei) {
            // Only if edge dependency is state or condition
            Dependency * d = getDependency(*iei);
            if (d->getDependencyType() == Dependency::state ||
                    d->getDependencyType() == Dependency::condition)
            {
                // Get source vertex of the in edge
                Vertex v = boost::source(*iei, graph_);


                if (containsVariableName(
                        getTask(v)->getFunction()->getReadWriteVariables(), variable))
                    // If vertex writes variable then add vertex to writing vertices
                    writing->insert(v);
                else
                    // If vertex does not write variable then try higher vertices
                    discover_last_variable_writes(variable, v, finished, writing);
            }
        }
        // Add vertex to finished list
        finished->insert(vertex);
    }
}

struct variableVertexVertex {
    std::string varName;
    Vertex functionVertex;
    Vertex variableVertex;
};

void XGraph::add_variable_verticies_to_graph(
        std::vector<XVariable*> * variables) {
    std::vector<Vertex> breadth_first_vertices;
    std::vector<Vertex>::iterator vit;
    std::set<Vertex>::iterator sit;
    std::vector<XVariable*>::iterator varit;
    std::map<std::string, Vertex> currentVariableVertex;
    std::pair<VertexIterator, VertexIterator> vp;

    std::set<Vertex> finished;
    std::set<Vertex> writing;

    func_layer vis(&breadth_first_vertices);
    boost::breadth_first_search(graph_, startVertex_, boost::visitor(vis));

    // Add vertex for each variable
    for (varit = variables->begin(); varit != variables->end(); varit++) {
        Task * conditionTask = new Task;
        conditionTask->setParentName(getTask(startVertex_)->getParentName());
        conditionTask->setName((*varit)->getName());
        conditionTask->setTaskType(Task::xvariable);
        Vertex variableVertex = addVertex(conditionTask);
        currentVariableVertex.insert(std::pair<std::string, Vertex>((*varit)->getName(), variableVertex));
        std::cout << "make_pair " << (*varit)->getName() << " " << variableVertex << std::endl;
    }

    // For each vertex in depth order
    for (vit = breadth_first_vertices.begin();
         vit != breadth_first_vertices.end(); vit++) {
        std::vector<XVariable*> * rov =
            getTask(*vit)->getFunction()->getReadOnlyVariables();
        std::vector<XVariable*> * rwv =
            getTask(*vit)->getFunction()->getReadWriteVariables();

        std::cout << "*** " << *vit;
        for (varit = rwv->begin(); varit != rwv->end(); varit++) {
            std::cout << " " << (*varit)->getName();

            // Add out going edge to new variable vertex
            Task * varTask = new Task;
            //varTask->setParentName();
            varTask->setName((*varit)->getName());
            varTask->setTaskType(Task::xvariable);
            Vertex variableVertex = addVertex(varTask);
            Dependency * d = new Dependency;
            //d->setParentName("v");
            d->setName((*varit)->getName());
            d->setDependencyType(Dependency::variable);
            addEdge(getTask(*vit), varTask, d);
        }
        std::cout << std::endl;

        // Link reading variables to current variable vertex
        for (varit = rov->begin(); varit != rwv->end();) {
            if(varit != rov->end()) {
                finished.clear();
                writing.clear();

                discover_last_variable_writes(*varit, *vit, &finished, &writing);

                if (writing.size() == 0) {
                    // Add edge from vertex to original variable vertex
                    Vertex v = currentVariableVertex.find((*varit)->getName())->second;
                    Dependency * d = new Dependency;
                    d->setName((*varit)->getName());
                    d->setDependencyType(Dependency::variable);
                    addEdge(v, (*vit), d);
                } else {
                    // For each writing vector
                    for (sit = writing.begin(); sit != writing.end(); ++sit) {
                        // Find out edge for variable
                        boost::graph_traits<Graph>::out_edge_iterator oei, oei_end;
                        for (boost::tie(oei, oei_end) = boost::out_edges(*sit, graph_);
                                                oei != oei_end; ++oei) {
                            Vertex v2 = boost::target(*oei, graph_);
                            if (getTask(v2)->getTaskType() == Task::xvariable &&
                                    getTask(v2)->getName() == (*varit)->getName()) {
                                Dependency * d = new Dependency;
                                d->setName((*varit)->getName());
                                d->setDependencyType(Dependency::variable);
                                addEdge(v2, (*vit), d);
                            }
                        }
                    }
                }

                varit++;
            }
            if(varit == rov->end()) varit = rwv->begin();
        }

        for (varit = rwv->begin(); varit != rwv->end(); varit++)
            discover_last_variable_writes(*varit, *vit, &finished, &writing);


    }

    write_graphviz("after.dot");
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

void XGraph::add_condition_vertices_to_graph() {
    std::pair<VertexIterator, VertexIterator> vp;
    boost::graph_traits<Graph>::out_edge_iterator oei, oei_end;
    std::set<Edge> edgesToRemove;

    // For each vertex
    for (vp = boost::vertices(graph_); vp.first != vp.second; ++vp.first) {
        // If out edges is larger than 1 and an xfunction task
        if (boost::out_degree(*vp.first, graph_) > 1 &&
            getTask(*vp.first)->getTaskType() == Task::xfunction) {
            // Add condition vertex as function task
            XFunction * f = new XFunction;
            Task * conditionTask = new Task;
            conditionTask->setParentName(getTask(*vp.first)->getParentName());
            conditionTask->setName("Condition");
            conditionTask->setTaskType(Task::xcondition);
            conditionTask->setPriorityLevel(5);
            conditionTask->setFunction(f);
            Vertex conditionVertex = addVertex(conditionTask);
            // Add edge from condition task to vertex
            Dependency * d = new Dependency;
            d->setParentName(getTask(*vp.first)->getParentName());
            d->setName("Condition");
            d->setDependencyType(Dependency::condition);
            addEdge((*vp.first), conditionVertex, d);
            // Make out edges source be from the condition vertex
            // For each out edge
            for (boost::tie(oei, oei_end) =
                boost::out_edges(*vp.first, graph_);
                oei != oei_end; ++oei) {
                // If a state edge, not a condition edge
                if (getDependency((Edge)*oei)->getDependencyType() ==
                        Dependency::state) {
                    // Get the task of the next vertex
                    Vertex v = boost::target((Edge)*oei, graph_);
                    Task * t = getTask(v);
                    // Create a new dependency from next vertex task to the
                    // condition task
                    Dependency * d = new Dependency;
                    d->setParentName(getTask(*vp.first)->getParentName());
                    d->setName("Condition");
                    d->setDependencyType(Dependency::condition);
                    addEdge(conditionTask, t, d);
                    // Add old dependency between next vertex and original task
                    // to list to be removed
                    edgesToRemove.insert((Edge)*oei);
                    // Copy memory access info from condition to xfunction
                    std::set<XVariable*> * rov =
                            t->getFunction()->getCondition()
                                ->getReadOnlyVariables();
                    f->getReadOnlyVariables()->insert(
                            f->getReadOnlyVariables()->end(),
                            rov->begin(),
                            rov->end());
                }
            }
        }
    }

    // Remove unwanted edges
    std::set<Edge>::iterator it;
    for (it = edgesToRemove.begin(); it != edgesToRemove.end(); it++)
        boost::remove_edge(*it, graph_);
}

void XGraph::setStartVector(Vertex sv) {
    startVertex_ = sv;
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

int XGraph::check_dependency_loops() {
    // Visitor cycle detector for use with depth_first_search
    cycle_detector vis;

    try {
        // Depth first search applied to graph
        boost::depth_first_search(graph_, visitor(vis));
    // If cyclic dependency is caught
    } catch(const has_cycle& err) {
        // Find associated dependency
        Dependency * d = getDependency(err.edge());
        // Find associated tasks
        Task * t1 = getTask(boost::source(err.edge(), graph_));
        Task * t2 = getTask(boost::target(err.edge(), graph_));
        std::fprintf(stderr,
                    "Error: cycle detected %s -> %s -> %s\n",
                    t1->getFullName().c_str(),
                    d->getName().c_str(),
                    t2->getFullName().c_str());
        return 1;
    }

    return 0;
}

int XGraph::check_function_conditions() {
    typedef boost::graph_traits<Graph>::vertex_iterator vertex_iter;
    std::pair<vertex_iter, vertex_iter> vp;
    // For each vertex
    for (vp = boost::vertices(graph_); vp.first != vp.second; ++vp.first) {
        // If out edges is larger than 1
        if (boost::out_degree(*vp.first, graph_) > 1) {
            boost::graph_traits<Graph>::out_edge_iterator oei, oei_end;
            // For each out edge
            for (boost::tie(oei, oei_end) = boost::out_edges(*vp.first, graph_);
                oei != oei_end; ++oei) {
                    Task * t = getTask(boost::target(*oei, graph_));
                    XCondition * condition = t->getFunction()->getCondition();
                    // If condition is null then return an error
                    if (condition == 0) {
                        std::fprintf(stderr,
            "Error: Function '%s' from a state with more than one %s\n",
                        t->getFunction()->getName().c_str(),
                        "out going function does not have a condition.");
                        return 1;
                    }
                }
        }
    }

    return 0;
}

struct vertex_label_writer {
    explicit vertex_label_writer(VertexMap * vm) : vertex2task(vm) {}
    void operator()(std::ostream& out, const Vertex& v) const {
        Task * t = vertex2task->find(v)->second;
        out << " [label=\"" << t->getName() << "\"";
        if (t->getTaskType() == Task::xfunction)
            out << " shape=rect, style=filled, fillcolor=yellow";
        if (t->getTaskType() == Task::xcondition)
            out << " shape=invhouse, style=filled, fillcolor=yellow";
        if (t->getTaskType() == Task::xvariable)
            out << " shape=ellipse";
        out << "]";
    }
  protected:
    VertexMap * vertex2task;
};

struct edge_label_writer {
    enum ArrowType { arrowForward = 0, arrowBackward };
    edge_label_writer(EdgeMap * em, ArrowType at)
        : edge2dependency(em), arrowType(at) {}
    void operator()(std::ostream& out, const Edge& e) const {
        //Dependency * d = edge2dependency->find(e)->second;
        out << " [";
        //out << " [label=\"" << d->getGraphName() << "\"";
        if (arrowType == edge_label_writer::arrowBackward) out << " dir = back";
        out << "]";
    }
  protected:
    EdgeMap * edge2dependency;
    ArrowType arrowType;
};

struct graph_writer {
    void operator()(std::ostream& out) const {
        //out << "node [shape = rect]" << std::endl;
    }
};

void XGraph::write_graphviz(std::string fileName) {
    std::fstream graphfile;
    graphfile.open(fileName.c_str(), std::fstream::out);

    boost::write_graphviz(graphfile, graph_,
            vertex_label_writer(&vertex2task_),
            edge_label_writer(&edge2dependency_,
                edge_label_writer::arrowForward),
            graph_writer());

    graphfile.clear();
}

}}   // namespace flame::model
