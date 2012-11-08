/*!
 * \file tests/model/test_model_validation.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Test suite for model validate
 */
#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE XGraph
#endif
#include <boost/test/unit_test.hpp>
#include "flame2/model/xmodel_validate.hpp"
#include "flame2/io/io_manager.hpp"

namespace model = flame::model;
namespace xml = flame::io::xml;

BOOST_AUTO_TEST_SUITE(xmodel_validation)

BOOST_AUTO_TEST_CASE(validate_model) {
    int rc;
    flame::io::IOManager& iomanager = flame::io::IOManager::GetInstance();
    model::XModel model;

    /* Test model validation using single file with all errors.
     * Possibly in future use a correct file and mutate for each
     * error and test separately. */
    iomanager.loadModel("io/models/all_not_valid.xml", &model);
    rc = model.validate();
    BOOST_CHECK(rc == 41);
}

BOOST_AUTO_TEST_SUITE_END()
