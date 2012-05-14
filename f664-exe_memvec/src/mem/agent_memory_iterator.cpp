/*!
 * \file src/mem/agent_memory_iterator.cpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Iterator to step through AgentMemory by agent (col-wise)
 */
#include "agent_memory_iterator.hpp"

namespace flame { namespace mem {

AgentMemoryIterator::AgentMemoryIterator(AgentMemory& am)
    : size_(0), position_(0), am_(am) {}

void AgentMemoryIterator::AllowAccess(const std::string& var_name,
                                      bool writeable) {
  if (position_ != 0) {
    throw flame::exceptions::logic_error("Cannot add vars to active iterator");
  }

  VectorWrapperBase* const vec_ptr = am_.GetVectorWrapper(var_name);
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
  }
  else {
    size_ = vec_ptr->size();
  }

  // assume ptr_map_ is only ever modified in tandem with vec_map, so
  // there's no need to use use .insert() and check for valid insertion
  ptr_map_[var_name] = vec_ptr->GetRawPtr();

  if (writeable) {
    rw_set_.insert(var_name);
  }
}

void AgentMemoryIterator::Rewind() {
  ConstVectorMap::const_iterator vec_it = vec_map_.begin();
  ConstVectorMap::const_iterator vec_end = vec_map_.end();
  VoidPtrMap::iterator ptr_it = ptr_map_.begin();

  for (; vec_it != vec_end; ++vec_it, ++ptr_it) {
#ifdef DEBUG
    if (vec_it->first != ptr_it->first) {
      throw flame::exceptions::flame_mem_exception("internal maps out of sync");
    }
    if (vec_it->second->size() != size_) {
      throw flame::exceptions::logic_error("vector sizes have changed");
    }
#endif
    ptr_it->second = vec_it->second->GetRawPtr(); // reset raw ptr to first elem
  }

  position_ = 0;
}

bool AgentMemoryIterator::AtEnd() const {
  return (position_ == size_);
}

bool AgentMemoryIterator::Step() {
  if (AtEnd()) { return false; }

  ConstVectorMap::const_iterator vec_it = vec_map_.begin();
  ConstVectorMap::const_iterator vec_end = vec_map_.end();
  VoidPtrMap::iterator ptr_it = ptr_map_.begin();

  for (; vec_it != vec_end; ++vec_it, ++ptr_it) {
#ifdef DEBUG
    if (vec_it->first != ptr_it->first) {
      throw flame::exceptions::flame_mem_exception("internal maps out of sync");
    }
    if (vec_it->second->size() != size_) {
      throw flame::exceptions::logic_error("vector sizes have changed");
    }
#endif
    // update ptr to point to next element
    ptr_it->second = vec_it->second->StepRawPtr(ptr_it->second);

#ifdef DEBUG
    if (ptr_it->second == NULL && position_ != (size_ - 1)) {
      throw flame::exceptions::logic_error("premature end of vector");
    }
#endif
  }

  ++position_;
  return true;
}

size_t AgentMemoryIterator::get_size() const {
  return size_;
}

size_t AgentMemoryIterator::get_position() const {
  return position_;
}
}}  //  namespace flame::mem
