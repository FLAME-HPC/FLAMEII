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
  // read model
  ioxmlmodel_.readXMLModel(file, model);
}

void IOManager::readPop(std::string const& file_name, FileType fileType) {
  // if file type is xml
  if (fileType == xml) {
    // set path to xml pop location
    ioxmlpop_.setXmlPopPath(file_name);
    // validate data using a schema
    ioxmlpop_.validateData(file_name, agentMemory_);
    // read validated pop xml
    ioxmlpop_.readPop(file_name, agentMemory_);
  } else {
    throw exc::flame_io_exception("unknown file type");
  }
}

void IOManager::writePop(
    std::string const& agent_name, std::string const& var_name) {
  // write vector of agent variables
  ioxmlpop_.writePop(agent_name, var_name);
}

void IOManager::initialiseData() {
  ioxmlpop_.initialiseData();
}

void IOManager::finaliseData() {
  ioxmlpop_.finaliseData();
}

void IOManager::setIteration(size_t i) {
  iteration_ = i;
  ioxmlpop_.setIteration(i);
}

void IOManager::setAgentMemoryInfo(AgentMemory agentMemory) {
  agentMemory_ = agentMemory;
}

}}  // namespace flame::io
