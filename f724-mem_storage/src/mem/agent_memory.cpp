/*!
 * \file src/mem/agent_memory.cpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief AgentMemory: management and storage class for per-agent memory vectors
 */
#include "agent_memory.hpp"
namespace flame { namespace mem {

void AgentMemory::HintPopulationSize(unsigned int size_hint) {
  if (mem_map_.size() == 0) {
    throw std::runtime_error("no agent memory variables registered");
  }

  // iterate through all vectors and reserve size based on hint
  MemoryMap::iterator it;
  for (it = mem_map_.begin(); it != mem_map_.end(); ++it ) {
    it->second->reserve(size_hint);
  }
  registration_closed_ = true;  // no more new variables
}


}}  // namespace flame::mem
