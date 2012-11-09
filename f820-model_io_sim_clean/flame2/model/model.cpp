/*!
 * \file flame2/model/model.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Model: model object for users to use
 */
#include <cstdio>
#include <string>
#include "flame2/config.hpp"
#include "flame2/io/io_manager.hpp"
#include "flame2/exceptions/model.hpp"
#include "model.hpp"

namespace flame {
namespace model {

/*!
 * \brief Constructs Model
 *
 * Initialises Model by loading model using path string and
 * validates the model.
 */
Model::Model(std::string path_to_model) {
    // Load model
    flame::io::IOManager::GetInstance().loadModel(path_to_model, &model_);

    // Validate model
    if (model_.validate() != 0) throw flame::exceptions::flame_model_exception(
            "Model could not be validated");
}

flame::model::XModel * Model::getXModel() {
    return &model_;
}

void Model::registerAgentFunction(std::string name,
        flame::exe::TaskFunction f_ptr) {
    model_.registerAgentFunction(name, f_ptr);
}

}}  // namespace flame::model
