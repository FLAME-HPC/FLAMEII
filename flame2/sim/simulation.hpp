/*!
 * \file flame2/sim/simulation.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Simulation: holds and manages simulation information
 */
#ifndef SIM__SIMULATION_HPP_
#define SIM__SIMULATION_HPP_
#include <string>
#include <map>
#include <set>
#include <utility>
#include <vector>
#include "flame2/model/model.hpp"

namespace flame { namespace sim {

namespace m = flame::model;

//! \brief Type to hold a variable with type and name
typedef std::pair<std::string, std::string> Var;
//! \brief Type to hold agent memory info with agent name and list of variables
typedef std::map<std::string, std::vector<Var> > AgentMemory;
//! \brief Type to hold a set of strings
typedef std::set<std::string> StringSet;

/*!
 * \brief Class to represent a simulation
 */
class Simulation {
  public:
    /*!
     * \brief Constructor
     * \param[in] model The model
     * \param[in] pop_file The population file
     */
    Simulation(const m::Model &model, std::string pop_file);
    /*!
     * \brief Start the simulation
     * \param[in] iterations The number of iterations to run for
     * \param[in] num_cores The number of cores
     */
    void start(size_t iterations, size_t num_cores = 1);
#ifdef TESTBUILD
    /*!
     * \brief Test constructor
     */
    Simulation() : iteration_(1) {}
    /*!
     * \brief Test register model with memory manager
     * \param[in] agentMemory Agent memory info
     */
    void registerModelWithMemoryManagerTest(const AgentMemory& agentMemory);
    /*!
     * \brief Test register model with task manager
     * \param[in] model The model to register
     */
    void registerModelWithTaskManagerTest(const m::Model &model);
#endif

  private:
    //! \brief Current iteration number
    size_t iteration_;
    /*!
     * \brief Register the model with the memory manager
     * \param[in] agentMemory Agent memory info
     */
    void registerModelWithMemoryManager(const AgentMemory& agentMemory);
    /*!
     * \brief Register the model with the task manager
     * \param[in] model The model to register
     */
    void registerModelWithTaskManager(const m::Model &model);
    /*!
     * \brief Register agent task with task manager
     * \param[in] task The agent task
     * \param[in] model The model
     */
    void registerAgentTaskWithTaskManager(
        m::ModelTask * task, const m::Model &model);
    /*!
     * \brief Register message board task with task manager
     * \param[in] task The message board task
     * \param[in] taskType The message board task type
     */
    void registerMBTaskWithTaskManager(m::ModelTask * task, int taskType);
    /*!
     * \brief Allow access to messages for agent task
     * \param[in] task Agent task
     * \param[in] messages Set of messages to allow access to
     * \param[in] post True for post, false for read
     */
    void registerAllowMessage(exe::Task * task, StringSet messages, bool post);
    /*!
     * \brief Allow access to memory for agent task
     * \param[in] task Agent task
     * \param[in] vars Set of variables
     * \param[in] writeable True for read write, false for read only
     */
    void registerAllowAccess(exe::Task * task, StringSet vars, bool writeable);
};
}}  // namespace flame::sim
#endif  // SIM__SIMULATION_HPP_
