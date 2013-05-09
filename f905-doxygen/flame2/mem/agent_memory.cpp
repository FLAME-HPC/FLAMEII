/*!
 * \file flame2/mem/agent_memory.cpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief AgentMemory: management and storage class for per-agent memory vectors
 */
#include <string>
#include "flame2/config.hpp"
#include "flame2/exceptions/mem.hpp"
#include "agent_memory.hpp"

namespace flame { namespace mem {

namespace exc = flame::exceptions;

void AgentMemory::HintPopulationSize(unsigned int size_hint) {
  if (mem_map_.size() == 0) {
    throw exc::invalid_agent("no agent memory variables registered");
  }
  registration_closed_ = true;  // no more new variables

  // iterate through all vectors and reserve size based on hint
  MemoryMap::iterator it;
  for (it = mem_map_.begin(); it != mem_map_.end(); ++it) {
    it->second->reserve(size_hint);
  }
}

VectorWrapperBase* AgentMemory::GetVectorWrapper(const std::string& var_name) {
  registration_closed_ = true;  // no more new variables
  try {
    return &(mem_map_.at(var_name));
  }
  catch(const boost::bad_ptr_container_operation& E) {
    throw exc::invalid_variable("Invalid agent memory variable");
  }
}

bool AgentMemory::IsRegistered(const std::string& var_name) const {
  return (mem_map_.find(var_name) != mem_map_.end());
}

size_t AgentMemory::GetPopulationSize(void) {
  MemoryMap::iterator iter = mem_map_.begin();
  if (iter == mem_map_.end()) {  // no memory vars
#ifdef DEBUG
    cached_size_ = 0;
#endif
    return 0;
  } else {
    size_t size = iter->second->size();
#ifdef DEBUG
    if (size != cached_size_) {
      for (++iter; iter != mem_map_.end(); ++iter) {
        if (iter->second->size() != size) {
          throw exc::flame_mem_exception("inconsistent vector sizes");
        }
      }
      cached_size_ = size;
    }
#endif
    return size;
  }
}
}}  // namespace flame::mem
