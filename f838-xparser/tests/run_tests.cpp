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
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/test/unit_test.hpp>

// Empty libxml error handler
void err(void* /*ctx*/, const char* /*msg*/, ...) {}

namespace fw = boost::unit_test::framework;
namespace fs = boost::filesystem;

struct TestConfig {
    // global setup
    TestConfig() {
        // make sure that test launced from the ./tests directory
        // - assume that the binary has not been moved and use binary path
        //   as expected launch

        fs::path pwd = fs::canonical(fs::current_path());
        fs::path exe_file(fw::master_test_suite().argv[0]);
        fs::path exe_dir = fs::canonical(exe_file.parent_path());
        
        // in the case where libtool creates the binary in .libs and replaces
        // the binary with a wrapper script, the exe_dir would point to the
        // .libs entry instead. Remove that.
        if (exe_dir.filename().string() == ".libs") {
          exe_dir = exe_dir.remove_filename();
        }

        if (exe_dir != pwd) {
          std::cerr << "ERROR: Test must be launched from the "
                    << "tests directory (" << exe_dir << ")" << std::endl;
          exit(1);
        }

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
