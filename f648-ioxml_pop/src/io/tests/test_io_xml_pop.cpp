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
#define BOOST_TEST_MODULE IO testcases
#include <boost/test/unit_test.hpp>
#include <vector>
#include "../io_xml_model.hpp"
#include "../io_xml_pop.hpp"

namespace io = flame::io;

BOOST_AUTO_TEST_SUITE(IOModule)

/* Test the reading of XML population files. */
BOOST_AUTO_TEST_CASE(test_read_XML_model) {
    int rc;
    io::IOXMLPop ioxmlpop;
    io::IOXMLModel ioxmlmodel;
    io::XModel model;

    /* Test model validation using single file with all errors.
     * Possibly in future use a correct file and mutate for each
     * error and test separately. */
    ioxmlmodel.readXMLModel("tests/models/all_data.xml", &model);
    rc = ioxmlpop.readXMLPop("tests/models/all_data_its/0.xml", &model);
    BOOST_CHECK(rc == 0);
}

BOOST_AUTO_TEST_SUITE_END()
