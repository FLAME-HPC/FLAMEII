/*!
 * \file flame2/io/io_manager.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief IOManager: management for I/O Backend
 */
#include <dlfcn.h>  // for dynamic linking loader
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

namespace flame { namespace io {

namespace exc = flame::exceptions;
namespace fs = boost::filesystem;

typedef boost::function<IO*()> pluginConstructor;

IOManager::IOManager() : iteration_(0), inputPlugin_(0), outputPlugin_(0) {
  std::vector<std::string>::iterator it;
  fs::directory_iterator end_iter;
  std::vector<std::string> plugins;

  // Find all .plugin files in the specified plugins directory
  fs::path pluginsDir("/Users/stc/workspace/f866-io_interfaces"
      "/flame2/io/plugins");
  if (fs::exists(pluginsDir) && fs::is_directory(pluginsDir)) {
    for (fs::directory_iterator dir_iter(pluginsDir);
        dir_iter != end_iter ; ++dir_iter) {
      if (fs::is_regular_file(dir_iter->status())) {
        if (dir_iter->path().extension() == ".plugin")
          plugins.push_back(dir_iter->path().string());
      }
    }
  }

  // iterate through all the plugins and call getIOPlugin and use an instance
  for (it = plugins.begin(); it != plugins.end(); ++it)
    loadIOPlugin(*it);

#ifndef TESTBUILD
/*  std::map<std::string, Plugin>::iterator pit;
  printf("IO Backends available: ");
  for (pit = plugins_.begin(); pit != plugins_.end();) {
    printf("%s", pit->second.first->getName().c_str());
    ++pit;
    if (pit != plugins_.end()) printf(" ");
  }
  printf("\n");*/
#endif

  // Set default input and output options
  setInputType("xml");
  setOutputType("xml");
}

void IOManager::loadIOPlugin(std::string const& path) {
  // load the plugin's shared object file
  void *handle = NULL;
  if (!(handle = dlopen(path.c_str(), RTLD_LAZY))) {
    printf("Failed loading plugin %s: %s\n", path.c_str(), dlerror());
  } else {
    // Get the pluginConstructor function ToDo fix compiler warning
    pluginConstructor construct = (IO* (*)(void)) dlsym(handle, "getIOPlugin");
    // if returns NULL then address of function could not be found
    if (dlerror()) {
      printf("Failed constructing plugin %s: "
          "Could not find 'getIOPlugin' function\n", path.c_str());
      dlclose(handle);
    } else {
      // construct a plugin
      IO *plugin = construct();
      // add plugin to set of plugins
      std::string pluginName = plugin->getName();
      std::map<std::string, Plugin>::iterator pit;
      pit = plugins_.find(pluginName);
      if (pit == plugins_.end()) {
        plugins_.insert(std::pair<std::string, Plugin>
          (pluginName, std::pair<IO*, void*>(plugin, handle)));
      } else {
        printf("Failed registering plugin %s: Plugin %s already exists\n",
            path.c_str(), pluginName.c_str());
        delete plugin;
        dlclose(handle);
      }
    }
  }
}

IOManager::~IOManager() {
  std::map<std::string, Plugin>::iterator it;
  for (it = plugins_.begin(); it != plugins_.end(); ++it) {
    // delete plugin object
    delete it->second.first;
    // close the handle and unload the dynamic library
    dlclose(it->second.second);
  }
}

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
  std::map<std::string, Plugin>::iterator pit;

  // find input type plugin
  pit = plugins_.find(inputType);
  if (pit != plugins_.end())
    inputPlugin_ = pit->second.first;
  else
    throw exc::flame_io_exception(
        "IO plugin not available for input type: " + inputType);
}

void IOManager::setOutputType(std::string const& outputType) {
  std::map<std::string, Plugin>::iterator pit;

  // find output type plugin
  pit = plugins_.find(outputType);
  if (pit != plugins_.end()) {
    outputPlugin_ = pit->second.first;
    outputPlugin_->setPath(path_);
    outputPlugin_->setAgentMemoryInfo(agentMemory_);
  } else {
    throw exc::flame_io_exception(
        "IO plugin not available for output type: " + outputType);
  }
}

#ifdef TESTBUILD
IO * IOManager::getIOPlugin(std::string const& name) {
  std::map<std::string, Plugin>::iterator pit;
  // find plugin
  pit = plugins_.find(name);
  if (pit != plugins_.end()) return pit->second.first;
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
