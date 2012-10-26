#include "compat/C/compatibility_manager.hpp"
//#include "mem/memory_manager.hpp"
#include "exe/task_manager.hpp"
#include "exe/scheduler.hpp"
#include "exe/splitting_fifo_task_queue.hpp"
#include "io/io_manager.hpp"
#include "model/xmodel.hpp"

#include "agent_functions.h"
#include "message_datatypes.h"

int main(int argc, const char* argv[]) {

  // Grab num_cores and num_iters from input args
  size_t num_cores, num_iters;
  // Grab pop file path and model path
  std::string pop_path;
  std::string model_path;

  // Hardcode for now
  num_cores = 2;
  num_iters = 2;
  pop_path = "0.xml";
  model_path = "circles.xml";

  /*
  // Define agents and population
  flame::mem::MemoryManager& mem_mgr = flame::mem::MemoryManager::GetInstance();
  mem_mgr.RegisterAgent("Circle");
  mem_mgr.RegisterAgentVar<double>("Circle", "x");
  mem_mgr.RegisterAgentVar<double>("Circle", "y");
  mem_mgr.RegisterAgentVar<double>("Circle", "fx");
  mem_mgr.RegisterAgentVar<double>("Circle", "fy");
  mem_mgr.RegisterAgentVar<double>("Circle", "radius");
  mem_mgr.RegisterAgentVar<int>("Circle", "id");
  */
  
  // Register Message Boards
  // For C API we cannnot handle individual vars in messages. Not yet.
  // Might have to do it via compatibilityManager :(
  flame::compat::c::CompatibilityManager& compat_mgr = flame::compat::c::CompatibilityManager::GetInstance();
  compat_mgr.RegisterMessage<location_message>("location");
  
  /*
  // Populate
  flame::io::IOManager io_mgr;
  // Currently readPop depends on read in model
  // Need to change dependency to memory mgr?
  io_mgr.readPop(pop_path, xmodel, FileType::xml);
  */

  // Load data
  flame::io::IOManager& iomanager = flame::io::IOManager::GetInstance();
  model::XModel model;
  iomanager.loadModel(model_path, &model);
  model.validate();
  model.registerWithMemoryManager();
  iomanager.readPop("./0.xml", &model, flame::io::IOManager::xml);

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
  task_mgr.CreateIOTask("__IO__output_Circle_x", "Circle", "x",
                                  flame::exe::IOTask::OP_OUTPUT);
  task_mgr.CreateIOTask("__IO__output_Circle_y", "Circle", "y",
                                  flame::exe::IOTask::OP_OUTPUT);
  task_mgr.CreateIOTask("__IO__output_Circle_fx", "Circle", "fx",
                                  flame::exe::IOTask::OP_OUTPUT);
  task_mgr.CreateIOTask("__IO__output_Circle_fy", "Circle", "fy",
                                  flame::exe::IOTask::OP_OUTPUT);
  task_mgr.CreateIOTask("__IO__output_Circle_radius", "Circle", "radius",
                                  flame::exe::IOTask::OP_OUTPUT);              
  task_mgr.CreateIOTask("__IO__output_Circle_id", "Circle", "id",
                                  flame::exe::IOTask::OP_OUTPUT);
  // Need to remember model environment constants
  //task_mgr.CreateIOTask("__IO__output_Model_env", "Circles", "env" 
  //                                flame::exe::IOTask::OP_OUTPUT);   
                                  
  // Add task dependencies
  //taskManager.AddDependency("__AGENT__outputdata", "__IO__output_Circle_radius");
  //taskManager.AddDependency("__AGENT__outputdata", "__IO__output_Circle_id");
  task_mgr.AddDependency("__MB__sync_location", "__AGENT__outputdata");
  task_mgr.AddDependency("__AGENT__inputdata", "__MB__sync_location");
  task_mgr.AddDependency("__MB__clear_location", "__AGENT__inputdata");
  task_mgr.AddDependency("__AGENT__move", "__AGENT__inputdata");
  task_mgr.AddDependency("__IO__output_Circle_fx", "__AGENT__inputdata");
  task_mgr.AddDependency("__IO__output_Circle_fy", "__AGENT__inputdata");
  task_mgr.AddDependency("__IO__output_Circle_x", "__AGENT__move");
  task_mgr.AddDependency("__IO__output_Circle_y", "__AGENT__move");

  
  // Initialise scheduler
  flame::exe::Scheduler s;
  flame::exe::Scheduler::QueueId q = s.CreateQueue<flame::exe::SplittingFIFOTaskQueue>(num_cores);
  s.AssignType(q, flame::exe::Task::AGENT_FUNCTION);
  s.AssignType(q, flame::exe::Task::MB_FUNCTION);
  s.AssignType(q, flame::exe::Task::IO_FUNCTION);
  s.SetSplittable(flame::exe::Task::AGENT_FUNCTION);  // Agent tasks can be split up
  
  // Run iterations
  for (size_t i = 0; i < num_iters; ++i) {
    s.RunIteration();
  }
  
}
