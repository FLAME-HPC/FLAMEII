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
#include "model/model_manager.hpp"
#include "mb/message_board_manager.hpp"

#include "mem/memory_manager.hpp"
#include "mb/message_board_manager.hpp"
#include "exe/task_manager.hpp"
#include "exe/scheduler.hpp"
#include "exe/fifo_task_queue.hpp"
#include "include/flame.h"

#include "mb/client.hpp"
#include "mb/message.hpp"
#include "mb/message_iterator.hpp"

namespace flame {
namespace model {

Model::Model(std::string path_to_model)
    : modelLoaded_(false) {
    flame::model::ModelManager modelManager;
    int rc = 0;

    rc += modelManager.loadModel(path_to_model, &model_);
    if (rc == 0) {
        modelLoaded_ = true;
    } else {
        std::fprintf(stderr, "Error: Cannot load model\n");
    }
}

Model::~Model() {

}

flame::model::XModel * Model::getXModel() {
    return &model_;
}

int Model::registerAgentFunction(std::string name, flame::exe::TaskFunction f_ptr) {
    return model_.registerAgentFunction(name, f_ptr);
}

}}  // namespace flame::model
