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

/*!
 * \brief Class to represent the simulation manager
 */
class SimManager {
  public:
    /*!
     * \brief Constructor
     */
    explicit SimManager();
    /*!
     * \brief Register message type with the framework
     * \param[in] name The message data type name
     */
    template <typename T>
    void registerMessageType(std::string name) {
      flame::mb::MessageBoardManager::GetInstance().RegisterMessage<T>(name);
    }
    /*!
     * \brief Set the population input IO type
     * \param[in] type The type name
     */
    void setPopInputType(std::string const& type);
    /*!
     * \brief Set the population output IO type
     * \param[in] type The type name
     */
    void setPopOutputType(std::string const& type);

  private:
};

}}  // namespace flame::sim
#endif  // SIM__SIM_MANAGER_HPP_
