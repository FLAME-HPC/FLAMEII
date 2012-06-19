/*!
 * \file run_tests.cpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Driver file for running all tests (uses Boost.Tests)
 */
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Flame Test Suite
#include <boost/test/unit_test.hpp>
#include <libxml/parser.h>

struct TestConfig {
    TestConfig() {
        /* global setup */
    }
    ~TestConfig() {
        /* global teardown */
        printf("Clean up xmllib\n");
        xmlCleanupParser();
    }
};

BOOST_GLOBAL_FIXTURE(TestConfig)
