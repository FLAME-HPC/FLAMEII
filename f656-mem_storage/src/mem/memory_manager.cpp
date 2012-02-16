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

void MemoryManager::RegisterAgent(std::string agent_name,
                                  size_t pop_size_hint) {
  AgentMemory a(agent_name, pop_size_hint);
  std::pair<AgentMap::iterator, bool> r;

  r = agent_map_.insert(std::pair<std::string, AgentMemory>(agent_name, a));
  if (!r.second) {  // no insertion. name exists
    throw std::invalid_argument("agent with that name already registered");
  }
}

AgentMemory& MemoryManager::GetAgent(std::string const& agent_name) {
  AgentMap::iterator it = agent_map_.find(agent_name);
  if (it == agent_map_.end()) {  // invalid agent name
    throw std::invalid_argument("Invalid agent name");
  }
  return it->second;
}

}}  // namespace flame::mem
