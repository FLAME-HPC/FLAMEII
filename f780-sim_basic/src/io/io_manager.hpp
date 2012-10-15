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
#include "./io_xml_model.hpp"
#include "./io_xml_pop.hpp"
#include "model/xmodel.hpp"

namespace flame { namespace io {

class IOManager {
  public:
    enum FileType { xml = 0 };
    IOManager() {}
    int loadModel(std::string const& file, flame::model::XModel * model);
    int readPop(std::string file_name,
            model::XModel * model,
            FileType fileType);
    int writePop(std::string file_name,
            int iterationNo,
            model::XModel * model,
            FileType fileType);

  private:
    xml::IOXMLModel ioxmlmodel;
    xml::IOXMLPop   ioxmlpop;
};
}}  // namespace flame::io
#endif  // IO__IO_MANAGER_HPP_
