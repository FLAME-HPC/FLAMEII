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
#include "flame2/model/model.hpp"

namespace flame { namespace sim {

namespace m = flame::model;

typedef std::pair<std::string, std::string> Var;
typedef std::map<std::string, std::vector<Var> > AgentMemory;

class Simulation {
  public:
    Simulation(const m::Model &model, std::string pop_file);
    void start(size_t iterations, size_t num_cores = 1);
#ifdef TESTBUILD
    Simulation() {}
    void registerModelWithMemoryManagerTest(const AgentMemory& agentMemory);
    void registerModelWithTaskManagerTest(const m::Model &model);
#endif

  private:
    void registerModelWithMessageManager(const m::Model &model);
    void registerModelWithMemoryManager(const AgentMemory& agentMemory);
    void registerModelWithTaskManager(const m::Model &model);
};
}}  // namespace flame::sim
#endif  // SIM__SIMULATION_HPP_
