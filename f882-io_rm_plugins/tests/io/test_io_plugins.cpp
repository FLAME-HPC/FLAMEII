/*!
 * \file tests/io/test_io_plugins.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Test suite for the io plugins
 */
#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE IO Pop
#endif
#include <boost/test/unit_test.hpp>
#include <vector>
#include <string>
#include <utility>
#include <map>
#include "flame2/io/io_manager.hpp"
#include "flame2/io/io_xml_model.hpp"
#include "flame2/mem/memory_manager.hpp"
#include "flame2/sim/simulation.hpp"
#include "flame2/exe/fifo_task_queue.hpp"
#include "flame2/exe/scheduler.hpp"
#include "flame2/api/flame2.hpp"

namespace io = flame::io;
namespace model = flame::model;
namespace mem = flame::mem;
namespace exe = flame::exe;
namespace e = flame::exceptions;

typedef std::pair<std::string, std::string> Var;
typedef std::map<std::string, std::vector<Var> > AgentMemory;

BOOST_AUTO_TEST_SUITE(IOPlugins)

BOOST_AUTO_TEST_CASE(test_read_same_dir) {
  flame::io::IOManager& iomanager = flame::io::IOManager::GetInstance();

  // Create 0.xml in program dir
  FILE *file;
  file = fopen("0.xml", "w");
  if (file == NULL) {
    BOOST_FAIL("Error: Could not create 0.xml for test");
  } else {
    fprintf(file, "<states><itno>0</itno></states>");
    fclose(file);

    BOOST_CHECK_NO_THROW(iomanager.readPop("0.xml"));

    BOOST_CHECK_NO_THROW(iomanager.readPop("./0.xml"));

    if (remove("0.xml") != 0)
      fprintf(stderr,
          "Warning: Could not delete the generated file: 0.xml\n");
  }

  iomanager.Reset();
}

//! Dummy addInt to test with
void addInt(std::string const&, std::string const&, int) {}

//! Dummy addDouble to test with
void addDouble(std::string const&, std::string const&, double) {}

// Test the reading of XML population files
BOOST_AUTO_TEST_CASE(test_read_XML_pop) {
  size_t ii;
  io::IO * ioxmlpop;
  io::xml::IOXMLModel ioxmlmodel;
  model::XModel model;
  mem::MemoryManager& memoryManager =
      mem::MemoryManager::GetInstance();
  flame::io::IOManager& iomanager = flame::io::IOManager::GetInstance();

  // read model xml
  ioxmlmodel.readXMLModel("io/models/all_data.xml", &model);
  // register agents with memory manager
  AgentMemory agentMemory = model.getAgentMemoryInfo();
  flame::sim::Simulation sim;
  sim.registerModelWithMemoryManagerTest(agentMemory);

  ioxmlpop = iomanager.getIOPlugin("xml");
  iomanager.setAgentMemoryInfo(model.getAgentMemoryInfo());

  BOOST_CHECK_THROW(ioxmlpop->readPop(
      "io/models/all_data_its/0_missing.xml", addInt, addDouble),
      std::exception);

  BOOST_CHECK_THROW(ioxmlpop->readPop(
      "io/models/all_data_its/0_malformed.xml", addInt, addDouble),
      std::exception);

  BOOST_CHECK_THROW(ioxmlpop->readPop(
      "io/models/all_data_its/0_unknown_tag.xml", addInt, addDouble),
      std::exception);

  BOOST_CHECK_THROW(ioxmlpop->readPop(
      "io/models/all_data_its/0_unknown_agent.xml", addInt, addDouble),
      std::exception);

  BOOST_CHECK_THROW(ioxmlpop->readPop(
      "io/models/all_data_its/0_unknown_variable.xml", addInt, addDouble),
      std::exception);

  BOOST_CHECK_THROW(ioxmlpop->readPop(
      "io/models/all_data_its/0_var_not_int.xml", addInt, addDouble),
      std::exception);

  BOOST_CHECK_THROW(ioxmlpop->readPop(
      "io/models/all_data_its/0_var_not_double.xml", addInt, addDouble),
      std::exception);

  std::string zeroxml = "io/models/all_data_its/0.xml";
  BOOST_CHECK_NO_THROW(iomanager.readPop(zeroxml));

  // Test pop data read in
  // Test ints data
  std::vector<int>* roi =
      memoryManager.GetVector<int>("agent_a", "int_single");
  int expectedi[] = {1, 2, 3};
  BOOST_CHECK_EQUAL_COLLECTIONS(expectedi, expectedi+3,
      roi->begin(), roi->end());
  // test doubles data
  std::vector<double>* rod =
      memoryManager.GetVector<double>("agent_a", "double_single");
  double expectedd[] = {0.1, 0.2, 0.3};
  for (ii = 0; ii < rod->size(); ii++)
    BOOST_CHECK_CLOSE(*(rod->begin()+ii), *(expectedd+ii), 0.0001);

  // Test pop data written out
  std::string onexml = "io/models/all_data_its/1.xml";
  iomanager.setIteration(1);
  iomanager.initialiseData();
  iomanager.writePop("agent_a", "int_single");
  iomanager.writePop("agent_a", "int_single");
  iomanager.writePop("agent_a", "double_single");
  iomanager.writePop("agent_b", "int_single");
  iomanager.writePop("agent_b", "double_single");
  iomanager.finaliseData();
  // Check 0.xml and 1.xml are identical
  size_t differences = 1;
  int c0, c1;
  FILE *zeroFile, *oneFile;
  zeroFile = fopen(zeroxml.c_str(), "r");
  oneFile  = fopen(onexml.c_str(), "r");
  if (zeroFile == 0) {
    fprintf(stderr, "Warning: Could not open the file: %s\n",
        zeroxml.c_str());
  } else if (oneFile == 0) {
    fprintf(stderr, "Warning: Could not open the file: %s\n",
        onexml.c_str());
  } else {
    differences = 0;
    c0 = fgetc(zeroFile);
    c1 = fgetc(oneFile);
    // While at least one file is not at the end
    while (c0 != EOF || c1 != EOF) {
      if (c0 != c1) differences++;
      if (c0 != EOF) c0 = fgetc(zeroFile);
      if (c1 != EOF) c1 = fgetc(oneFile);
    }
  }
  // Close files
  fclose(zeroFile);
  fclose(oneFile);
  BOOST_CHECK(differences == 0);

  // Remove created 1.xml
  if (remove(onexml.c_str()) != 0)
    fprintf(stderr, "Warning: Could not delete the generated file: %s\n",
        onexml.c_str());

  // Reset memory manager as to not affect next test suite
  memoryManager.Reset();
}

