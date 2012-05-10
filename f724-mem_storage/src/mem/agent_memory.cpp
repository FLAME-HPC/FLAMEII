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
#include "src/exceptions/mem.hpp"

namespace flame { namespace mem {

namespace exc = flame::exceptions;

void AgentMemory::HintPopulationSize(unsigned int size_hint) {
  if (mem_map_.size() == 0) {
    throw exc::invalid_agent("no agent memory variables registered");
  }

  // iterate through all vectors and reserve size based on hint
  MemoryMap::iterator it;
  for (it = mem_map_.begin(); it != mem_map_.end(); ++it) {
    it->second->reserve(size_hint);
  }
  registration_closed_ = true;  // no more new variables
}

VectorWrapperBase* AgentMemory::GetVectorWrapper(std::string var_name) {
  try {
    return &(mem_map_.at(var_name));
  }
  catch(const boost::bad_ptr_container_operation& E) {
    throw exc::invalid_variable("Invalid agent memory variable");
  }
}

}}  // namespace flame::mem
