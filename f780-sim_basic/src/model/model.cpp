/*!
 * \file src/model/model.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Model: model object for users to use
 */
#include <cstdio>
#include <string>
#include "model.hpp"
#include "io/io_manager.hpp"

namespace flame {
namespace model {

Model::Model(std::string path_to_model)
    : modelLoaded_(false) {
    flame::io::IOManager ioManager;
    int rc = 0;

    // Load model
    rc = ioManager.loadModel(path_to_model, &model_);
    if (rc != 0) {
        std::fprintf(stderr, "Error: Cannot load model\n");
        model_.clear();
        return;
    }

    // Validate model
    rc = model_.validate();
    if (rc != 0) {
std::fprintf(stderr, "Error: Model from XML file could not be validated.\n");
        model_.clear();
        return;
    }

    modelLoaded_ = true;
}

Model::~Model() {
}

flame::model::XModel * Model::getXModel() {
    return &model_;
}

int Model::registerAgentFunction(std::string name,
        flame::exe::TaskFunction f_ptr) {
    return model_.registerAgentFunction(name, f_ptr);
}

}}  // namespace flame::model
