/*!
 * \file flame2/mb2/message_iterator_backend.hpp
 * \author Shawn Chin
 * \date November 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Definition of the Abstract base class for MessageIteratorBackend
 */
#ifndef MB__MESSAGE_ITERATOR_BACKEND_HPP_
#define MB__MESSAGE_ITERATOR_BACKEND_HPP_
#include "flame2/mem/vector_wrapper.hpp"

namespace flame { namespace mb2 {

class MessageIteratorBackend {
  public:
    template <typename BackendType>
    static MessageIteratorBackend* create(flame::mem::VectorWrapperBase* vw_ptr) {
      BackendType *b = new BackendType(vw_ptr);
      b->data_type_ = vw_ptr->GetDataType();
      return b;
    }
    
    ~MessageIteratorBackend(void) {};
    virtual bool AtEnd(void) const = 0;
    virtual size_t GetCount(void) const = 0;
    virtual void Rewind(void) = 0;
    virtual bool Next(void) = 0;
    virtual bool Randomise(void) = 0;
    virtual void* Get(void) = 0;

    virtual bool IsMutable(void) const = 0;
    virtual MessageIteratorBackend* GetMutableVersion(void) const = 0;
    
    const std::type_info* GetDataType() const {
      return data_type_;
    } 
    
  private:
    const std::type_info *data_type_;
};

}}  // namespace flame::mb2

#endif  // MB__MESSAGE_ITERATOR_BACKEND_HPP_