FLAME_AGENT_FUNCTION(idle) {
  FLAME.GetMem<int>("i");  // stop compiler warning
  return FLAME_AGENT_ALIVE;
}

void test_data(std::string const& agent_name,
    std::vector<int> i_save, std::vector<double> d_save) {
  size_t ii;
  mem::MemoryManager& memoryManager = mem::MemoryManager::GetInstance();

  std::vector<int>* a_i = memoryManager.GetVector<int>(agent_name, "i");
  BOOST_CHECK_EQUAL_COLLECTIONS(i_save.begin(), i_save.end(),
      a_i->begin(), a_i->end());
  std::vector<double>* a_d = memoryManager.GetVector<double>(agent_name, "d");
  // check array size is the same
  BOOST_CHECK(a_d->size() == d_save.size());
  for (ii = 0; ii < a_d->size(); ii++)
    BOOST_CHECK_CLOSE(*(a_d->begin()+ii), *(d_save.begin()+ii), 0.0001);
}

void test_plugin(std::string const& plugin_name) {
  mem::MemoryManager& memoryManager = mem::MemoryManager::GetInstance();
  flame::io::IOManager& ioManager = flame::io::IOManager::GetInstance();
  // load model and pop
  model::Model model("io/models/pluginmodel.xml");
  // validate model
  model.registerAgentFunction("idle", &idle);
  flame::sim::Simulation sim(model, "io/models/plugin_its/0.xml");
  // save pop locally so can be check later
  std::vector<int> a_i_save(*memoryManager.GetVector<int>("a", "i"));
  std::vector<double> a_d_save(*memoryManager.GetVector<double>("a", "d"));
  std::vector<int> b_i_save(*memoryManager.GetVector<int>("b", "i"));
  std::vector<double> b_d_save(*memoryManager.GetVector<double>("b", "d"));
  // update io manager to use test plugin
  ioManager.setOutputType(plugin_name);
  sim.start(1, 1);

  memoryManager.Reset();
  flame::exe::TaskManager::GetInstance().Reset();

  ioManager.setInputType(plugin_name);
  std::string pop("io/models/plugin_its/1.");
  pop.append(plugin_name);
  flame::sim::Simulation sim2(model, pop);

  test_data("a", a_i_save, a_d_save);
  test_data("b", b_i_save, b_d_save);

  // Remove created pop file
  if (remove(pop.c_str()) != 0)
    fprintf(stderr, "Warning: Could not delete the generated file: %s\n",
        pop.c_str());

  ioManager.Reset();
  memoryManager.Reset();
  flame::exe::TaskManager::GetInstance().Reset();
}

BOOST_AUTO_TEST_CASE(test_xml_plugin) {
  test_plugin("xml");
}

BOOST_AUTO_TEST_CASE(test_csv_plugin) {
  test_plugin("csv");
}

#ifdef HAVE_SQLITE
BOOST_AUTO_TEST_CASE(test_sqlite_plugin) {
  test_plugin("sqlite");
}
#endif

#ifdef HAVE_HDF5
BOOST_AUTO_TEST_CASE(test_hdf5_plugin) {
  test_plugin("hdf5");
}
#endif

BOOST_AUTO_TEST_SUITE_END()
