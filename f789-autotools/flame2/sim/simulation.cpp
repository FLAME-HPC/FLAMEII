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
#include "flame2/io/io_manager.hpp"
#include "flame2/exe/fifo_task_queue.hpp"
#include "flame2/exe/scheduler.hpp"
#include "simulation.hpp"

namespace flame { namespace sim {

Simulation::Simulation(flame::model::Model * model, std::string pop_file) {
    flame::io::IOManager& iomanager = flame::io::IOManager::GetInstance();
    int rc = 0;

    modelLoaded_ = false;
    popLoaded_ = false;
    model_ = model->getXModel();

    rc += model_->registerWithMemoryManager();

    if (rc == 0) modelLoaded_ = true;

    if (modelLoaded_) {
        popLoaded_ = true;
        iomanager.readPop(pop_file, model_, io::IOManager::xml);
    }
}

void Simulation::start(size_t iterations) {
    if (popLoaded_) {
        // Register agents with memory and task manager
        model_->registerWithTaskManager();

    } else {
        std::fprintf(stderr,
                "Error: Cannot start simulation because pop not loaded\n");
    }

    exe::Scheduler s;
    exe::Scheduler::QueueId q = s.CreateQueue<exe::FIFOTaskQueue>(4);
    s.AssignType(q, exe::Task::AGENT_FUNCTION);
    s.AssignType(q, exe::Task::MB_FUNCTION);
    s.AssignType(q, exe::Task::IO_FUNCTION);

    size_t ii;
    for (ii = 0; ii < iterations; ++ii)
       s.RunIteration();
}

}}  // namespace flame::sim
