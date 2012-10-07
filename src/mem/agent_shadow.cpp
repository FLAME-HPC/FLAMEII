/*!
 * \file src/mem/agent_shadow.cpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Proxy object which only exposes selected vars of an agent
 */
#include <utility>
#include <string>
#include "vector_wrapper.hpp"
#include "memory_iterator.hpp"
#include "agent_memory.hpp"

namespace flame { namespace mem {

AgentShadow::AgentShadow(AgentMemory* am)
    : am_(am) {}

void AgentShadow::AllowAccess(const std::string& var_name,
                                      bool writeable) {
  VectorWrapperBase* const vec_ptr = am_->GetVectorWrapper(var_name);

  std::pair<ConstVectorMap::iterator, bool> ret;
  ret = vec_map_.insert(ConstVectorMap::value_type(var_name, vec_ptr));
  if (!ret.second) {
    throw flame::exceptions::logic_error("variable already registered");
  }

  if (writeable) {
    rw_set_.insert(var_name);
  }
}

MemoryIteratorPtr AgentShadow::GetMemoryIterator() {
  return MemoryIteratorPtr(new MemoryIterator(this));
}

MemoryIteratorPtr AgentShadow::GetMemoryIterator(size_t offset, size_t count) {
  return MemoryIteratorPtr(new MemoryIterator(this, offset, count));
}

size_t AgentShadow::get_size() {
  return am_->GetPopulationSize();
}

}}  //  namespace flame::mem
