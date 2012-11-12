/*!
 * \file flame2/mb2/message_iterator_backend_raw.cpp
 * \author Shawn Chin
 * \date November 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Implementation of an Iterator Backend that references the original
 * message data and steps through using pointer arithmetic.
 */
#include "flame2/exceptions/all.hpp"
#include "flame2/mem/vector_wrapper.hpp"
#include "flame2/config.hpp"
#include "message_iterator_backend_raw.hpp"

namespace flame { namespace mb2 {
  MessageIteratorBackendRaw::MessageIteratorBackendRaw(
      flame::mem::VectorWrapperBase* vw_ptr)
          : v_(vw_ptr),
            count_(vw_ptr->size()),
            pos_(0),
            current_(vw_ptr->GetRawPtr()) {}

  MessageIteratorBackendRaw::~MessageIteratorBackendRaw(void) {}

  bool MessageIteratorBackendRaw::AtEnd(void) const {
    return (pos_ == count_);
  }

  size_t MessageIteratorBackendRaw::GetCount(void) const {
    return count_;
  }

  void MessageIteratorBackendRaw::Rewind(void) {
    pos_ = 0;
    current_ = v_->GetRawPtr();
  }

  bool MessageIteratorBackendRaw::Next(void) {
    if (pos_ == count_) {
      throw flame::exceptions::out_of_range("End of iteration");
    }
    pos_++;
    current_ = v_->StepRawPtr(current_);
    return (pos_ != count_);  // return false if end of iter, true otherwise
  }

  bool MessageIteratorBackendRaw::Randomise(void) {
    throw flame::exceptions::logic_error("Raw backend cannot be randomised");
  }

  void* MessageIteratorBackendRaw::Get(void) {
    return current_;
  }
  
  bool MessageIteratorBackendRaw::IsMutable(void) const {
    return false;
  }
  
  MessageIteratorBackend*
  MessageIteratorBackendRaw::GetMutableVersion(void) const {
    throw flame::exceptions::not_implemented("not yet implemented");
  }
  
}}  // namespace flame::mb2
