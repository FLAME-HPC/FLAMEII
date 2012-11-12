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
#include "flame2/mem/vector_wrapper.hpp"
#include "message_iterator_backend.hpp"

namespace flame { namespace mb2 {

class MessageIteratorBackendRaw : public MessageIteratorBackend {
  public:
    MessageIteratorBackendRaw(flame::mem::VectorWrapperBase* vw_ptr);
    ~MessageIteratorBackendRaw(void);
    bool AtEnd(void) const;
    size_t GetCount(void) const;
    void Rewind(void);
    bool Next(void);
    bool Randomise(void);
    void* Get(void);

    bool IsMutable(void) const;
    MessageIteratorBackend* GetMutableVersion(void) const;
    
  private:
    flame::mem::VectorWrapperBase* v_;
    size_t count_;
    size_t pos_;
    void* current_;
};

}}  // namespace flame::mb2

#endif  // MB__MESSAGE_ITERATOR_BACKEND_RAW_HPP_
