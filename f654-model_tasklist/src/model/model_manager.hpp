/*!
 * \file src/model/model_manager.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief ModelManager: management and storage class for model data
 */
#ifndef MODEL__MODEL_MANAGER_HPP_
#define MODEL__MODEL_MANAGER_HPP_
#include <string>
#include <vector>
#include "./xadt.hpp"
#include "./xcondition.hpp"
#include "./xfunction.hpp"
#include "./xioput.hpp"
#include "./xmachine.hpp"
#include "./xmessage.hpp"
#include "./xmodel.hpp"
#include "./xtimeunit.hpp"
#include "./xvariable.hpp"
#include "./task.hpp"

namespace flame { namespace model {

class ModelManager {
  public:
    ModelManager() {}
    ~ModelManager();
    int loadModel(std::string const& file);
    int generate_task_list();
    std::vector<Task*> * get_task_list();

  private:
    XModel model_;
    std::vector<Task*> tasks_;
};

}}  // namespace flame::model
#endif  // MODEL__MODEL_MANAGER_HPP_
