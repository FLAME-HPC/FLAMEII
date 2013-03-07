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

void IOManager::registerIOPlugins() {
  addInputType("xml");
  addOutputType("xml");
}

void IOManager::loadModel(std::string const& file,
    flame::model::XModel * model) {
  // read model
  ioxmlmodel_.readXMLModel(file, model);
}

void IOManager::readPop(std::string const& file_name) {
  // check input type
  if (inputType_ == "xml") {
    ioxmlpop_.readPop(file_name, agentMemory_);
  } else {
    throw exc::flame_io_exception("unknown input type");
  }
}

void IOManager::writePop(
    std::string const& agent_name, std::string const& var_name) {
  // ToDo get var array here and pass to output plugin?

  // check output type
  if (inputType_ == "xml") {
    ioxmlpop_.writePop(agent_name, var_name);
  } else {
    throw exc::flame_io_exception("unknown output type");
  }
}

void IOManager::initialiseData() {
  // check output type
  if (inputType_ == "xml") {
    ioxmlpop_.initialiseData();
  } else {
    throw exc::flame_io_exception("unknown output type");
  }
}

void IOManager::finaliseData() {
  // check output type
  if (inputType_ == "xml") {
    ioxmlpop_.finaliseData();
  } else {
    throw exc::flame_io_exception("unknown output type");
  }
}

void IOManager::setIteration(size_t i) {
  iteration_ = i;
  ioxmlpop_.setIteration(i);
}

void IOManager::setAgentMemoryInfo(AgentMemory agentMemory) {
  agentMemory_ = agentMemory;
}

void IOManager::addInputType(std::string const& inputType) {
  // check if already added
  if (inputTypes_.find(inputType) != inputTypes_.end())
    throw exc::flame_io_exception("input type already added");

  inputTypes_.insert(inputType);
}

void IOManager::setInputType(std::string const& inputType) {
  inputType_ = inputType;
}

void IOManager::addOutputType(std::string const& outputType) {
  // check if already added
  if (outputTypes_.find(outputType) != outputTypes_.end())
    throw exc::flame_io_exception("input type already added");

  outputTypes_.insert(outputType);
}

void IOManager::setOutputType(std::string const& outputType) {
  outputType_ = outputType;
}

#ifdef TESTBUILD
void IOManager::Reset() {
  inputTypes_.clear();
  outputTypes_.clear();
}
#endif

}}  // namespace flame::io
