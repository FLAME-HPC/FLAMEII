/*!
 * \file tests/model/test_dependencygraph.cpp
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

namespace m = flame::model;
namespace io = flame::io;

BOOST_AUTO_TEST_SUITE(DependencyGraph)

BOOST_AUTO_TEST_CASE(test_raw_conflict) {
  m::DependencyGraph dgraph;

  // Add function tasks to graph
  m::ModelTask * f0 = new m::ModelTask("agent", "f0", m::ModelTask::xfunction);
  f0->addReadOnlyVariable("a");
  m::ModelTask * f1 = new m::ModelTask("agent", "f1", m::ModelTask::xfunction);
  f1->addReadOnlyVariable("a");
  m::ModelTask * f2 = new m::ModelTask("agent", "f2", m::ModelTask::xfunction);
  f2->addReadWriteVariable("a");
  m::ModelTask * f3 = new m::ModelTask("agent", "f3", m::ModelTask::xfunction);
  f3->addReadOnlyVariable("a");
  m::Vertex v0 = dgraph.addTestVertex(f0);
  m::Vertex v1 = dgraph.addTestVertex(f1);
  m::Vertex v2 = dgraph.addTestVertex(f2);
  m::Vertex v3 = dgraph.addTestVertex(f3);
  // Add dependencies between tasks
  dgraph.addTestEdge(v0, v1, "", m::Dependency::state);
  dgraph.addTestEdge(v1, v2, "", m::Dependency::state);
  dgraph.addTestEdge(v2, v3, "", m::Dependency::state);
  // Set up graph for processing
  boost::ptr_vector<m::XVariable> variables;
  variables.push_back(new m::XVariable("a"));
  dgraph.setTestStartTask(f0);
  dgraph.addTestEndTask(f3);
  dgraph.setName("test_xgraph");
  // Process graph
  dgraph.generateDependencyGraph(&variables);
  // Test outcome
  BOOST_CHECK(dgraph.dependencyExists(m::ModelTask::xfunction, "f1",
      m::ModelTask::xfunction, "f2") == true);
  BOOST_CHECK(dgraph.dependencyExists(m::ModelTask::xfunction, "f0",
      m::ModelTask::xfunction, "f2") == true);
}

BOOST_AUTO_TEST_CASE(test_dependencygraph) {
  io::IOManager& m = io::IOManager::GetInstance();
  m::XModel model;
  m::DependencyGraph * graph;

  BOOST_CHECK_NO_THROW(m.loadModel(
      "model/models/infection.xml", &model));
  BOOST_CHECK(model.validate() == 0);
  graph = model.getGraph();

  // check agent function and condition dependencies
  BOOST_CHECK(graph->dependencyExists(
      m::ModelTask::xfunction, "output_location",
      m::ModelTask::xcondition, "0") == true);
  BOOST_CHECK(graph->dependencyExists(
      m::ModelTask::xcondition, "0",
      m::ModelTask::xfunction, "transit_disease") == true);
  BOOST_CHECK(graph->dependencyExists(
      m::ModelTask::xcondition, "0",
      m::ModelTask::xfunction, "update_resistance") == true);
  BOOST_CHECK(graph->dependencyExists(
      m::ModelTask::xcondition, "0",
      m::ModelTask::xfunction, "calculate_connection_forces") == true);
  BOOST_CHECK(graph->dependencyExists(
      m::ModelTask::xcondition, "0",
      m::ModelTask::xfunction, "calculate_crowd_forces") == true);
  BOOST_CHECK(graph->dependencyExists(
      m::ModelTask::xfunction, "transit_disease",
      m::ModelTask::xfunction, "update_connections") == true);
  BOOST_CHECK(graph->dependencyExists(
      m::ModelTask::xfunction, "calculate_connection_forces",
      m::ModelTask::xfunction, "update_connections") == true);
  BOOST_CHECK(graph->dependencyExists(
      m::ModelTask::xfunction, "update_connections",
      m::ModelTask::xfunction, "move") == true);
  BOOST_CHECK(graph->dependencyExists(
      m::ModelTask::xfunction, "calculate_crowd_forces",
      m::ModelTask::xfunction, "move") == true);
  BOOST_CHECK(graph->dependencyExists(
      m::ModelTask::xfunction, "transit_disease",
      m::ModelTask::xfunction, "diagnosis_and_recovery") == true);
  BOOST_CHECK(graph->dependencyExists(
      m::ModelTask::xfunction, "update_infection_status",
      m::ModelTask::xfunction, "diagnosis") == true);
  // check message dependencies
  BOOST_CHECK(graph->dependencyExists(
      m::ModelTask::xfunction, "output_location",
      m::ModelTask::xmessage_sync, "location") == true);
  BOOST_CHECK(graph->dependencyExists(
      m::ModelTask::xmessage_sync, "location",
      m::ModelTask::xfunction, "calculate_connection_forces") == true);
  BOOST_CHECK(graph->dependencyExists(
      m::ModelTask::xmessage_sync, "location",
      m::ModelTask::xfunction, "calculate_crowd_forces") == true);
  BOOST_CHECK(graph->dependencyExists(
      m::ModelTask::xfunction, "calculate_connection_forces",
      m::ModelTask::xmessage_clear, "location") == true);
  BOOST_CHECK(graph->dependencyExists(
      m::ModelTask::xfunction, "calculate_crowd_forces",
      m::ModelTask::xmessage_clear, "location") == true);
  // check io dependencies
  BOOST_CHECK(graph->dependencyExists(
      m::ModelTask::xcondition, "0",
      m::ModelTask::io_pop_write, "id") == true);
  BOOST_CHECK(graph->dependencyExists(
      m::ModelTask::xfunction, "calculate_connection_forces",
      m::ModelTask::io_pop_write, "fx_connections") == true);
  BOOST_CHECK(graph->dependencyExists(
      m::ModelTask::xfunction, "calculate_connection_forces",
      m::ModelTask::io_pop_write, "fy_connections") == true);
  BOOST_CHECK(graph->dependencyExists(
      m::ModelTask::xfunction, "calculate_crowd_forces",
      m::ModelTask::io_pop_write, "fx_crowd") == true);
  BOOST_CHECK(graph->dependencyExists(
      m::ModelTask::xfunction, "calculate_crowd_forces",
      m::ModelTask::io_pop_write, "fy_crowd") == true);
  BOOST_CHECK(graph->dependencyExists(
      m::ModelTask::xfunction, "move",
      m::ModelTask::io_pop_write, "x") == true);
  BOOST_CHECK(graph->dependencyExists(
      m::ModelTask::xfunction, "move",
      m::ModelTask::io_pop_write, "y") == true);
  BOOST_CHECK(graph->dependencyExists(
      m::ModelTask::xfunction, "diagnosis_and_recovery",
      m::ModelTask::io_pop_write, "diagnosed") == true);
  BOOST_CHECK(graph->dependencyExists(
      m::ModelTask::xfunction, "diagnosis_and_recovery",
      m::ModelTask::io_pop_write, "infected") == true);
  BOOST_CHECK(graph->dependencyExists(
      m::ModelTask::xfunction, "diagnosis_and_recovery",
      m::ModelTask::io_pop_write, "resistant") == true);
  BOOST_CHECK(graph->dependencyExists(
      m::ModelTask::xfunction, "update_connections",
      m::ModelTask::io_pop_write, "connections") == true);
  BOOST_CHECK(graph->dependencyExists(
      m::ModelTask::xfunction, "diagnosis",
      m::ModelTask::io_pop_write, "diagnosed") == true);
  BOOST_CHECK(graph->dependencyExists(
      m::ModelTask::xfunction, "update_infection_status",
      m::ModelTask::io_pop_write, "infected") == true);
}

BOOST_AUTO_TEST_SUITE_END()
