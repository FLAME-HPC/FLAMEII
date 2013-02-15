/*!
 * \file tests/model/test_xgraph.cpp
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
#include "flame2/model/dependencygraph.hpp"
#include "flame2/model/model.hpp"
#include "flame2/io/io_manager.hpp"

namespace model = flame::model;
namespace io = flame::io;

BOOST_AUTO_TEST_SUITE(DependencyGraph)

BOOST_AUTO_TEST_CASE(test_raw_conflict) {
  model::DependencyGraph dgraph;

  // Add function tasks to graph
  model::Task * f0 = new model::Task("agent", "f0", model::Task::xfunction);
  f0->addReadOnlyVariable("a");
  model::Task * f1 = new model::Task("agent", "f1", model::Task::xfunction);
  f1->addReadOnlyVariable("a");
  model::Task * f2 = new model::Task("agent", "f2", model::Task::xfunction);
  f2->addReadWriteVariable("a");
  model::Task * f3 = new model::Task("agent", "f3", model::Task::xfunction);
  f3->addReadOnlyVariable("a");
  model::Vertex v0 = dgraph.addTestVertex(f0);
  model::Vertex v1 = dgraph.addTestVertex(f1);
  model::Vertex v2 = dgraph.addTestVertex(f2);
  model::Vertex v3 = dgraph.addTestVertex(f3);
  // Add dependencies between tasks
  dgraph.addTestEdge(v0, v1, "", model::Dependency::state);
  dgraph.addTestEdge(v1, v2, "", model::Dependency::state);
  dgraph.addTestEdge(v2, v3, "", model::Dependency::state);
  // Set up graph for processing
  boost::ptr_vector<model::XVariable> variables;
  variables.push_back(new model::XVariable("a"));
  dgraph.setTestStartTask(f0);
  dgraph.addTestEndTask(f3);
  dgraph.setName("test_xgraph");
  // Process graph
  dgraph.generateDependencyGraph(&variables);
  // Test outcome
  BOOST_CHECK(dgraph.dependencyExists("f1", "f2") == true);
  BOOST_CHECK(dgraph.dependencyExists("f0", "f2") == true);
}

BOOST_AUTO_TEST_CASE(test_dependencygraph) {
  flame::io::IOManager& m = flame::io::IOManager::GetInstance();
  flame::model::XModel model;
  flame::model::DependencyGraph * graph;

  BOOST_CHECK_NO_THROW(m.loadModel(
      "model/models/infection.xml", &model));
  BOOST_CHECK(model.validate() == 0);
  graph = model.getGraph();

  BOOST_CHECK(graph->dependencyExists("output_location", "0") == true);
  BOOST_CHECK(graph->dependencyExists("0", "transit_disease") == true);
  BOOST_CHECK(graph->dependencyExists("0", "update_resistance") == true);
  BOOST_CHECK(graph->dependencyExists(
      "0", "calculate_connection_forces") == true);
  BOOST_CHECK(graph->dependencyExists("0", "calculate_crowd_forces") == true);
  BOOST_CHECK(graph->dependencyExists(
      "transit_disease", "update_connections") == true);
  BOOST_CHECK(graph->dependencyExists(
      "calculate_connection_forces", "update_connections") == true);
  BOOST_CHECK(graph->dependencyExists("update_connections", "move") == true);
  BOOST_CHECK(graph->dependencyExists(
      "calculate_crowd_forces", "move") == true);
  BOOST_CHECK(graph->dependencyExists(
      "transit_disease", "diagnosis_and_recovery") == true);
  BOOST_CHECK(graph->dependencyExists(
      "update_infection_status", "diagnosis") == true);
}

BOOST_AUTO_TEST_SUITE_END()
