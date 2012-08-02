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
#include <boost/graph/adjacency_list.hpp>
#include <boost/property_map/property_map.hpp>
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

// Define graph type
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS> Graph;
// Define vertex and edge descriptor types
typedef boost::graph_traits<Graph>::vertex_descriptor vertex_descriptor;
typedef boost::graph_traits<Graph>::edge_descriptor edge_descriptor;

class ModelManager {
  public:
    ModelManager() {}
    ~ModelManager();
    int loadModel(std::string const& file);
    int generate_task_list();
    std::vector<Task*> * get_task_list();

  private:
    int catalog_state_dependencies_functions(
            XModel * model, std::vector<Task*> * tasks);
    int catalog_state_dependencies_transitions(XModel * model);
    int catalog_state_dependencies(XModel * model, std::vector<Task*> * tasks);
    int catalog_communication_dependencies_syncs(
            XModel * model,
            std::vector<Task*> * tasks);
    int catalog_communication_dependencies_ioput(XModel * model,
            std::vector<XFunction*>::iterator function);
    int catalog_communication_dependencies(XModel * model,
            std::vector<Task*> * tasks);
    int catalog_data_dependencies_variable(
            std::vector<XMachine*>::iterator agent,
            std::vector<XVariable*>::iterator variable,
            std::vector<Task*> * tasks);
    int catalog_data_dependencies(XModel * model,
            std::vector<Task*> * tasks);
    int check_dependency_loops(XModel * model);
    int calculate_dependencies(std::vector<Task*> * tasks);
    int calculate_task_list(std::vector<Task*> * tasks);
    void write_dependency_graph_dependencies(
            std::vector<Task*>::iterator task, FILE *file);
    void write_dependency_graph(std::string filename,
            std::vector<Task*> * tasks);
    std::string taskTypeToString(Task::TaskType t);
    void printTaskList(std::vector<Task*> * tasks);
    XModel model_;
    std::vector<Task*> tasks_;
    // New boost graph definitions
    Graph graph_;
};

}}  // namespace flame::model
#endif  // MODEL__MODEL_MANAGER_HPP_
