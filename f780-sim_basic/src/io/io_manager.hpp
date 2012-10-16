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
#include "exceptions/io.hpp"

namespace flame { namespace io {

class IOManager {
  public:
    enum FileType { xml = 0 };

    static IOManager& GetInstance() {
          static IOManager instance;
          return instance;
        }

    void loadModel(std::string const& file, flame::model::XModel * model);
    void readPop(std::string file_name,
            model::XModel * model,
            FileType fileType);
    void writePop(std::string file_name,
            int iterationNo,
            model::XModel * model,
            FileType fileType);
    void writePop(std::string agent_name, std::string var_name);
    void setIteration(size_t i);

  private:
    //! This is a singleton class. Disable manual instantiation
    IOManager() : iteration_(0) {}
    //! This is a singleton class. Disable copy constructor
    IOManager(const IOManager&);
    //! This is a singleton class. Disable assignment operation
    void operator=(const IOManager&);

    xml::IOXMLModel ioxmlmodel;
    xml::IOXMLPop   ioxmlpop;
    size_t iteration_;
};
}}  // namespace flame::io
#endif  // IO__IO_MANAGER_HPP_
