/*!
 * \file src/mem/agent_memory.cpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief AgentMemory: management and storage class for per-agent memory vectors
 */
#include <string>
#include "agent_memory.hpp"
#include "exceptions/mem.hpp"

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

//! Returns true if said memory variable has been registered.
bool AgentMemory::IsRegistered(std::string var_name) const {
  return (mem_map_.find(var_name) != mem_map_.end());
}

/*!
 * \brief Returns the current population size
 *
 * For now, we query the size from the first memory vector. In the future,
 * once we have a more structured approach to populating the vectors, we
 * should then simply return the counter value
 *
 * In debug more, we double check the size against all other memory vectors.
 * To speed up subsequent calls, we cache the return value and skip the
 * checks if the size remains the same.
 *
 * Throws flame::exceptions::flame_mem_exception when the sizes are inconsistent
 *
 * Not that these checks will not detect problems when other vectors apart from
 * the first reduces in size.
 *
 */
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
