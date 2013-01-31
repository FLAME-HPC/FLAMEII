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
#include "flame2/exceptions/sim.hpp"
#include "flame2/sim/sim_manager.hpp"
#include "flame2/io/io_manager.hpp"
#include "flame2/model/model.hpp"
#include "flame2/mb/client.hpp"
#include "flame2/mb/message_iterator.hpp"
#include "flame2/mb/message_board_manager.hpp"

#include "flame2/api/flame2.hpp"

namespace sim = flame::sim;
namespace io = flame::io;
namespace model = flame::model;
namespace e = flame::exceptions;

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

FLAME_AGENT_FUNCTION(outputdata) {
  // printf("outputdata\n");

  my_location_message msg;
  msg.x = FLAME.GetMem<double>("x");
  msg.y = FLAME.GetMem<double>("y");
  msg.id = FLAME.GetMem<int>("id");

  FLAME.PostMessage<my_location_message>("location", msg);
  return FLAME_AGENT_ALIVE;
}

FLAME_AGENT_FUNCTION(inputdata) {
  MessageIterator iter;
  my_location_message msg;
  double x = FLAME.GetMem<double>("x");
  double y = FLAME.GetMem<double>("y");
  double diameter = FLAME.GetMem<double>("radius") * 2;

  /* tmp vars */
  double p, core_distance;
  double fx = 0, fy = 0;

  /* loop through messages */
  iter = FLAME.GetMessageIterator("location");
  for (; !iter.AtEnd(); iter.Next()) {
    msg = iter.GetMessage<my_location_message>();
    if (msg.id != FLAME.GetMem<int>("id")) {
      core_distance = distance(x, y, msg.x, msg.y);

      if (core_distance < diameter) {
        p = kr * diameter / core_distance;
        /* accumulate forces */
        fx += (x - msg.x) * p;
        fy += (y - msg.y) * p;
      }
    }
  }

  /* store forces in agent mem */
  FLAME.SetMem<double>("fx", fx);
  FLAME.SetMem<double>("fy", fy);
  return FLAME_AGENT_ALIVE;
}

FLAME_AGENT_FUNCTION(move) {
  /* update position based on accumulated forces */
  FLAME.SetMem<double>("x", FLAME.GetMem<double>("x") +
      FLAME.GetMem<double>("fx"));
  FLAME.SetMem<double>("y", FLAME.GetMem<double>("y") +
      FLAME.GetMem<double>("fy"));
  return FLAME_AGENT_ALIVE;
}

BOOST_AUTO_TEST_CASE(test_simulation) {
  // Create model
  flame::model::Model m("sim/models/circles/circles.xml");
  // Register agent functions
  m.registerAgentFunction("outputdata", &outputdata);
  m.registerAgentFunction("inputdata", &inputdata);
  m.registerAgentFunction("move", &move);

    sim::Simulation s(m, "sim/models/circles/0.xml");

  // Register message types
  m.registerMessageType<my_location_message>("location");

  s.start(1);

  // Reset memory manager
  flame::mem::MemoryManager::GetInstance().Reset();
  flame::exe::TaskManager::GetInstance().Reset();
  flame::mb::MessageBoardManager::GetInstance().Reset();

  // Try and use generated output as input
  BOOST_CHECK_NO_THROW(
      sim::Simulation s2(m, "sim/models/circles/1.xml"));

  // Remove created all_data.xsd
/*  if (remove("sim/models/circles/1.xml") != 0)
    fprintf(stderr, "Warning: Could not delete the generated file: %s\n",
        "sim/models/circles/1.xml");*/

  // Reset memory manager
  flame::mem::MemoryManager::GetInstance().Reset();
  flame::exe::TaskManager::GetInstance().Reset();
  flame::mb::MessageBoardManager::GetInstance().Reset();
}

//! Check exception throwing of unvalidated model being added to a simulation
BOOST_AUTO_TEST_CASE(unvalidated_model) {
  // unvalidated model
  flame::model::Model model;

  BOOST_CHECK_THROW(sim::Simulation s2(model, ""), e::flame_sim_exception);
}

BOOST_AUTO_TEST_SUITE_END()
