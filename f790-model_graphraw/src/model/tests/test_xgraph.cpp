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
#include "model/xgraph.hpp"
#include "model/model.hpp"
#include "io/io_manager.hpp"

namespace model = flame::model;
namespace io = flame::io;

BOOST_AUTO_TEST_SUITE(XGraph)

/*BOOST_AUTO_TEST_CASE(TestCompareTaskSets) {
    model::XGraph xgraph;

    bool r = xgraph.testCompareTaskSets();
    BOOST_CHECK(r == true);
}*/


BOOST_AUTO_TEST_CASE(test_raw_conflict) {
    model::XGraph graph;

    // Add function tasks to graph
    model::Task f0("agent", "f0", model::Task::xfunction);
    f0.addReadOnlyVariable("a");
    model::Task f1("agent", "f1", model::Task::xfunction);
    f1.addReadOnlyVariable("a");
    model::Task f2("agent", "f2", model::Task::xfunction);
    f2.addReadWriteVariable("a");
    model::Task f3("agent", "f3", model::Task::xfunction);
    f3.addReadOnlyVariable("a");
    model::Vertex v0 = graph.addTestVertex(&f0);
    model::Vertex v1 = graph.addTestVertex(&f1);
    model::Vertex v2 = graph.addTestVertex(&f2);
    model::Vertex v3 = graph.addTestVertex(&f3);
    // Add dependencies between tasks
    graph.addTestEdge(v0, v1, "", model::Dependency::state);
    graph.addTestEdge(v1, v2, "", model::Dependency::state);
    graph.addTestEdge(v2, v3, "", model::Dependency::state);
    // Set up graph for processing
    std::vector<model::XVariable*> variables;
    model::XVariable a("a");
    variables.push_back(&a);
    graph.setTestStartTask(&f0);
    graph.addTestEndTask(&f3);
    graph.setTasksImported(true);
    graph.setAgentName("test_xgraph");
    // Process graph
    graph.generateDependencyGraph(&variables);
    // Test outcome
    BOOST_CHECK(graph.dependencyExists("f1", "f2") == true);
    BOOST_CHECK(graph.dependencyExists("f0", "f2") == true);
}
/*
BOOST_AUTO_TEST_CASE(test_raw_conflict) {
    flame::io::IOManager& m = flame::io::IOManager::GetInstance();
    flame::model::XModel model;
    flame::model::XGraph graph;
    int rc;

    BOOST_CHECK_NO_THROW(m.loadModel(
            "src/model/tests/models/test_raw_conflict.xml", &model));
    rc = model.validate();
    BOOST_CHECK(rc == 0);
    model.generateGraph(&graph);

    BOOST_CHECK(graph.dependencyExists("f1", "f2") == true);
    BOOST_CHECK(graph.dependencyExists("f0", "f2") == true);
}

BOOST_AUTO_TEST_CASE(test_add_variable_vertices_1) {
    model::XGraph xgraph;

    // Create variable 'a'
    model::XVariable v("a");
    // Create variables list and add 'a'
    std::vector<model::XVariable*> variables;
    variables.push_back(&v);
    // Create functions, add 'a' to RW variables, and add to graph
    model::XFunction finit("Init");
    finit.getReadWriteVariables()->push_back(&v);
    xgraph.add_init_task_to_graph(&finit);
    model::XFunction f0("0");
    f0.getReadWriteVariables()->push_back(&v);
    xgraph.addTransitionFunctionToGraph(&f0);
    model::XFunction f1("1");
    f1.getReadWriteVariables()->push_back(&v);
    xgraph.addTransitionFunctionToGraph(&f1);
    model::XFunction f2("2");
    f2.getReadOnlyVariables()->push_back(&v);
    xgraph.addTransitionFunctionToGraph(&f2);
    // Add edges between functions to graph
    xgraph.addEdge(finit.getTask(), f0.getTask(), model::Dependency::init);
    xgraph.addEdge(f0.getTask(), f1.getTask(), model::Dependency::state);
    xgraph.addEdge(f1.getTask(), f2.getTask(), model::Dependency::state);
    xgraph.addEdge(f0.getTask(), f2.getTask(), model::Dependency::state);

    //xgraph.write_graphviz("test1.dot");
    // Init graph conditions
    xgraph.add_condition_vertices_to_graph();
    // Init variable edges
    xgraph.add_variable_vertices_to_graph(&variables);
    //xgraph.write_graphviz("test2.dot");
}

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
*/

BOOST_AUTO_TEST_SUITE_END()
