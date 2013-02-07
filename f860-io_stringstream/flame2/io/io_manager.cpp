/*!
 * \file flame2/io/io_manager.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief IOManager: management for I/O Backend
 */
#include <string>
#include "flame2/config.hpp"
#include "io_manager.hpp"

namespace flame { namespace io {

namespace exc = flame::exceptions;

void IOManager::loadModel(std::string const& file,
    flame::model::XModel * model) {
  ioxmlmodel.readXMLModel(file, model);
}

void IOManager::readPop(std::string const& file_name,
    AgentMemory AgentMemory, FileType fileType) {
  std::string xmlpopxsd;

  if (fileType == xml) {
    // set path to xml pop location
    ioxmlpop.setXmlPopPath(file_name);
    // validate data using a schema
    ioxmlpop.validateData(file_name, AgentMemory);
    // read validated pop xml
    ioxmlpop.readPop(file_name, AgentMemory);
  } else {
    throw exc::flame_io_exception("unknown file type");
  }
}

void IOManager::writePop(
    std::string const& agent_name, std::string const& var_name) {
  ioxmlpop.writePop(agent_name, var_name);
}

void IOManager::initialiseData() {
  ioxmlpop.initialiseData();
}

void IOManager::finaliseData() {
  ioxmlpop.finaliseData();
}

void IOManager::setIteration(size_t i) {
  iteration_ = i;
  ioxmlpop.setIteration(i);
}

}}  // namespace flame::io
