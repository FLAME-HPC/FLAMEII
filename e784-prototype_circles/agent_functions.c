#include <math.h>
#include "flame.h"
#include "message_datatypes.h"

#define kr 0.1 /* Stiffness variable for repulsion */
#define distance(x1,y1,x2,y2) (sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)))

FLAME_AGENT_FUNCTION(outputdata) {
  location_message msg;
  msg.x = flame_mem_get_double("x");
  msg.y = flame_mem_get_double("y");
  msg.id = flame_mem_get_int("id");
  
  /* post message with current location */
  flame_msg_post("location", &msg);
  return FLAME_AGENT_ALIVE;
}

FLAME_AGENT_FUNCTION(inputdata) {
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

FLAME_AGENT_FUNCTION(move) {
  /* update position based on accumulated forces */
  flame_mem_set_double("x", flame_mem_get_double("x") + flame_mem_get_double("fx"));
  flame_mem_set_double("y", flame_mem_get_double("y") + flame_mem_get_double("fy"));
  return FLAME_AGENT_ALIVE;
}
