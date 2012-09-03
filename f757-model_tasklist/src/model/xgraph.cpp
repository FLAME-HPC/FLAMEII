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

namespace flame { namespace model {

XGraph::~XGraph() {
    /* Free tasks and dependencies memory */
}

Vertex XGraph::addVertex(Task * t) {
    Vertex v = add_vertex(graph_);
    vertex2task_.insert(std::make_pair(v, t));

    std::cout << v << t->getFullName() << std::endl;

    return v;
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

void XGraph::write_graphviz() {
    std::fstream graphfile;
    graphfile.open("graph.dot", std::fstream::out);

    boost::write_graphviz(graphfile, graph_,
            vertex_label_writer(&vertex2task_),
            edge_label_writer(&edge2dependency_, edge_label_writer::arrowBackward),
            graph_writer());

    graphfile.clear();
}

}}   // namespace flame::model
