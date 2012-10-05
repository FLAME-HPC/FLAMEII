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
    int rc = 0;

    rc += modelManager.loadModel(model_file, &model_);
    rc += modelManager.registerModelWithMemoryManager(&model_);
    rc += modelManager.registerModelWithMessageBoardManager(&model_);

    if (rc == 0) modelLoaded_ = true;
}

void Simulation::loadPop(std::string pop_file) {
    flame::io::IOManager iomanager;
    int rc;

    if (modelLoaded_) {
        rc = iomanager.readPop(pop_file, &model_, io::IOManager::xml);
        if (rc == 0) popLoaded_ = true;
    } else {
        std::fprintf(stderr, "Error: Cannot load pop because model not loaded\n");
    }
}

void Simulation::start(size_t iterations) {
    flame::model::ModelManager modelManager;

    if (popLoaded_) {
        // Register agents with memory and task manager
        modelManager.registerModelWithTaskManager(&model_);
    } else {
        std::fprintf(stderr, "Error: Cannot start simulation because pop not loaded\n");
    }
}

}}  // namespace flame::sim
