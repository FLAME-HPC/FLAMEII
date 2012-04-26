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
#   define BOOST_TEST_MODULE Model
#endif
#include <boost/test/unit_test.hpp>
#include <vector>
#include <string>
#include "../model_manager.hpp"

namespace model = flame::model;

BOOST_AUTO_TEST_SUITE(ModelManager)

/* Test the loading of a model. */
BOOST_AUTO_TEST_CASE(test_load_model) {
//    int rc;
//    model::ModelManager modelManager;

    /* Test unreadable model */
//    rc = modelManager.loadModel("src/io/tests/models/malformed_xml.xml");
//    BOOST_CHECK(rc == 1);
    /* Test invalid model */
//    rc = modelManager.loadModel("src/io/tests/models/all_not_valid.xml");
//    BOOST_CHECK(rc == 2);
    /* Test valid model */
//    rc = modelManager.loadModel("src/io/tests/models/all_data.xml");
//    BOOST_CHECK(rc == 0);
}

/* Test the generating of the task list. */
BOOST_AUTO_TEST_CASE(test_generate_task_list) {
    int rc;
    model::ModelManager modelManager;

    rc = modelManager.loadModel("src/model/tests/models/circles.xml");
    BOOST_CHECK(rc == 0);
    rc = modelManager.generate_task_list();
    BOOST_CHECK(rc == 0);
}

BOOST_AUTO_TEST_SUITE_END()
