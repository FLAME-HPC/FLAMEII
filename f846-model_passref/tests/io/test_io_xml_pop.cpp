/*!
 * \file tests/io/test_io_xml_pop.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Test suite for the io xml pop method
 */
#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE IO Pop
#endif
#include <boost/test/unit_test.hpp>
#include <vector>
#include <string>
#include "flame2/io/io_manager.hpp"
#include "flame2/io/io_xml_model.hpp"
#include "flame2/io/io_xml_pop.hpp"
#include "flame2/mem/memory_manager.hpp"
#include "flame2/sim/simulation.hpp"

namespace xml = flame::io::xml;
namespace model = flame::model;
namespace e = flame::exceptions;

BOOST_AUTO_TEST_SUITE(IOPop)

BOOST_AUTO_TEST_CASE(test_read_same_dir) {
  xml::IOXMLModel ioxmlmodel;
  model::XModel model;
  flame::io::IOManager& iomanager = flame::io::IOManager::GetInstance();

  /* Read model xml */
  ioxmlmodel.readXMLModel("io/models/all_data.xml", &model);

  // Create 0.xml in program dir
  FILE *file;
  file = fopen("0.xml", "w");
  if (file == NULL) {
    BOOST_FAIL("Error: Could not create 0.xml for test");
  } else {
    fprintf(file, "<states><itno>0</itno></states>");
    fclose(file);

    BOOST_CHECK_NO_THROW(
        iomanager.readPop("0.xml", &model, flame::io::IOManager::xml));

    BOOST_CHECK_NO_THROW(
        iomanager.readPop("./0.xml", &model, flame::io::IOManager::xml));

    if (remove("0.xml") != 0)
      fprintf(stderr,
          "Warning: Could not delete the generated file: 0.xml\n");
  }
}

/* Test creation of data schema */
BOOST_AUTO_TEST_CASE(test_data_schema) {
  xml::IOXMLPop ioxmlpop;
  xml::IOXMLModel ioxmlmodel;
  model::XModel model;

  /* Read model xml */
  BOOST_CHECK_NO_THROW(
      ioxmlmodel.readXMLModel("io/models/all_data.xml", &model));

  /* Generate data schema */
  BOOST_CHECK_NO_THROW(
      ioxmlpop.createDataSchema("io/models/all_data.xsd", &model));

  /* Validate data using schema */
  std::string xsd = "io/models/all_data.xsd";
  BOOST_CHECK_NO_THROW(
      ioxmlpop.validateData("io/models/all_data_its/0.xml", xsd));
  /* Remove created all_data.xsd */
  if (remove(xsd.c_str()) != 0)
    fprintf(stderr, "Warning: Could not delete the generated file: %s\n",
        xsd.c_str());
}
/*
// Test the reading of XML population files
BOOST_AUTO_TEST_CASE(test_read_XML_pop) {
  unsigned int ii;
  xml::IOXMLPop ioxmlpop;
  xml::IOXMLModel ioxmlmodel;
  model::XModel model;
  flame::mem::MemoryManager& memoryManager =
      flame::mem::MemoryManager::GetInstance();

  // read model xml
  ioxmlmodel.readXMLModel("io/models/all_data.xml", &model);
  // register agents with memory manager
  flame::sim::Simulation sim;
  sim.registerModelWithMemoryManagerTest(model);

  BOOST_CHECK_THROW(ioxmlpop.readPop(
      "io/models/all_data_its/0_missing.xml", &model),
      e::inaccessable_file);

  BOOST_CHECK_THROW(ioxmlpop.readPop(
      "io/models/all_data_its/0_malformed.xml", &model),
      e::unparseable_file);

  BOOST_CHECK_THROW(ioxmlpop.readPop(
      "io/models/all_data_its/0_unknown_tag.xml", &model),
      e::unparseable_file);

  BOOST_CHECK_THROW(ioxmlpop.readPop(
      "io/models/all_data_its/0_unknown_agent.xml", &model),
      e::invalid_pop_file);

  BOOST_CHECK_THROW(ioxmlpop.readPop(
      "io/models/all_data_its/0_unknown_variable.xml", &model),
      e::invalid_pop_file);

  BOOST_CHECK_THROW(ioxmlpop.readPop(
      "io/models/all_data_its/0_var_not_int.xml", &model),
      e::invalid_pop_file);

  BOOST_CHECK_THROW(ioxmlpop.readPop(
      "io/models/all_data_its/0_var_not_double.xml", &model),
      e::invalid_pop_file);

  std::string zeroxml = "io/models/all_data_its/0.xml";
  BOOST_CHECK_NO_THROW(ioxmlpop.readPop(zeroxml, &model));

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
  for (ii = 0; ii < rod->size(); ii++) {
    BOOST_CHECK_CLOSE(*(rod->begin()+ii), *(expectedd+ii), 0.0001);
  }

  // Test pop data written out
  std::string onexml = "io/models/all_data_its/1.xml";
  ioxmlpop.setIteration(1);
  ioxmlpop.setXmlPopPath(zeroxml);
  ioxmlpop.finaliseData();
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
}*/

BOOST_AUTO_TEST_SUITE_END()
