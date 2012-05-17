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
    : size_(0), am_(am) {}

void AgentShadow::AllowAccess(const std::string& var_name,
                                      bool writeable) {
  VectorWrapperBase* const vec_ptr = am_->GetVectorWrapper(var_name);
  if (!vec_map_.empty() && vec_ptr->size() != size_) {
    throw flame::exceptions::logic_error("registered vars not of equal size");
  }
  if (vec_ptr->empty()) {
    throw flame::exceptions::logic_error("registered vars has no data");
  }

  std::pair<ConstVectorMap::iterator, bool> ret;
  ret = vec_map_.insert(ConstVectorMapValue(var_name, vec_ptr));
  if (!ret.second) {
    throw flame::exceptions::logic_error("variable already registered");
  } else {
    size_ = vec_ptr->size();
  }

  if (writeable) {
    rw_set_.insert(var_name);
  }
}

MemoryIteratorPtr AgentShadow::GetMemoryIterator() {
  return MemoryIteratorPtr(new MemoryIterator(this));
}

size_t AgentShadow::get_size() const {
  return size_;
}

}}  //  namespace flame::mem
