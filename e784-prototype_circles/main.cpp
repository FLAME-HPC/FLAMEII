#include <cstdio>
#include <iostream>
#include <sys/time.h>
#include "flame2/exe/task_manager.hpp"
#include "flame2/exe/scheduler.hpp"
#include "flame2/exe/splitting_fifo_task_queue.hpp"
#include "flame2/io/io_manager.hpp"
#include "flame2/model/model.hpp"
#include "flame2/mb/message_board_manager.hpp"
#include "flame2/sim/simulation.hpp"

#include "agent_functions.hpp"
#include "message_datatypes.hpp"

// returns time in seconds
static double get_time(void) {
  struct timeval now;
  gettimeofday(&now, NULL);
  return now.tv_sec + (now.tv_usec * 1.e-6);
}

int main(int argc, const char* argv[]) {

  if (argc < 3 || argc > 4) {
    std::cerr << "Usage: " << argv[0];
    std::cerr << " POP_DATA NUM_ITERATIONS [NUM_CORES]" << std::endl;
    exit(1);
  }

  std::string pop_path = argv[1];
  size_t num_iters = (size_t)atoi(argv[2]);
  if (num_iters == 0) {
    std::cerr << "Invalid value for NUM_ITERATIONS" << std::endl;
    exit(2);
  }

  size_t num_cores = 1; // default to single core run
  if (argc > 3) {
    num_cores = (size_t)atoi(argv[3]);
    if (num_cores == 0) {
      std::cerr << "Invalid value for NUM_CORES" << std::endl;
      exit(3);
    }
  }
  
  // Create model
  model::Model model;
  model::XModel * xmodel = model.getXModel();
  model::XMachine * circle_agent = xmodel->addAgent("Circle");
  circle_agent->addVariable("int", "id");
  circle_agent->addVariable("double", "x");
  circle_agent->addVariable("double", "y");
  circle_agent->addVariable("double", "fx");
  circle_agent->addVariable("double", "fy");
  circle_agent->addVariable("double", "radius");

  model::XFunction * function0 = circle_agent->addFunction("outputdata", "0", "1");
  function0->setMemoryAccessInfoAvailable(true);
  function0->addReadOnlyVariable("x");
  function0->addReadOnlyVariable("y");
  function0->addReadOnlyVariable("radius");
  function0->addReadOnlyVariable("id");
  function0->addOutput("location");

  model::XFunction * function1 = circle_agent->addFunction("inputdata", "1", "2");
  function1->setMemoryAccessInfoAvailable(true);
  function1->addReadOnlyVariable("x");
  function1->addReadOnlyVariable("y");
  function1->addReadOnlyVariable("radius");
  function1->addReadOnlyVariable("id");
  function1->addReadWriteVariable("fx");
  function1->addReadWriteVariable("fy");
  function1->addInput("location");

  model::XFunction * function2 = circle_agent->addFunction("move", "2", "3");
  function2->setMemoryAccessInfoAvailable(true);
  function2->addReadWriteVariable("x");
  function2->addReadWriteVariable("y");
  function2->addReadOnlyVariable("fx");
  function2->addReadOnlyVariable("fy");

  model::XMessage * xmessage = xmodel->addMessage("location");
  xmessage->addVariable("double", "x");
  xmessage->addVariable("double", "y");
  xmessage->addVariable("double", "z");
  xmessage->addVariable("int", "id");

  // Validate model
  xmodel->validate();

  // Register agent functions
  model.registerAgentFunction("outputdata", &outputdata);
  model.registerAgentFunction("inputdata", &inputdata);
  model.registerAgentFunction("move", &move);
  // Register message types
  model.registerMessageType<location_message>("location");

  // Create simulation using model and path to initial pop
  flame::sim::Simulation s(&model, pop_path);

  double start_time, stop_time, total_time;
  start_time = get_time();

  // Run simulation
  s.start(num_iters);

  stop_time = get_time();
  total_time = stop_time - start_time;
  printf("Execution time - %d:%02d:%03d [mins:secs:msecs]\n",
           (int)(total_time/60), 
           ((int)total_time)%60, 
           (((int)(total_time * 1000.0)) % 1000));
  
  return 0;
}
