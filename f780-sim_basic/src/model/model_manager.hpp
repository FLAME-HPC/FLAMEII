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
#include <boost/graph/adjacency_list.hpp>
#include <boost/property_map/property_map.hpp>
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
#include "./xgraph.hpp"

namespace flame { namespace model {

class ModelManager {
  public:
    ModelManager() {}
    ~ModelManager();
    int loadModel(std::string const& file, XModel * model);
    int generate_task_list();
    std::vector<Task*> * get_task_list();

  private:
    int calculate_dependencies(std::vector<Task*> * tasks);
    int calculate_task_list(std::vector<Task*> * tasks);
    std::string taskTypeToString(Task::TaskType t);
    void printTaskList(std::vector<Task*> * tasks);
    XModel model_;
    std::vector<Task*> tasks_;
};

}}  // namespace flame::model
#endif  // MODEL__MODEL_MANAGER_HPP_
