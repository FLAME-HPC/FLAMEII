/*!
 * \file flame2/model/model.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Model: model object for users to use
 */

#ifndef MODEL__MODEL_HPP_
#define MODEL__MODEL_HPP_
#include <string>
#include "flame2/model/xmodel.hpp"
#include "flame2/compat/C/compatibility_manager.hpp"
#include "flame2/compat/C/flame2.h"

namespace flame {
namespace model {

class Model {
  public:
    explicit Model(std::string model);
    void registerAgentFunction(std::string name,
            flame::exe::TaskFunction f_ptr);
    template <typename T>
    void registerMessageType(std::string name) {
        flame::compat::c::CompatibilityManager::GetInstance().
                RegisterMessage<T>(name);
    }
    flame::model::XModel * getXModel();
  private:
    flame::model::XModel model_;
};

}}  // namespace flame::model
#endif  // MODEL__MODEL_HPP_
