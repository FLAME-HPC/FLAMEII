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

namespace sim = flame::sim;

BOOST_AUTO_TEST_SUITE(SimManager)

BOOST_AUTO_TEST_CASE(test_graph_layers) {
    // int rc;
    sim::SimManager simManager;


}

BOOST_AUTO_TEST_SUITE_END()
