/*!
 * \file flame2/mb/message_iterator_backend_raw.cpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief DESCRIPTION
 */
#include <string>
#include "flame2/exceptions/all.hpp"
#include "message.hpp"
#include "message_iterator_backend_raw.hpp"

namespace flame { namespace mb {

/*!
 * \brief Constructor
 *
 * Calls parent constructor then calls Rewind() to initialise raw_map_.
 */
MessageIteratorBackendRaw::MessageIteratorBackendRaw(MemoryMap* vec_map_ptr,
                                                     TypeValidator *tv)
    : MessageIteratorBackend(vec_map_ptr, tv) {  // call parent constructor
  Rewind();  // use Rewind to initialise raw_map_
}

/*!
 * \brief Rewinds the iterator
 *
 * position_ is reset to 0 and raw_map_ is initialised with to the address of
 * the first element in each message vector.
 * 
 * When DEBUG is defined, we also validate the sizes of all message vectors
 * while we iterate through them. flame::exception:flame_exception is thrown
 * if the sizes are inconsistent.
 */
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

/*!
 * \brief Indicates end of iteration
 * \return true if iteration has ended, false otherwise
 *
 * Returns true when position_ equals the number of messages (count_).
 */
bool MessageIteratorBackendRaw::AtEnd(void) const {
  return (position_ == count_);
}

//! Returns count_
size_t MessageIteratorBackendRaw::GetCount(void) const {
  return count_;
}

/*!
 * \brief Step through to the next message in the iteration
 * \return false if end of iteration, true otherwise
 *
 * Iterates through the list of raw pointers, and using
 * VectorWrapperBase::StepRawPtr() for the associated vector, perform the
 * appropriated pointer increment so it points to the next message in the
 * array within the vector.
 *
 * position_ is incremented.
 *
 * When DEBUG is defined, we also check that our maps are in sync. As we're
 * iterationd through them concurrently we expect the keys to be returned in
 * the same order (avoid map lookups for each variable). The maps may go out
 * of sync if the map is modified along the way, or if an unordered map
 * container is used.
 */
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

  ++position_;  // increment position counter
  return (AtEnd() ? false: true);  // success
}

/*!
 * \brief Internal routine to return raw pointer for a the curren message var
 * \return raw pointer to actual message variable or Null if end of iteration
 *
 * Throws flame::exceptions::out_of_range if called after the iteration has
 * ended.
 *
 * If an invalid var_name is provided, the container (std::map) may throw
 * a std::out_of_range exception.
 */
void* MessageIteratorBackendRaw::Get(std::string var_name) {
  return AtEnd() ? NULL : raw_map_.at(var_name);
}

}}  // namespace flame::mb
