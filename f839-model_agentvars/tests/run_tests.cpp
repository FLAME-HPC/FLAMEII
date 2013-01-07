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
#include <libxml/parser.h>
#include <libxml/xmlerror.h>
#include <boost/test/unit_test.hpp>

// Empty libxml error handler
void err(void* /*ctx*/, const char* /*msg*/, ...) {}

struct TestConfig {
    // global setup
    TestConfig() {
        // Stop libxml error output
        xmlGenericErrorFunc handler = (xmlGenericErrorFunc)err;
        initGenericErrorDefaultFunc(&handler);
    }

    // global teardown
    ~TestConfig() {
        // Flush libxml memory
        xmlCleanupParser();
    }
};

BOOST_GLOBAL_FIXTURE(TestConfig)
