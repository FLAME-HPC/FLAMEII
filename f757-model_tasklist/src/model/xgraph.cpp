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
#include <string>
#include <vector>
#include <utility>
#include "./xgraph.hpp"

namespace flame { namespace model {

XGraph::~XGraph() {
    /* Free tasks and dependencies memory */
}

void XGraph::addVertex(Task * t) {
    vertex_descriptor v = add_vertex(graph_);
    vertex2task_.insert(std::make_pair(v, t));
}

void XGraph::addEdge(Task * to, Task * from, Dependency * d) {
    std::pair<edge_descriptor, bool> e =
        add_edge(getVertex(to), getVertex(from), graph_);
    edge2dependency_.insert(std::make_pair(e.first, d));
}

vertex_descriptor XGraph::getVertex(Task * t) {
    VertexMap::iterator it;
    for (it = vertex2task_.begin(); it != vertex2task_.end(); ++it) {
        if ((*it).second == t) return (*it).first;
    }
    return 0;
}

struct has_cycle { };

// Visitor function object passed to depth_first_search
// Contains back_edge method that is called when the depth_first_search
// explores an edge to an already discovered vertex
struct cycle_detector : public boost::default_dfs_visitor {
    cycle_detector(edge_descriptor& cycle_edge) : cycle_edge_(cycle_edge) {}
    void back_edge(edge_descriptor edge_t, const Graph &) {
        cycle_edge_ = edge_t;
        throw has_cycle();
    }
  protected:
    edge_descriptor& cycle_edge_;
};

int XGraph::check_dependency_loops() {
    // Possible cyclic edge
    edge_descriptor edge;
    // Visitor cycle detector for use with depth_first_search
    cycle_detector vis(edge);

    try {
        // Depth first search applied to graph
        depth_first_search(graph_, visitor(vis));
    } catch (has_cycle) {
        // If has_cycle is caught
        Dependency * d = edge2dependency_.find(edge)->second;
        std::cout << "Error: cycle detected " << d->getName() << std::endl;
        return 1;
    }

    return 0;
}

void XGraph::write_graphviz() {
    boost::write_graphviz(std::cout, graph_);
}

void XGraph::write_dependency_graph_dependencies(FILE *file) {
    std::pair<boost::graph_traits<Graph>::edge_iterator,
              boost::graph_traits<Graph>::edge_iterator> ep;
    vertex_descriptor u, v;
    // For each edge in the graph
    for (ep = edges(graph_); ep.first != ep.second; ++ep.first) {
        // Get the two vertices that are joined by this edge
        u = boost::source(*ep.first, graph_);
        v = boost::target(*ep.first, graph_);
        // Find the associated tasks of the verticies
        Task * t1 = vertex2task_.find(u)->second;
        Task * t2 = vertex2task_.find(v)->second;
        // Find the associated dependency of the edge
        Dependency * dependency = edge2dependency_.find(*ep.first)->second;
        // Write info to file
        fputs("\t", file);
        fputs(t1->getParentName().c_str(), file);
        fputs("_", file);
        fputs(t1->getName().c_str(), file);
        fputs(" -> ", file);
        fputs(t2->getParentName().c_str(), file);
        fputs("_", file);
        fputs(t2->getName().c_str(), file);
        fputs(" [ label = \"<", file);
        // Check dependency type and output
        // appropriate description
        if (dependency->getDependencyType() == Dependency::communication)
            fputs("Message: ", file);
        else if (dependency->getDependencyType() == Dependency::data)
            fputs("Memory: ", file);
        else if (dependency->getDependencyType() == Dependency::state)
            fputs("State: ", file);
        fputs(dependency->getName().c_str(), file);
        fputs(">\" ];\n", file);
    }
}

void XGraph::write_dependency_graph(std::string filename) {
    /* File to write to */
    FILE *file;

    /* print out the location of the source file */
    printf("Writing file: %s\n", filename.c_str());
    /* open the file to write to */
    file = fopen(filename.c_str(), "w");

    fputs("digraph dependency_graph {\n", file);
    fputs("\trankdir=BT;\n", file);
    fputs("\tsize=\"8,5;\"\n", file);
    fputs("\tnode [shape = rect];\n", file);

    fputs("\t\n\t/* Tasks */\n", file);
    /* For each task */
    std::pair<boost::graph_traits<Graph>::vertex_iterator,
              boost::graph_traits<Graph>::vertex_iterator> vi;
    for (vi = boost::vertices(graph_); vi.first != vi.second; ++vi.first) {
        Task * task = vertex2task_.find(*vi.first)->second;
        fputs("\t", file);
        fputs(task->getParentName().c_str(), file);
        fputs("_", file);
        fputs(task->getName().c_str(), file);
        fputs("[label = \"", file);
        fputs(task->getParentName().c_str(), file);
        fputs("\\n", file);
        fputs(task->getName().c_str(), file);
        fputs("\"]\n", file);
    }
    // For each dependency
    write_dependency_graph_dependencies(file);

    fputs("}", file);

    /* Close the file */
    fclose(file);
}

}}   // namespace flame::model
