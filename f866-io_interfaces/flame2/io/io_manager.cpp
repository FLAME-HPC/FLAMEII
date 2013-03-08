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
#include "flame2/mem/memory_manager.hpp"
#include "io_manager.hpp"

namespace flame { namespace io {

namespace exc = flame::exceptions;

void IOManager::loadModel(std::string const& file,
    flame::model::XModel * model) {
  // read model
  ioxmlmodel_.readXMLModel(file, model);
}

void addInt(std::string const& agent_name,
    std::string const& var_name, int value) {
  // Add value to memory manager
  flame::mem::MemoryManager::GetInstance().
      GetVector<int>(agent_name, var_name)->push_back(value);
}

void addDouble(std::string const& agent_name,
    std::string const& var_name, double value) {
  // Add value to memory manager
  flame::mem::MemoryManager::GetInstance().
      GetVector<double>(agent_name, var_name)->push_back(value);
}

void IOManager::readPop(std::string const& file_name) {
  void (*paddInt)(std::string const&, std::string const&, int) = addInt;
  void (*paddDouble)(std::string const&, std::string const&, double) =
      addDouble;

  // check input type
  if (inputType_ == "xml") {
    ioxmlpop_.readPop(file_name, paddInt, paddDouble);
  } else {
    throw exc::flame_io_exception("unknown input type");
  }
}

void IOManager::writePop(
    std::string const& agent_name, std::string const& var_name) {
  // get vector for the agent variable
  mem::VectorWrapperBase* vw = mem::MemoryManager::GetInstance().
            GetVectorWrapper(agent_name, var_name);

  // check output type
  if (inputType_ == "xml") {
    ioxmlpop_.writePop(agent_name, var_name, vw->size(), vw->GetRawPtr());
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

  // set plugins too
  ioxmlpop_.setAgentMemoryInfo(agentMemory);
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
