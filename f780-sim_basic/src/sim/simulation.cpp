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

namespace flame { namespace sim {

Simulation::Simulation() {
    modelLoaded_ = false;
    popLoaded_ = false;
}

void Simulation::loadModel(std::string model_file) {
    flame::model::ModelManager modelManager;
    int rc;

    rc = modelManager.loadModel(model_file, &model_);

    model_.registerWithMemoryManager();

    if (rc == 0) modelLoaded_ = true;
}

void Simulation::loadPop(std::string pop_file) {
    flame::io::IOManager iomanager;
    int rc;

    if (modelLoaded_) {
        rc = iomanager.readPop(pop_file, &model_, io::IOManager::xml);
        if (rc == 0) popLoaded_ = true;
    }
}

void Simulation::start(size_t iterations) {
    if (popLoaded_) {
        // Register agents with memory and task manager
        model_.registerWithTaskManager();
    }
}

}}  // namespace flame::sim
