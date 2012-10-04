/*!
 * \file src/sim/tests/test_sim_manager.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Test suite for the simulation manager
 */
#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Sim
#endif
#include <boost/test/unit_test.hpp>
#include <vector>
#include <string>
#include "../sim_manager.hpp"
#include "io/io_manager.hpp"
#include "model/model_manager.hpp"

namespace sim = flame::sim;
namespace io = flame::io;
namespace model = flame::model;

BOOST_AUTO_TEST_SUITE(SimManager)

BOOST_AUTO_TEST_CASE(test_simManager) {
    // int rc;
    model::XModel model;
    sim::Simulation sim1;

    // Add <xml> to both of these
    // sim1.loadModel("src/sim/tests/models/circles/circles.xml");
    sim1.loadModel("src/sim/tests/models/test01/test01.xml");
    // Can only do this after loading a model
    // sim1.loadPop("src/sim/tests/models/circles/0.xml");
    sim1.loadPop("src/sim/tests/models/test01/0.xml");
    // Run for one iteration
    sim1.start(1);
}

BOOST_AUTO_TEST_SUITE_END()
