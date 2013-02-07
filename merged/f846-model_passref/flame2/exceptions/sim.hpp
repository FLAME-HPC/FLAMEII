/*!
 * \file flame2/exceptions/sim.hpp
 * \author Simon Coakley
 * \date 2013
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Exceptions thrown by sim classes
 */
#ifndef EXCEPTIONS__SIM_HPP_
#define EXCEPTIONS__SIM_HPP_
#include <string>
#include "base.hpp"

namespace flame { namespace exceptions {

class flame_sim_exception : public flame_exception {
  public:
    explicit flame_sim_exception(const std::string& msg)
        : flame_exception(msg) {}
};

}}  // namespace flame::exceptions
#endif  // EXCEPTIONS__SIM_HPP_
