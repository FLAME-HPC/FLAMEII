/*!
 * \file flame2/sim/sim_manager.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief SimManager: simulation manager
 */
#ifndef SIM__SIM_MANAGER_HPP_
#define SIM__SIM_MANAGER_HPP_
#include <string>
#include "flame2/mb/message_board_manager.hpp"
#include "simulation.hpp"

namespace flame { namespace sim {

class SimManager {
  public:
    explicit SimManager();
    //! Register message type with the framework
    template <typename T>
    void registerMessageType(std::string name) {
      flame::mb::MessageBoardManager::GetInstance().RegisterMessage<T>(name);
    }

  private:
};

}}  // namespace flame::sim
#endif  // SIM__SIM_MANAGER_HPP_
