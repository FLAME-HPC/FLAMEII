/*!
 * \file src/model/model.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Model: model object for users to use
 */

#ifndef MODEL__MODEL_HPP_
#define MODEL__MODEL_HPP_
#include "model/xmodel.hpp"

namespace flame {
namespace model {

class Model {
  public:
    Model(std::string model);
    ~Model();
    int registerAgentFunction(std::string name, flame::exe::TaskFunction f_ptr);
    template <typename T>
    int registerMessageType(std::string name);
    flame::model::XModel * getXModel();
  private:
    flame::model::XModel model_;
    bool modelLoaded_;
};

}}  // namespace flame::model
#endif  // MODEL__MODEL_HPP_
