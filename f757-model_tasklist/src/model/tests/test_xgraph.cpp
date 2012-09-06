/*!
 * \file src/model/tests/test_xgraph.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Test suite for xgraph
 */
#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE XGraph
#endif
#include <boost/test/unit_test.hpp>
#include <vector>
#include <string>
#include "../xgraph.hpp"

namespace model = flame::model;

BOOST_AUTO_TEST_SUITE(XGraph)

BOOST_AUTO_TEST_CASE(test_graph_layers) {
    model::XGraph xgraph;

    model::Graph * graph = xgraph.getGraph();
    model::Vertex v0 = add_vertex(*graph);
    model::Vertex v1 = add_vertex(*graph);
    model::Vertex v2 = add_vertex(*graph);
    model::Vertex v3 = add_vertex(*graph);
    model::Vertex v4 = add_vertex(*graph);
    model::Vertex v5 = add_vertex(*graph);
    model::Vertex v6 = add_vertex(*graph);
    add_edge(v0, v2, *graph);  // 0->2
    add_edge(v2, v1, *graph);  // 2->1
    add_edge(v1, v3, *graph);  // 1->3

    add_edge(v0, v4, *graph);  // 0->4
    add_edge(v4, v6, *graph);  // 4->6
    add_edge(v6, v5, *graph);  // 6->5

    // xgraph.write_graphviz();

    // xgraph.test_layers();
}

BOOST_AUTO_TEST_SUITE_END()
