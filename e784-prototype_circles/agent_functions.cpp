#include <math.h>
#include "flame2.hpp"
#include "message_datatypes.hpp"

#define kr 0.1 /* Stiffness variable for repulsion */
#define distance(x1,y1,x2,y2) (sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)))

FLAME_AGENT_FUNCTION(outputdata) {
  location_message msg;
  msg.x = FLAME.GetMem<double>("x");
  msg.y = FLAME.GetMem<double>("y");
  msg.id = FLAME.GetMem<int>("id");
  
  /* post message with current location */
  FLAME.PostMessage("location", msg);
  return FLAME_AGENT_ALIVE;
}

FLAME_AGENT_FUNCTION(inputdata) {
  location_message msg;
  int id = FLAME.GetMem<int>("id");
  double x = FLAME.GetMem<double>("x");
  double y = FLAME.GetMem<double>("y");
  double diameter = FLAME.GetMem<double>("radius") * 2;
  
  /* tmp vars */
  double p, core_distance;
  double fx = 0, fy = 0;
  
  /* loop through messages */
  MessageIterator iter = FLAME.GetMessageIterator("location");
  for (; !iter.AtEnd(); iter.Next()) {
    msg = iter.GetMessage<location_message>();
    if (msg.id != id) {
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
  FLAME.SetMem<double>("x", FLAME.GetMem<double>("x") + FLAME.GetMem<double>("fx"));
  FLAME.SetMem<double>("y", FLAME.GetMem<double>("y") + FLAME.GetMem<double>("fy"));
  return FLAME_AGENT_ALIVE;
}
