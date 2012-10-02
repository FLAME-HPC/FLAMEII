/*!
 * \file src/io/tests/test_io_xml_pop.cpp
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
#include "../io_xml_model.hpp"
#include "../io_xml_pop.hpp"
#include "../../mem/memory_manager.hpp"

namespace xml = flame::io::xml;
namespace model = flame::model;

BOOST_AUTO_TEST_SUITE(IOPop)

/* Test creation of data schema */
BOOST_AUTO_TEST_CASE(test_data_schema) {
    int rc;
    xml::IOXMLPop ioxmlpop;
    xml::IOXMLModel ioxmlmodel;
    model::XModel model;

    /* Read model xml */
    ioxmlmodel.readXMLModel("src/io/tests/models/all_data.xml", &model);

    /* Generate data schema */
    rc = ioxmlpop.createDataSchema("src/io/tests/models/all_data.xsd", &model);
    BOOST_CHECK(rc == 0);

    /* Validate data using schema */
    std::string xsd = "src/io/tests/models/all_data.xsd";
    rc = ioxmlpop.validateData("src/io/tests/models/all_data_its/0.xml", xsd);
    BOOST_CHECK(rc == 0);
    /* Remove created all_data.xsd */
    if (remove(xsd.c_str()) != 0)
    fprintf(stderr, "Warning: Could not delete the generated file: %s\n",
            xsd.c_str());
}

/* Test the reading of XML population files. */
BOOST_AUTO_TEST_CASE(test_read_XML_pop) {
    int rc;
    xml::IOXMLPop ioxmlpop;
    xml::IOXMLModel ioxmlmodel;
    model::XModel model;
    flame::mem::MemoryManager& memoryManager =
            flame::mem::MemoryManager::GetInstance();

    /* Read model xml */
    ioxmlmodel.readXMLModel("src/io/tests/models/all_data.xml", &model);
    /* Register agents with memory manager */
    unsigned int ii, jj;
    size_t pop_size_hint = 3;
    for (ii = 0; ii < model.getAgents()->size(); ii++) {
        model::XMachine * agent = model.getAgents()->at(ii);
        /* Register agent */
        memoryManager.RegisterAgent(agent->getName());
        /* Register agent memory variables */
        for (jj = 0; jj < agent->getVariables()->size(); jj++) {
            model::XVariable * var =
                    agent->getVariables()->at(jj);
            if (var->getType() == "int") {
                /* Register int variable */
                memoryManager.RegisterAgentVar<int>
                    (agent->getName(), var->getName());
            } else if (var->getType() == "double") {
                /* Register double variable */
                memoryManager.RegisterAgentVar<double>
                    (agent->getName(), var->getName());
            }
        }
        /* Population Size hint */
        memoryManager.HintPopulationSize(agent->getName(), pop_size_hint);
    }

    rc = ioxmlpop.readXMLPop(
            "src/io/tests/models/all_data_its/0_missing.xml",
            &model, &memoryManager);
    BOOST_CHECK(rc == 1);

    rc = ioxmlpop.readXMLPop(
            "src/io/tests/models/all_data_its/0_malformed.xml",
            &model, &memoryManager);
    BOOST_CHECK(rc == 2);

    rc = ioxmlpop.readXMLPop(
            "src/io/tests/models/all_data_its/0_unknown_tag.xml",
            &model, &memoryManager);
    BOOST_CHECK(rc == 3);

    rc = ioxmlpop.readXMLPop(
            "src/io/tests/models/all_data_its/0_unknown_agent.xml",
            &model, &memoryManager);
    BOOST_CHECK(rc == 4);

    rc = ioxmlpop.readXMLPop(
            "src/io/tests/models/all_data_its/0_unknown_variable.xml",
            &model, &memoryManager);
    BOOST_CHECK(rc == 5);

    rc = ioxmlpop.readXMLPop(
            "src/io/tests/models/all_data_its/0_var_not_int.xml",
            &model, &memoryManager);
    BOOST_CHECK(rc == 6);

    rc = ioxmlpop.readXMLPop(
            "src/io/tests/models/all_data_its/0_var_not_double.xml",
            &model, &memoryManager);
    BOOST_CHECK(rc == 6);

    std::string zeroxml = "src/io/tests/models/all_data_its/0.xml";
    rc = ioxmlpop.readXMLPop(zeroxml, &model, &memoryManager);
    BOOST_CHECK(rc == 0);

    /* Test pop data read in */
    /* Test ints data */
    std::vector<int>* roi =
            memoryManager.GetVector<int>("agent_a", "int_single");
    int expectedi[] = {1, 2, 3};
    BOOST_CHECK_EQUAL_COLLECTIONS(expectedi, expectedi+3,
            roi->begin(), roi->end());
    /* Test doubles data */
    std::vector<double>* rod =
            memoryManager.GetVector<double>("agent_a", "double_single");
    double expectedd[] = {0.1, 0.2, 0.3};
    for (ii = 0; ii < rod->size(); ii++) {
        BOOST_CHECK_CLOSE(*(rod->begin()+ii), *(expectedd+ii), 0.0001);
    }

    /* Test pop data written out */
    std::string onexml = "src/io/tests/models/all_data_its/1.xml";
    rc = ioxmlpop.writeXMLPop(onexml, 1, &model, &memoryManager);
    BOOST_CHECK(rc == 0);
    /* Check 0.xml and 1.xml are identical */
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
        /* While at least one file is not at the end */
        while (c0 != EOF || c1 != EOF) {
            if (c0 != c1) differences++;
            if (c0 != EOF) c0 = fgetc(zeroFile);
            if (c1 != EOF) c1 = fgetc(oneFile);
        }
    }
    /* Close files */
    fclose(zeroFile);
    fclose(oneFile);
    BOOST_CHECK(differences == 0);

    /* Remove created 1.xml */
    if (remove(onexml.c_str()) != 0)
        fprintf(stderr, "Warning: Could not delete the generated file: %s\n",
            onexml.c_str());

    /* Reset memory manager as to not affect next test suite */
    memoryManager.Reset();
}

BOOST_AUTO_TEST_SUITE_END()
