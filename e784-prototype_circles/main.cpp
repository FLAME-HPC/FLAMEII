#include <cstdio>
#include <iostream>
#include <sys/time.h>
#include "flame2/exe/task_manager.hpp"
#include "flame2/exe/scheduler.hpp"
#include "flame2/exe/splitting_fifo_task_queue.hpp"
#include "flame2/io/io_manager.hpp"
#include "flame2/model/xmodel.hpp"
#include "flame2/mb/message_board_manager.hpp"

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
  
  model::XFunction * function = 0;
  model::XIOput * ioput = 0;
  // Create model
  model::XModel model;
  model::XMachine * circle_agent = model.addAgent("Circle");
  circle_agent->addVariable("int", "id");
  circle_agent->addVariable("double", "x");
  circle_agent->addVariable("double", "y");
  circle_agent->addVariable("double", "fx");
  circle_agent->addVariable("double", "fy");
  circle_agent->addVariable("double", "radius");

  function = circle_agent->addFunction();
  function->setName("outputdata");
  function->setCurrentState("0");
  function->setNextState("1");
  function->setMemoryAccessInfoAvailable(true);
  function->addReadOnlyVariable("x");
  function->addReadOnlyVariable("y");
  function->addReadOnlyVariable("radius");
  function->addReadOnlyVariable("id");
  ioput = function->addOutput();
  ioput->setMessageName("location");

  function = circle_agent->addFunction();
  function->setName("inputdata");
  function->setCurrentState("1");
  function->setNextState("2");
  function->setMemoryAccessInfoAvailable(true);
  function->addReadOnlyVariable("x");
  function->addReadOnlyVariable("y");
  function->addReadOnlyVariable("radius");
  function->addReadOnlyVariable("id");
  function->addReadWriteVariable("fx");
  function->addReadWriteVariable("fy");
  ioput = function->addInput();
  ioput->setMessageName("location");

  function = circle_agent->addFunction();
  function->setName("move");
  function->setCurrentState("2");
  function->setNextState("3");
  function->setMemoryAccessInfoAvailable(true);
  function->addReadWriteVariable("x");
  function->addReadWriteVariable("y");
  function->addReadOnlyVariable("fx");
  function->addReadOnlyVariable("fy");

  model::XMessage * xmessage = model.addMessage();
  xmessage->setName("location");
  xmessage->addVariable("double", "x");
  xmessage->addVariable("double", "y");
  xmessage->addVariable("double", "z");
  xmessage->addVariable("int", "id");

  model.validate();

  // Above handled by model::Model

  // Register agent functions
  model.registerAgentFunction("outputdata", &outputdata);
  model.registerAgentFunction("inputdata", &inputdata);
  model.registerAgentFunction("move", &move);
  // Register Message Boards
  //model.registerMessage<location_message>("location");
  flame::mb::MessageBoardManager& mb_mbr = flame::mb::MessageBoardManager::GetInstance();
  mb_mbr.RegisterMessage<location_message>("location");

  // Handled by sim::Simulation initialisation
  model.registerWithMemoryManager();
  // Create population data
  flame::io::IOManager& iomanager = flame::io::IOManager::GetInstance();
  iomanager.readPop(pop_path, &model, flame::io::IOManager::xml);

  // Handled by sim::Simulation start method
  model.registerWithTaskManager();
  
  // Initialise scheduler
  flame::exe::Scheduler s;
  flame::exe::Scheduler::QueueId q = s.CreateQueue<flame::exe::SplittingFIFOTaskQueue>(num_cores);
  s.AssignType(q, flame::exe::Task::AGENT_FUNCTION);
  s.AssignType(q, flame::exe::Task::MB_FUNCTION);
  s.AssignType(q, flame::exe::Task::IO_FUNCTION);
  s.SetSplittable(flame::exe::Task::AGENT_FUNCTION);  // Agent tasks can be split up
  
  // Run iterations
  double start_time, stop_time, total_time;
  start_time = get_time();
  for (size_t i = 1; i <= num_iters; ++i) {
    std::cout << "Iteration " << i << std::endl;
    s.RunIteration();
  }

  // Above handled by model::simulation::start()

  stop_time = get_time();
  total_time = stop_time - start_time;
  printf("Execution time - %d:%02d:%03d [mins:secs:msecs]\n",
           (int)(total_time/60), 
           ((int)total_time)%60, 
           (((int)(total_time * 1000.0)) % 1000));
  
  return 0;
}
