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

#define kr 0.1 /* Stiffness variable for repulsion */
#define distance(x1, y1, x2, y2) (sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)))

typedef struct {
  double x, y;
  int id;
} my_location_message;

FLAME_AGENT_FUNC(outputdata) {
    // printf("outputdata\n");

    location_message msg;
    msg.x = flame_mem_get_double("x");
    msg.y = flame_mem_get_double("y");
    msg.id = flame_mem_get_int("id");

    flame_msg_post("location", &msg);

    return FLAME_AGENT_ALIVE;
}

FLAME_AGENT_FUNC(inputdata) {
    flame_msg_iterator iter;
    location_message msg;
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

    flame::io::IOManager& iomanager = flame::io::IOManager::GetInstance();

    iomanager.writePop("Circle", "x");

    iomanager.writePop("Circle", "id");

    /*flame::mem::MemoryManager& memoryManager =
                flame::mem::MemoryManager::GetInstance();
    std::vector<double>* rod =
                memoryManager.GetVector<double>("Circle", "x");
    size_t ii;
    for (ii = 0; ii < 10; ii++) {
        printf("x: %f\n", rod->at(ii));
        BOOST_CHECK_CLOSE(*(rod->begin()+ii), *(expectedd+ii), 0.0001);
    }*/
}

BOOST_AUTO_TEST_SUITE_END()
