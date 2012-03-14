/*!
 * \file src/io/tests/test_io_xml_model.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Test suite for the io xml model method
 */
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE IO testcases
#include <boost/test/unit_test.hpp>
#include <vector>
#include "../io_xml_model.hpp"

namespace io = flame::io;

BOOST_AUTO_TEST_SUITE(IOModule)

BOOST_AUTO_TEST_CASE(test_class) {
    int rc;
    io::IOXMLModel ioxmlmodel;

    rc = ioxmlmodel.readFile("missing.xml");
    BOOST_CHECK(rc == 1);

    rc = ioxmlmodel.readFile("test.xml");
    BOOST_CHECK(rc == 2);

    rc = ioxmlmodel.readFile("keratinocyte.xml");
    BOOST_CHECK(rc == 0);
}

BOOST_AUTO_TEST_SUITE_END()
