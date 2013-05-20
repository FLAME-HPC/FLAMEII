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
#include <vector>
#include <map>
#include <utility>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/variant.hpp>
#include <boost/function.hpp>
#include "flame2/config.hpp"
#include "flame2/mem/memory_manager.hpp"
#include "io_manager.hpp"
#include "plugins/io_xml_pop.hpp"
#include "plugins/io_csv_pop.hpp"
#ifdef HAVE_SQLITE3
#include "plugins/io_sqlite_pop.hpp"
#endif
#ifdef HAVE_HDF5
#include "plugins/io_hdf5_pop.hpp"
#endif
#include "flame2/build_config.hpp"  // required for install path

namespace flame { namespace io {

namespace exc = flame::exceptions;
namespace fs = boost::filesystem;

IOManager::IOManager() : iteration_(0), inputPlugin_(0), outputPlugin_(0) {
  // add plugins
  plugins_.insert(std::pair<std::string, IO*>("xml", new IOXMLPop));
  plugins_.insert(std::pair<std::string, IO*>("csv", new IOCSVPop));
#ifdef HAVE_SQLITE3
  plugins_.insert(std::pair<std::string, IO*>("sqlite", new IOSQLitePop));
#endif
#ifdef HAVE_HDF5
  plugins_.insert(std::pair<std::string, IO*>("hdf5", new IOHDF5Pop));
#endif
  // set default input and output options
  setInputType("xml");
  setOutputType("xml");
}

IOManager::~IOManager() {
  std::map<std::string, IO*>::iterator it;
  for (it = plugins_.begin(); it != plugins_.end(); ++it) {
    // delete plugin object
    delete it->second;
  }
}

void IOManager::loadModel(std::string const& file,
    flame::model::XModel * model) {
  // read model
  ioxmlmodel_.readXMLModel(file, model);
}

/*!
 * \brief Function to pass to IO plugins to add integers to an agent variable array
 * \param[in] agent_name Agent name
 * \param[in] var_name Variable name
 * \param[in] value Integer value
 */
void addInt(std::string const& agent_name,
    std::string const& var_name, int value) {
  // Add value to memory manager
  flame::mem::MemoryManager::GetInstance().
      GetVector<int>(agent_name, var_name)->push_back(value);
}

/*!
 * \brief Function to pass to IO plugins to add doubles to an agent variable array
 * \param[in] agent_name Agent name
 * \param[in] var_name Variable name
 * \param[in] value Double value
 */
void addDouble(std::string const& agent_name,
    std::string const& var_name, double value) {
  // Add value to memory manager
  flame::mem::MemoryManager::GetInstance().
      GetVector<double>(agent_name, var_name)->push_back(value);
}

size_t IOManager::readPop(std::string const& file_name) {
  // check input plugin has been set
  if (inputPlugin_ != NULL)
    inputPlugin_->readPop(file_name, addInt, addDouble);
  else
    throw exc::flame_io_exception("IO input type has not been set");

  // set the pop path to the directory of the opened file
  // this path is then used as the root directory to write pop files to
  boost::filesystem::path p(file_name);
  boost::filesystem::path dir = p.parent_path();
  path_ = dir.string();
  if (path_ != "")
    path_.append("/");

  // try and get the iteration number from the file name
  boost::filesystem::path file = p.stem();
  size_t i = 0;
  try {
    i = boost::lexical_cast<size_t>(file.string());
  } catch(const boost::bad_lexical_cast&) {
    printf("Warning: could not determine iteration number from input file name"
        ", using zero\n");
    i = 0;
  }
  // set the iteration number
  setIteration(i);

  // check output plugin has been set
  if (outputPlugin_ != NULL) {
    outputPlugin_->setPath(path_);
  } else {
    throw exc::flame_io_exception("IO output type has not been set");
  }

  // return iteration number to Simulation which sets the Scheduler
  return i;
}

void IOManager::writePop(
    std::string const& agent_name, std::string const& var_name) {
  // get vector for the agent variable
  mem::VectorWrapperBase* vw = mem::MemoryManager::GetInstance().
            GetVectorWrapper(agent_name, var_name);

  // check output plugin has been set
  if (outputPlugin_ != NULL)
      outputPlugin_->writePop(
          agent_name, var_name, vw->size(), vw->GetRawPtr());
  else
    throw exc::flame_io_exception("IO output type has not been set");
}

void IOManager::initialiseData() {
  // check output plugin has been set
  if (outputPlugin_ != NULL)
      outputPlugin_->initialiseData();
  else
    throw exc::flame_io_exception("IO output type has not been set");
}

void IOManager::finaliseData() {
  // check output plugin has been set
  if (outputPlugin_ != NULL)
      outputPlugin_->finaliseData();
  else
    throw exc::flame_io_exception("IO output type has not been set");
}

void IOManager::setIteration(size_t i) {
  iteration_ = i;

  // check output plugin has been set
  if (outputPlugin_ != NULL)
      outputPlugin_->setIteration(i);
  else
    throw exc::flame_io_exception("IO output type has not been set");
}

void IOManager::setAgentMemoryInfo(AgentMemory agentMemory) {
  agentMemory_ = agentMemory;

  // check input plugin has been set
  if (inputPlugin_ != NULL)
      inputPlugin_->setAgentMemoryInfo(agentMemory_);
  else
    throw exc::flame_io_exception("IO input type has not been set");

  // check output plugin has been set
  if (outputPlugin_ != NULL)
      outputPlugin_->setAgentMemoryInfo(agentMemory_);
  else
    throw exc::flame_io_exception("IO output type has not been set");
}

void IOManager::setInputType(std::string const& inputType) {
  std::map<std::string, IO*>::iterator pit;

  // find input type plugin
  pit = plugins_.find(inputType);
  if (pit != plugins_.end())
    inputPlugin_ = pit->second;
  else
    throw exc::flame_io_exception(
        "IO plugin not available for input type: " + inputType);
}

void IOManager::setOutputType(std::string const& outputType) {
  std::map<std::string, IO*>::iterator pit;

  // find output type plugin
  pit = plugins_.find(outputType);
  if (pit != plugins_.end()) {
    outputPlugin_ = pit->second;
    outputPlugin_->setPath(path_);
    outputPlugin_->setAgentMemoryInfo(agentMemory_);
  } else {
    throw exc::flame_io_exception(
        "IO plugin not available for output type: " + outputType);
  }
}

#ifdef TESTBUILD
IO * IOManager::getIOPlugin(std::string const& name) {
  std::map<std::string, IO*>::iterator pit;
  // find plugin
  pit = plugins_.find(name);
  if (pit != plugins_.end()) return pit->second;
  else
    throw exc::flame_io_exception(
        "IO plugin not available for type: " + name);
}

void IOManager::Reset() {
  // Set default input and output options
  setInputType("xml");
  setOutputType("xml");
}
#endif

}}  // namespace flame::io
