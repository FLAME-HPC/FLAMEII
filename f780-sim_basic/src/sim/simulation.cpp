/*!
 * \file src/sim/simulation.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Simulation: holds and manages simulation information
 */
#include <string>
#include <cstdio>
#include "./simulation.hpp"
#include "io/io_manager.hpp"
#include "exe/fifo_task_queue.hpp"
#include "exe/scheduler.hpp"

namespace flame { namespace sim {

Simulation::Simulation(flame::model::Model * model, std::string pop_file) {
    flame::io::IOManager& iomanager = flame::io::IOManager::GetInstance();
    int rc = 0;

    modelLoaded_ = false;
    popLoaded_ = false;
    model_ = model->getXModel();

    rc += model_->registerWithMemoryManager();
    rc += model_->registerWithMessageBoardManager();

    if (rc == 0) modelLoaded_ = true;

    if (modelLoaded_) {
        popLoaded_ = true;
        try {
            iomanager.readPop(pop_file, model_, io::IOManager::xml);
        }
        catch(const flame::exceptions::flame_io_exception& E) {
            std::fprintf(stderr, "Error: %s\n", E.what());
            popLoaded_ = false;
        }
    }
}

void Simulation::start(size_t iterations) {
    size_t ii;

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
    for (ii = 0; ii < iterations; ++ii)
        s.RunIteration();
}

}}  // namespace flame::sim
