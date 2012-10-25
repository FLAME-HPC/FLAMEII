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

BOOST_AUTO_TEST_CASE(test_xgraph) {
    flame::io::IOManager& m = flame::io::IOManager::GetInstance();
    flame::model::XModel model;
    flame::model::XGraph graph;

    BOOST_CHECK_NO_THROW(m.loadModel(
            "model/models/infection.xml", &model));
    BOOST_CHECK(model.validate() == 0);
    model.generateGraph(&graph);

    BOOST_CHECK(graph.dependencyExists("output_location", "0") == true);
    BOOST_CHECK(graph.dependencyExists("0", "transit_disease") == true);
    BOOST_CHECK(graph.dependencyExists("0", "update_resistance") == true);
    BOOST_CHECK(graph.dependencyExists(
            "0", "calculate_connection_forces") == true);
    BOOST_CHECK(graph.dependencyExists("0", "calculate_crowd_forces") == true);
    BOOST_CHECK(graph.dependencyExists(
            "transit_disease", "update_connections") == true);
    BOOST_CHECK(graph.dependencyExists(
            "calculate_connection_forces", "update_connections") == true);
    BOOST_CHECK(graph.dependencyExists("update_connections", "move") == true);
    BOOST_CHECK(graph.dependencyExists(
            "calculate_crowd_forces", "move") == true);
    BOOST_CHECK(graph.dependencyExists(
            "transit_disease", "diagnosis_and_recovery") == true);
    BOOST_CHECK(graph.dependencyExists(
            "update_infection_status", "diagnosis") == true);
}

BOOST_AUTO_TEST_SUITE_END()
