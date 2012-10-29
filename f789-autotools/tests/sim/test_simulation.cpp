/*!
 * \file tests/sim/test_simulation.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Test suite for the simulation class
 */
#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Sim
#endif
#include <boost/test/unit_test.hpp>
#include <vector>
#include <string>
#include "flame2/compat/C/flame2.h"
#include "flame2/sim/sim_manager.hpp"
#include "flame2/io/io_manager.hpp"
#include "flame2/model/model.hpp"

#include "flame2/mb/client.hpp"
#include "flame2/mb/message.hpp"
#include "flame2/mb/message_iterator.hpp"
#include "flame2/mb/message_board_manager.hpp"

namespace sim = flame::sim;
namespace io = flame::io;
namespace model = flame::model;

typedef struct {
    double x, y;
    int id;
} my_location_message;

// The structure must support the << operator for it to be store in the board
inline std::ostream &operator<<(std::ostream &os,
        const my_location_message& ob) {
    os << "{" << ob.x << ", " << ob.y << ", " << ob.id << "}";
    return os;
}

BOOST_AUTO_TEST_SUITE(Simulation)

#define kr 0.1 /* Stiffness variable for repulsion */
#define distance(x1, y1, x2, y2) (sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)))

FLAME_AGENT_FUNC(outputdata) {
    // printf("outputdata\n");

    my_location_message msg;
    msg.x = flame_mem_get_double("x");
    msg.y = flame_mem_get_double("y");
    msg.id = flame_mem_get_int("id");

    flame_msg_post("location", &msg);

    return FLAME_AGENT_ALIVE;
}

FLAME_AGENT_FUNC(inputdata) {
    flame_msg_iterator iter;
    my_location_message msg;
    double x = flame_mem_get_double("x");
    double y = flame_mem_get_double("y");
    double diameter = flame_mem_get_double("radius") * 2;

    /* tmp vars */
    double p, core_distance;
    double fx = 0, fy = 0;

    /* loop through messages */
    iter = flame_msg_get_iterator("location");
    for (; !flame_msg_iterator_end(iter); flame_msg_iterator_next(iter)) {
      flame_msg_iterator_get_message(iter, &msg);
      if (msg.id != flame_mem_get_int("id")) {
        core_distance = distance(x, y, msg.x, msg.y);

        if (core_distance < diameter) {
          p = kr * diameter / core_distance;
          /* accumulate forces */
          fx += (x - msg.x) * p;
          fy += (y - msg.y) * p;
        }
      }
    }
    flame_msg_iterator_free(iter);

    /* store forces in agent mem */
    flame_mem_set_double("fx", fx);
    flame_mem_set_double("fy", fy);
    return FLAME_AGENT_ALIVE;
}

FLAME_AGENT_FUNC(move) {
    /* update position based on accumulated forces */
    flame_mem_set_double("x", flame_mem_get_double("x") +
            flame_mem_get_double("fx"));
    flame_mem_set_double("y", flame_mem_get_double("y") +
            flame_mem_get_double("fy"));
    return FLAME_AGENT_ALIVE;
}

BOOST_AUTO_TEST_CASE(test_simulation) {
    // Create model
    flame::model::Model m("sim/models/circles/circles.xml");
    // Register agent functions
    m.registerAgentFunction("outputdata", &outputdata);
    m.registerAgentFunction("inputdata", &inputdata);
    m.registerAgentFunction("move", &move);

    sim::Simulation s(&m, "sim/models/circles/0.xml");

    // Register message types
    m.registerMessageType<my_location_message>("location");

    s.start(1);

    // Reset memory manager
    flame::mem::MemoryManager::GetInstance().Reset();

    // Try and use generated output as input
    BOOST_CHECK_NO_THROW(
        sim::Simulation s2(&m, "sim/models/circles/1.xml"));

    /* Remove created all_data.xsd */
    if (remove("sim/models/circles/1.xml") != 0)
    fprintf(stderr, "Warning: Could not delete the generated file: %s\n",
            "sim/models/circles/1.xml");

    // Reset memory manager
    flame::mem::MemoryManager::GetInstance().Reset();
    flame::exe::TaskManager::GetInstance().Reset();
    flame::compat::c::CompatibilityManager::GetInstance().Reset();
    flame::mb::MessageBoardManager::GetInstance().Reset();
}

BOOST_AUTO_TEST_SUITE_END()
