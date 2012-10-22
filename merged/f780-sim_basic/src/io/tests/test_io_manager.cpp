/*!
 * \file src/io/tests/test_io_manager.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Test suite for the io manager
 */
#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE IO Model
#endif
#include <boost/test/unit_test.hpp>
#include <vector>
#include <string>
#include <cstdio>
#include "../io_manager.hpp"

namespace io = flame::io;
namespace model = flame::model;
namespace e = flame::exceptions;

BOOST_AUTO_TEST_SUITE(IOManager)

/* Test the reading of XML model files and sub model files. */
BOOST_AUTO_TEST_CASE(test_loadModel) {
    flame::io::IOManager& m = flame::io::IOManager::GetInstance();
    model::XModel model;

    BOOST_CHECK_THROW(m.loadModel("src/io/tests/models/missing.xml",
            &model), e::flame_io_exception);

    BOOST_CHECK_THROW(m.loadModel("src/io/tests/models/malformed_xml.xml",
            &model), e::flame_io_exception);

    BOOST_CHECK_THROW(m.loadModel("src/io/tests/models/not_xmodel.xml",
            &model), e::flame_io_exception);

    BOOST_CHECK_THROW(m.loadModel("src/io/tests/models/xmodelv1.xml",
            &model), e::flame_io_exception);

    BOOST_CHECK_THROW(
            m.loadModel("src/io/tests/models/submodel_enable_error.xml",
            &model), e::flame_io_exception);

    BOOST_CHECK_THROW(
            m.loadModel("src/io/tests/models/submodel_end_not_xml.xml",
            &model), e::flame_io_exception);

    BOOST_CHECK_THROW(m.loadModel("src/io/tests/models/submodel_duplicate.xml",
            &model), e::flame_io_exception);

    BOOST_CHECK_THROW(m.loadModel("src/io/tests/models/submodel_missing.xml",
            &model), e::flame_io_exception);

    BOOST_CHECK_NO_THROW(m.loadModel("src/io/tests/models/all_not_valid.xml",
            &model));
}

/* Test the reading of XML population files. */
BOOST_AUTO_TEST_CASE(test_readPop) {
    flame::io::IOManager& iomanager = flame::io::IOManager::GetInstance();
    model::XModel model;
    flame::mem::MemoryManager& memoryManager =
            flame::mem::MemoryManager::GetInstance();

    /* Read model xml */
    iomanager.loadModel("src/io/tests/models/all_data.xml", &model);

    model.validate();
    model.registerWithMemoryManager();

    BOOST_CHECK_THROW(iomanager.readPop(
            "src/io/tests/models/all_data_its/0_missing.xml",
            &model,
            io::IOManager::xml), e::flame_io_exception);

    BOOST_CHECK_THROW(iomanager.readPop(
            "src/io/tests/models/all_data_its/0_malformed.xml",
            &model,
            io::IOManager::xml), e::flame_io_exception);

    BOOST_CHECK_THROW(iomanager.readPop(
            "src/io/tests/models/all_data_its/0_unknown_tag.xml",
            &model,
            io::IOManager::xml), e::flame_io_exception);

    BOOST_CHECK_THROW(iomanager.readPop(
            "src/io/tests/models/all_data_its/0_unknown_agent.xml",
            &model,
            io::IOManager::xml), e::flame_io_exception);

    BOOST_CHECK_THROW(iomanager.readPop(
            "src/io/tests/models/all_data_its/0_unknown_variable.xml",
            &model,
            io::IOManager::xml), e::flame_io_exception);

    BOOST_CHECK_THROW(iomanager.readPop(
            "src/io/tests/models/all_data_its/0_var_not_int.xml",
            &model,
            io::IOManager::xml), e::flame_io_exception);

    BOOST_CHECK_THROW(iomanager.readPop(
            "src/io/tests/models/all_data_its/0_var_not_double.xml",
            &model,
            io::IOManager::xml), e::flame_io_exception);

    std::string zeroxml = "src/io/tests/models/all_data_its/0.xml";
    BOOST_CHECK_NO_THROW(iomanager.readPop(zeroxml,
            &model,
            io::IOManager::xml));

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
    unsigned int ii;
    for (ii = 0; ii < rod->size(); ii++) {
        BOOST_CHECK_CLOSE(*(rod->begin()+ii), *(expectedd+ii), 0.0001);
    }

    /* Reset memory manager as to not affect next test suite */
    memoryManager.Reset();
}

BOOST_AUTO_TEST_CASE(test_writePop_model) {
    flame::io::IOManager& iomanager = flame::io::IOManager::GetInstance();
    flame::mem::MemoryManager& memoryManager =
                flame::mem::MemoryManager::GetInstance();
    model::XModel model;
    std::string zeroxml = "src/io/tests/models/all_data_its/0.xml";

    // Read model
    iomanager.loadModel("src/io/tests/models/all_data.xml", &model);
    model.registerWithMemoryManager();
    // Read pop
    iomanager.readPop(zeroxml, &model, io::IOManager::xml);

    /* Test pop data written out */
    std::string onexml = "src/io/tests/models/all_data_its/1.xml";
    BOOST_CHECK_NO_THROW(iomanager.finaliseData());
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
    // printf("Removing file: %s\n", onexml.c_str());
    if (remove(onexml.c_str()) != 0)
        fprintf(stderr, "Warning: Could not delete the generated file: %s\n",
            onexml.c_str());

    /* Reset memory manager as to not affect next test suite */
    memoryManager.Reset();
}

BOOST_AUTO_TEST_CASE(test_writePop_1_agent_var) {
    flame::io::IOManager& iomanager = flame::io::IOManager::GetInstance();
    flame::mem::MemoryManager& memoryManager =
                flame::mem::MemoryManager::GetInstance();
    model::XModel model;
    std::string zeroxml = "src/io/tests/models/all_data_its/0.xml";

    // Read model
    iomanager.loadModel("src/io/tests/models/all_data.xml", &model);
    model.registerWithMemoryManager();
    // Read pop
    iomanager.readPop(zeroxml, &model, io::IOManager::xml);
/*
    BOOST_CHECK_THROW(iomanager.writePop("na", "int_single"),
            std::runtime_error);

    BOOST_CHECK_THROW(iomanager.writePop("agent_a", "na"), std::runtime_error);
*/
    BOOST_CHECK_NO_THROW(iomanager.writePop("agent_a", "int_single"));

    // Check contents of 0_agent_a_int_single.xml
    /*FILE *file;
    zeroFile = fopen(zeroxml.c_str(), "r");
    if (file == 0)
            fprintf(stderr, "Warning: Could not open the file: %s\n",
                    zeroxml.c_str());

    // Remove created 0_agent_a_int_single.xml
    if (remove("0_agent_a_int_single.xml") != 0)
        fprintf(stderr, "Warning: Could not delete the generated file: %s\n",
                "0_agent_a_int_single.xml");
*/
    /* Reset memory manager as to not affect next test suite */
    memoryManager.Reset();
}

BOOST_AUTO_TEST_SUITE_END()
