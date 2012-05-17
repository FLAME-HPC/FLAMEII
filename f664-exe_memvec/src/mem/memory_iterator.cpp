/*!
 * \file FILENAME
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief DESCRIPTION
 */
#include "boost/foreach.hpp"
#include "memory_iterator.hpp"

namespace flame { namespace mem {

MemoryIterator::MemoryIterator(AgentShadow* shadow) : position_(0) {
  vec_map_ptr_ = &(shadow->vec_map_);
  rw_set_ptr_ = &(shadow->rw_set_);
  size_ = shadow->get_size();
  BOOST_FOREACH(const ConstVectorMap::value_type& iter, *vec_map_ptr_) {
    ptr_map_[iter.first] = iter.second->GetRawPtr();
  }
}

void MemoryIterator::Rewind() {
  VoidPtrMap::iterator ptr_it = ptr_map_.begin();
  BOOST_FOREACH(const ConstVectorMap::value_type& iter, *vec_map_ptr_) {
#ifdef DEBUG
    if (iter.first != ptr_it->first) {
      throw flame::exceptions::flame_mem_exception("internal maps out of sync");
    }
    if (iter.second->size() != size_) {
      throw flame::exceptions::logic_error("vector sizes have changed");
    }
#endif
    ptr_it->second = iter.second->GetRawPtr();
    ++ptr_it;
  }
  position_ = 0;
}

bool MemoryIterator::AtEnd() const {
  return (position_ == size_);
}

bool MemoryIterator::Step() {
  if (AtEnd()) { return false; }
  VoidPtrMap::iterator ptr_it = ptr_map_.begin();
  BOOST_FOREACH(const ConstVectorMap::value_type& iter, *vec_map_ptr_) {
#ifdef DEBUG
    if (iter.first != ptr_it->first) {
      throw flame::exceptions::flame_mem_exception("internal maps out of sync");
    }
    if (iter.second->size() != size_) {
      throw flame::exceptions::logic_error("vector sizes have changed");
    }
#endif
    ptr_it->second = iter.second->StepRawPtr(ptr_it->second);
    ++ptr_it;
  }
  ++position_;
  return true;
}

size_t MemoryIterator::get_size() const {
  return size_;
}


size_t MemoryIterator::get_position() const {
  return position_;
}

}}  //  namespace flame::mem
