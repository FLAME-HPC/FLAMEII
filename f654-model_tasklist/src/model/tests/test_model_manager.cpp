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

/* Test the generating of the task list. */
BOOST_AUTO_TEST_CASE(test_generate_task_list) {
    int rc;
    model::ModelManager modelManager;
    size_t ii;

    /* Load test model */
    rc = modelManager.loadModel("src/model/tests/models/dependency_test.xml");
    BOOST_CHECK(rc == 0);
    /* Generate task list */
    rc = modelManager.generate_task_list();
    BOOST_CHECK(rc == 0);
    /* Set up tasks to test against */
    std::string names[] = {
            "f1_Test", "f2_Test", "f3_Test",
            "sync_start_m1", "sync_start_m2", "sync_start_m3",
            "f4_Test",
            "sync_finish_m1", "sync_finish_m2", "sync_finish_m3",
            "f5_Test",
            "a_Test", "b_Test", "c_Test"};
    size_t level[] = {1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 4, 5, 5, 5};
    size_t priority[] = {5, 5, 5, 10, 10, 10, 5, 1, 1, 1, 5, 0, 0, 0};
    /* Test generated task list */
    std::vector<model::Task*> * tasks = modelManager.get_task_list();
    BOOST_CHECK(tasks->size() == 14);
    if (tasks->size() == 14) {
        for (ii = 0; ii < tasks->size(); ii++) {
            BOOST_CHECK(names[ii] == tasks->at(ii)->getFullName());
            BOOST_CHECK(level[ii] == tasks->at(ii)->getLevel());
            BOOST_CHECK(priority[ii] == tasks->at(ii)->getPriorityLevel());
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
