/*!
 * \file flame2/sim/simulation.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Simulation: holds and manages simulation information
 */
#include <string>
#include <cstdio>
#include "flame2/config.hpp"
#include "flame2/io/io_manager.hpp"
#include "flame2/exe/fifo_task_queue.hpp"
#include "flame2/exe/scheduler.hpp"
#include "flame2/exceptions/sim.hpp"
#include "simulation.hpp"

namespace flame { namespace sim {

Simulation::Simulation(flame::model::Model * model, std::string pop_file) {
  flame::io::IOManager& iomanager = flame::io::IOManager::GetInstance();

  // check model has been validated
  if (!model->isValidated()) throw flame::exceptions::flame_sim_exception(
      "Model added to a simulation has not be validated");

  model_ = model->getXModel();

  model_->registerWithMemoryManager();
  iomanager.readPop(pop_file, model_, io::IOManager::xml);
}

void Simulation::start(size_t iterations, size_t num_cores) {
  // Register agents with memory and task manager
  model_->registerWithTaskManager();

  exe::Scheduler s;
  exe::Scheduler::QueueId q = s.CreateQueue<exe::FIFOTaskQueue>(num_cores);
  s.AssignType(q, exe::Task::AGENT_FUNCTION);
  s.AssignType(q, exe::Task::MB_FUNCTION);
  s.AssignType(q, exe::Task::IO_FUNCTION);

  size_t ii;
  for (ii = 1; ii <= iterations; ++ii) {
#ifndef TESTBUILD
    printf("Iteration - %lu\n", ii);
#endif
    s.RunIteration();
  }
}

}}  // namespace flame::sim
