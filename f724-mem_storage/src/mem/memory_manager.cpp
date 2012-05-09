/*!
 * \file src/mem/memory_manager.cpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief MemoryManager: management and storage class for agent data
 */
#include "memory_manager.hpp"
#include <utility>
#include <string>

namespace flame { namespace mem {

//! Key-Value pair for AgentMemory
typedef std::pair<std::string, AgentMemory> AgentMapValue;

void MemoryManager::RegisterAgent(std::string agent_name) {
  std::pair<AgentMap::iterator, bool> ret;
  ret = agent_map_.insert(AgentMapValue(agent_name, AgentMemory(agent_name)));
  if (!ret.second) { // if replacement instead of insertion
    throw std::invalid_argument("agent already registered");
  }
}

void MemoryManager::HintPopulationSize(std::string agent_name,
                                       unsigned int size_hint) {
  GetAgentMemory(agent_name).HintPopulationSize(size_hint);
}

AgentMemory& MemoryManager::GetAgentMemory(std::string agent_name) {
  AgentMap::iterator it = agent_map_.find(agent_name);
  if (it == agent_map_.end()) {  // unknown agent name
    throw std::invalid_argument("unknown agent name");
  }
  return it->second;
}

}}  // namespace flame::mem
