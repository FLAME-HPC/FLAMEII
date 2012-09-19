/*!
 * \file src/model/model_manager.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief ModelManager: management and storage class for model data
 */
#include <string>
#include <vector>
#include <cstdio>
#include "./model_manager.hpp"
#include "../io/io_manager.hpp"

namespace flame { namespace model {

ModelManager::~ModelManager() {
    /* Free task list memory */
    Task * task;
    while (!tasks_.empty()) {
        task = tasks_.back();
        delete task;
        tasks_.pop_back();
    }
}

int ModelManager::loadModel(std::string const& file) {
    int rc;
    flame::io::IOManager ioManager;

    // Read model
    rc = ioManager.loadModel(file, &model_);
    if (rc != 0) {
        std::fprintf(stderr,
            "Error: Model XML file could not be read.\n");
        model_.clear();
        return 1;
    }

    // Validate model
    rc = model_.validate();
    if (rc != 0) {
        std::fprintf(stderr,
            "Error: Model from XML file could not be validated.\n");
        model_.clear();
        return 2;
    }

    // Initialise model (generate graphs and task list)
    rc = model_.initialise();
    if (rc != 0) {
        std::fprintf(stderr,
            "Error: Model from XML file could not be initialised.\n");
        model_.clear();
        return 3;
    }

    return 0;
}

std::vector<Task*> * ModelManager::get_task_list() {
    return &tasks_;
}

}}  // namespace flame::model
