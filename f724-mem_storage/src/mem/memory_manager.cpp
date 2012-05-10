/*!
 * \file src/mem/memory_manager.cpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief MemoryManager: management and storage class for agent data
 */
#include <utility>
#include <string>
#include "memory_manager.hpp"
#include "src/exceptions/mem.hpp"

namespace flame { namespace mem {

namespace exc = flame::exceptions;

//! Key-Value pair for AgentMemory
typedef std::pair<std::string, AgentMemory> AgentMapValue;

void MemoryManager::RegisterAgent(std::string agent_name) {
  std::pair<AgentMap::iterator, bool> ret;
  ret = agent_map_.insert(AgentMapValue(agent_name, AgentMemory(agent_name)));
  if (!ret.second) { // if replacement instead of insertion
    throw exc::logic_error("agent already registered");
  }
}

void MemoryManager::HintPopulationSize(std::string agent_name,
                                       unsigned int size_hint) {
  GetAgentMemory(agent_name).HintPopulationSize(size_hint);
}

AgentMemory& MemoryManager::GetAgentMemory(std::string agent_name) {
  try {
    return agent_map_.at(agent_name);
  }
  catch (std::out_of_range &E) {
    throw exc::invalid_agent("unknown agent name");
  }
}

}}  // namespace flame::mem
