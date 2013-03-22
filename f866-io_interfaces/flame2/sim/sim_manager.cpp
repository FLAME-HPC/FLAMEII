/*!
 * \file flame2/sim/sim_manager.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief SimManager: simulation manager
 */
#include <string>
#include "flame2/config.hpp"
#include "sim_manager.hpp"
#include "flame2/io/io_manager.hpp"

namespace flame { namespace sim {

SimManager::SimManager() {}

void SimManager::setPopInputType(std::string const& type) {
  // call io manager to set input type
  flame::io::IOManager::GetInstance().setInputType(type);
}

void SimManager::setPopOutputType(std::string const& type) {
  // call io manager to set output type
  flame::io::IOManager::GetInstance().setOutputType(type);
}

void SimManager::includeIOPluginDirectory(std::string const& dir) {
  // call io manager to include dir
  flame::io::IOManager::GetInstance().includeIOPluginDirectory(dir);
}

}}  // namespace flame::sim
