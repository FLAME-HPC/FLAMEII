/*!
 * \file FILENAME
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief DESCRIPTION
 */
#include <iostream>
#include "exceptions/all.hpp"
#include "message.hpp"
#include "message_iterator_backend_raw.hpp"

namespace flame { namespace mb {

MessageIteratorBackendRaw::MessageIteratorBackendRaw(MemoryMap* vec_map_ptr,
                                                     TypeValidator *tv)
    : MessageIteratorBackend(vec_map_ptr, tv) {  // call parent constructor
  Rewind(); // use Rewind to initialise raw_map_

}

void MessageIteratorBackendRaw::Rewind(void) {
  MemoryMap::iterator iter = vec_map_->begin();
  MemoryMap::iterator end = vec_map_->end();

  for (; iter != end; ++iter) {
#ifdef DEBUG
    if (count_ != iter->second->size()) {
      throw flame::exceptions::flame_exception("Inconsistent vector sizes");
    }
#endif
    raw_map_[iter->first] = iter->second->GetRawPtr();
  }

  position_ = 0;
}

bool MessageIteratorBackendRaw::AtEnd(void) const {
  return (position_ == count_);
}

size_t MessageIteratorBackendRaw::GetCount(void) const {
  return count_;
}

bool MessageIteratorBackendRaw::Step(void) {
  if (AtEnd()) { return false; }

  // Progress each raw ptr to the next element
  RawPtrMap::iterator ptr_iter = raw_map_.begin();
  MemoryMap::iterator vec_iter = vec_map_->begin();
  for (; vec_iter != vec_map_->end(); ++vec_iter, ++ptr_iter) {
#ifdef DEBUG
    if (vec_iter->first != ptr_iter->first) {
      throw flame::exceptions::flame_exception("internal maps out of sync");
    }
#endif
    ptr_iter->second = vec_iter->second->StepRawPtr(ptr_iter->second);
  }

  ++position_; // increment position counter
  return (AtEnd() ? false: true);  // success
}

void* MessageIteratorBackendRaw::Get(std::string var_name) {
  if (AtEnd()) {
    throw flame::exceptions::out_of_range("Nothing to retrieve");
  }
  return raw_map_.at(var_name);
}

}}  // namespace flame::mb
