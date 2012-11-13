/*!
 * \file flame2/mb/message_iterator_backend_raw.hpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Implementation of an Iterator Backend that references the original
 * message data and steps through using pointer arithmetic.
 *
 * This backend should be one or the more efficient ones, but becuase it
 * references the actual data, iteration must be in order. This backend is
 * therefore immutable.
 *
 * To access messages out of order (sorted/randomised) or to iterated through
 * only a subset of messages, the parent iterator needs to first generate
 * a new backend which is mutable using GetMutableVersion().
 */
#ifndef MB__MESSAGE_ITERATOR_BACKEND_RAW_HPP_
#define MB__MESSAGE_ITERATOR_BACKEND_RAW_HPP_
#include "flame2/exceptions/all.hpp"
#include "flame2/mem/vector_wrapper.hpp"
#include "message_iterator_backend.hpp"

namespace flame { namespace mb2 {

class MessageIteratorBackendRaw : public MessageIteratorBackend {
  public:
    MessageIteratorBackendRaw(flame::mem::VectorWrapperBase* vw_ptr)
        : v_(vw_ptr), count_(vw_ptr->size()), pos_(0),
          current_(vw_ptr->GetRawPtr()) {}
          
    ~MessageIteratorBackendRaw(void) {}
    
    inline bool AtEnd(void) const {
      return (pos_ == count_);
    }
    
    inline size_t GetCount(void) const {
      return count_;
    }
    
    inline void Rewind(void) {
      pos_ = 0;
      current_ = v_->GetRawPtr();
    }
    
    inline bool Next(void) {
      if (pos_ == count_) {
        throw flame::exceptions::out_of_range("End of iteration");
      }
      pos_++;
      current_ = v_->StepRawPtr(current_);
      return (pos_ != count_);  // return false if end of iter, true otherwise
    }
    
    inline bool Randomise(void) {
      throw flame::exceptions::logic_error("Raw backend cannot be randomised");
    }
    
    inline void* Get(void) {
      return current_;
    }
    
    inline bool IsMutable(void) const {
      return false;
    }
    
    inline MessageIteratorBackend* GetMutableVersion(void) const {
      throw flame::exceptions::not_implemented("not yet implemented");
    }
    
  private:
    flame::mem::VectorWrapperBase* v_;
    size_t count_;
    size_t pos_;
    void* current_;
};

}}  // namespace flame::mb2

#endif  // MB__MESSAGE_ITERATOR_BACKEND_RAW_HPP_
