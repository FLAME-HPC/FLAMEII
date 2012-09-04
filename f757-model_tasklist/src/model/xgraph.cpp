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
#include <utility>
#include <exception>
#include <fstream>
#include "./xgraph.hpp"
#include "./xcondition.hpp"
#include "./xfunction.hpp"

namespace flame { namespace model {

XGraph::~XGraph() {
    /* Free tasks and dependencies memory */

    /* Free condition functions */
}

Vertex XGraph::addVertex(Task * t) {
    Vertex v = add_vertex(graph_);
    vertex2task_.insert(std::make_pair(v, t));

    std::cout << v << t->getFullName() << std::endl;

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
    // Vertex discovered when traversing out edges
    void discover_vertex(Vertex u, const Graph & g) const {
        std::cout << "Discover: " << u << std::endl;
    }
    // Vertex finished when all of its out edge vertices traversed
    void finish_vertex(Vertex u, const Graph & g) const {
        std::cout << "Finish:   " << u << std::endl;
    }
};

void XGraph::test_layers() {
    func_layer vis;

    boost::breadth_first_search(graph_, startVertex_, boost::visitor(vis));
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

void XGraph::add_branch_vertices_to_graph() {
    typedef boost::graph_traits<Graph>::vertex_iterator vertex_iter;
    std::pair<vertex_iter, vertex_iter> vp;
    std::set<Edge> edgesToRemove;

    write_graphviz("before.dot");

    // For each vertex
    for (vp = boost::vertices(graph_); vp.first != vp.second; ++vp.first) {
        std::cout << *vp.first << " " << boost::out_degree(*vp.first, graph_) << std::endl;
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
            Dependency * d = new Dependency;
            d->setParentName(getTask(*vp.first)->getParentName());
            d->setName("Condition");
            d->setDependencyType(Dependency::condition);
            Edge e = addEdge((*vp.first), conditionVertex, d);
            std::cout << "test " << conditionVertex << " -> " << (*vp.first) << " " << e <<std::endl;
            // Make out edges source be from this new vertex
            boost::graph_traits<Graph>::out_edge_iterator oei, oei_end;
            // For each out edge
            for (boost::tie(oei, oei_end) = boost::out_edges(*(vp.first), graph_);
                oei != oei_end; ++oei) {
                // If a state edge, not a condition edge
                if (getDependency(*oei)->getDependencyType() == Dependency::state) {
                    std::cout << boost::source(*oei, graph_) << " -> " <<
                        boost::target(*oei, graph_) << std::endl;
                    Task * t = getTask(boost::target(*oei, graph_));
                    //XCondition * condition = t->getFunction()->getCondition();
                    //condition->print();
                    Dependency * d = new Dependency;
                    d->setParentName(getTask(*vp.first)->getParentName());
                    d->setName("Condition");
                    d->setDependencyType(Dependency::condition);
                    addEdge(conditionTask, t, d);
                    edgesToRemove.insert(*oei);
                }
            }
        }
    }

    // Remove unwanted edges
    std::set<Edge>::iterator it;
    for (it = edgesToRemove.begin(); it != edgesToRemove.end(); it++)
        boost::remove_edge(*it, graph_);

    write_graphviz("after.dot");
    boost::write_graphviz(std::cout, graph_);
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
    has_cycle(Edge d) : d_(d) {}
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
    } catch (has_cycle& err) {
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
        std::cout << *vp.first << " " << boost::out_degree(*vp.first, graph_) << std::endl;
        // If out edges is larger than 1
        if (boost::out_degree(*vp.first, graph_) > 1) {
            boost::graph_traits<Graph>::out_edge_iterator oei, oei_end;
            // For each out edge
            for (boost::tie(oei, oei_end) = boost::out_edges(*vp.first, graph_);
                oei != oei_end; ++oei) {
                    std::cout << boost::source(*oei, graph_) << " -> " <<
                        boost::target(*oei, graph_) << std::endl;
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
    vertex_label_writer(VertexMap * vm) : vertex2task(vm) {}
    void operator()(std::ostream& out, const Vertex& v) const {
        Task * t = vertex2task->find(v)->second;
        out << " [label=\"" << t->getFullName() << "\"]";
    }
  protected:
    VertexMap * vertex2task;
};

struct edge_label_writer {
    enum ArrowType { arrowForward = 0, arrowBackward };
    edge_label_writer(EdgeMap * em, ArrowType at) : edge2dependency(em), arrowType(at) {}
    void operator()(std::ostream& out, const Edge& e) const {
        Dependency * d = edge2dependency->find(e)->second;
        out << " [label=\"" << d->getGraphName() << "\"";
        if (arrowType == edge_label_writer::arrowBackward) out << " dir = back";
        out << "]";
    }
  protected:
    EdgeMap * edge2dependency;
    ArrowType arrowType;
};

struct graph_writer {
    void operator()(std::ostream& out) const {
        out << "node [shape = rect]" << std::endl;
    }
};

void XGraph::write_graphviz(std::string fileName) {
    std::fstream graphfile;
    graphfile.open(fileName.c_str(), std::fstream::out);

    boost::write_graphviz(graphfile, graph_,
            vertex_label_writer(&vertex2task_),
            edge_label_writer(&edge2dependency_, edge_label_writer::arrowForward),
            graph_writer());

    graphfile.clear();
}

}}   // namespace flame::model
