/*!
 * \file src/model/tests/test_model_manager.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Test suite for the model manager
 */
#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE IO Pop
#endif
#include <boost/test/unit_test.hpp>
#include <vector>
#include <string>
#include "../model_manager.hpp"
#include "../../io/io_xml_model.hpp"

namespace io = flame::io;
namespace model = flame::model;

BOOST_AUTO_TEST_SUITE(ModelManager)

/* Test the loading of a model. */
BOOST_AUTO_TEST_CASE(test_load_model) {
    int rc;
    io::IOXMLModel ioxmlmodel;
    model::XModel model;


    ioxmlmodel.readXMLModel("src/io/tests/models/all_data.xml", &model);

}

BOOST_AUTO_TEST_SUITE_END()
