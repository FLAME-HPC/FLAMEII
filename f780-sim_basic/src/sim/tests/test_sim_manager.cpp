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
#include "include/flame.h"
#include "../sim_manager.hpp"
#include "io/io_manager.hpp"
#include "model/model.hpp"

#include "mb/client.hpp"
#include "mb/message.hpp"
#include "mb/message_iterator.hpp"
#include "mb/message_board_manager.hpp"

namespace sim = flame::sim;
namespace io = flame::io;
namespace model = flame::model;

BOOST_AUTO_TEST_SUITE(SimManager)

typedef struct {
  double x, y, z;
  int id;
} my_location_message;

FLAME_AGENT_FUNC(outputdata) {
    printf("outputdata\n");

    location_message msg;
    msg.x = flame_mem_get_double("x");
    msg.y = flame_mem_get_double("y");
    msg.z = 0.0;
    msg.id = flame_mem_get_int("id");

    flame_msg_post("location", &msg);

    return FLAME_AGENT_ALIVE;
}

FLAME_AGENT_FUNC(inputdata) {
    int checksum = 0;
    flame_msg_iterator iter;
    location_message msg;

    iter = flame_msg_get_iterator("location");
    for (; !flame_msg_iterator_end(iter);
            flame_msg_iterator_next(iter)) {
        flame_msg_iterator_get_message(iter, &msg);
        checksum += msg.id;
    }
    flame_msg_iterator_free(iter);

    printf("inputdata checksum=%d\n", checksum);

    return FLAME_AGENT_ALIVE;
}

FLAME_AGENT_FUNC(move) {
    printf("move\n");
    return FLAME_AGENT_ALIVE;
}

BOOST_AUTO_TEST_CASE(test_simManager) {
    // Create model
    flame::model::Model m("src/sim/tests/models/circles/circles.xml");
    // Register agent functions
    m.registerAgentFunction("outputdata", &outputdata);
    m.registerAgentFunction("inputdata", &inputdata);
    m.registerAgentFunction("move", &move);

    sim::Simulation s(&m, "src/sim/tests/models/circles/0.xml");

    // Register message types
    m.registerMessageType<location_message>("location");

    s.start(1);
}

BOOST_AUTO_TEST_SUITE_END()
