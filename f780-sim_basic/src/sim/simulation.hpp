/*!
 * \file src/sim/simulation.hpp
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
#include "model/model_manager.hpp"
#include "model/model.hpp"

namespace flame { namespace sim {

class Simulation {
  public:
    Simulation(flame::model::Model * model, std::string pop_file);
    void start(size_t iterations);

  private:
    flame::model::XModel * model_;
    bool modelLoaded_;
    bool popLoaded_;
};
}}  // namespace flame::sim
#endif  // SIM__SIMULATION_HPP_
