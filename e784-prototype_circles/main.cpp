#include <iostream>
#include "flame2/exe/task_manager.hpp"
#include "flame2/exe/scheduler.hpp"
#include "flame2/exe/splitting_fifo_task_queue.hpp"
#include "flame2/io/io_manager.hpp"
#include "flame2/model/xmodel.hpp"
#include "flame2/mb/message_board_manager.hpp"

#include "agent_functions.hpp"
#include "message_datatypes.hpp"

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
  
  // We shouldn't really need a model file since main.cpp is meant to be
  // generated based on circles.xml
  std::string model_path;
  model_path = "circles.xml";

  
  // Load model and population data
  flame::io::IOManager& iomanager = flame::io::IOManager::GetInstance();
  model::XModel model;
  iomanager.loadModel(model_path, &model);
  model.validate();
  model.registerWithMemoryManager();
  iomanager.readPop(pop_path, &model, flame::io::IOManager::xml);

  // Register Message Boards
  flame::mb::MessageBoardManager& mb_mbr = flame::mb::MessageBoardManager::GetInstance();
  mb_mbr.RegisterMessage<location_message>("location");

  // Define tasks
  flame::exe::TaskManager& task_mgr = flame::exe::TaskManager::GetInstance();
  
  // Register Agent Tasks (+ define access to memory/board)
  flame::exe::Task& t1 = task_mgr.CreateAgentTask(
                "__AGENT__outputdata", "Circle", &outputdata);
  t1.AllowAccess("x", false);
  t1.AllowAccess("y", false);
  t1.AllowAccess("radius", false);
  t1.AllowAccess("id", false);
  t1.AllowMessagePost("location");
  
  flame::exe::Task& t2 = task_mgr.CreateAgentTask(
                "__AGENT__inputdata", "Circle", &inputdata);
  t2.AllowAccess("x", false);
  t2.AllowAccess("y", false);
  t2.AllowAccess("radius", false);
  t2.AllowAccess("id", false);
  t2.AllowAccess("fx", true);
  t2.AllowAccess("fy", true);
  t2.AllowMessageRead("location");
  
  flame::exe::Task& t3 = task_mgr.CreateAgentTask(
                "__AGENT__move", "Circle", &move);
  t3.AllowAccess("x", true);
  t3.AllowAccess("y", true);
  t3.AllowAccess("fx", false);
  t3.AllowAccess("fy", false);
  
  // Register framework tasks (IO/Mem)
  task_mgr.CreateMessageBoardTask("__MB__sync_location", "location", 
                                  flame::exe::MessageBoardTask::OP_SYNC);
  task_mgr.CreateMessageBoardTask("__MB__clear_location", "location", 
                                  flame::exe::MessageBoardTask::OP_CLEAR);

  
  // Dependencies for agent functions
  task_mgr.AddDependency("__MB__sync_location", "__AGENT__outputdata");
  task_mgr.AddDependency("__AGENT__inputdata", "__MB__sync_location");
  task_mgr.AddDependency("__MB__clear_location", "__AGENT__inputdata");
  task_mgr.AddDependency("__AGENT__move", "__AGENT__inputdata");

  // Task and dependencies for data output
  task_mgr.CreateIOTask("__IO__output_Model_initialise", "", "",
                          flame::exe::IOTask::OP_INIT);
  task_mgr.CreateIOTask("__IO__output_Model_finalise", "", "",
                          flame::exe::IOTask::OP_FIN);
  task_mgr.AddDependency("__IO__output_Model_finalise", "__IO__output_Model_initialise");
  task_mgr.AddDependency("__IO__output_Model_finalise", "__AGENT__move");
  
  // Initialise scheduler
  flame::exe::Scheduler s;
  flame::exe::Scheduler::QueueId q = s.CreateQueue<flame::exe::SplittingFIFOTaskQueue>(num_cores);
  s.AssignType(q, flame::exe::Task::AGENT_FUNCTION);
  s.AssignType(q, flame::exe::Task::MB_FUNCTION);
  s.AssignType(q, flame::exe::Task::IO_FUNCTION);
  s.SetSplittable(flame::exe::Task::AGENT_FUNCTION);  // Agent tasks can be split up
  
  // Run iterations
  for (size_t i = 1; i <= num_iters; ++i) {
    std::cout << "Iteration " << i << std::endl;
    s.RunIteration();
  }
  
}
