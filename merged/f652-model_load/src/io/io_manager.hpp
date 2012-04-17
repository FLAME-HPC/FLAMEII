/*!
 * \file src/io/io_manager.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief IOManager: management for I/O Backend
 */
#ifndef IO__IO_MANAGER_HPP_
#define IO__IO_MANAGER_HPP_
#include <string>
#include <vector>
#include "../model/model_manager.hpp"

namespace flame { namespace io {

class IOManager {
  public:
    enum FileType { xml = 0 };
    IOManager() {}
    int loadModel(std::string const& file, flame::model::XModel * model);

  private:
};
}}  // namespace flame::io
#endif  // IO__IO_MANAGER_HPP_
