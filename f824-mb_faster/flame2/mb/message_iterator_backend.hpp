/*!
 * \file flame2/mb/message_iterator_backend.hpp
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

namespace flame { namespace mb {

//! Abstract base class for message iterator backends
class MessageIteratorBackend {
  public:

    //! Factory method to instantiate a backend of a specific type
    template <typename BackendType>
    static MessageIteratorBackend* create(flame::mem::VectorWrapperBase* vw_ptr) {
      BackendType *b = new BackendType(vw_ptr);
      b->data_type_ = vw_ptr->GetDataType();
      return b;
    }

    //! Destructor
    virtual ~MessageIteratorBackend(void) {};

    //! Returns true if end of iteration reached (or if iterator is empty)
    virtual bool AtEnd(void) const = 0;

    //! Returns the number of messages within the scope of the iterator
    virtual size_t GetCount(void) const = 0;

    //! Move the iteration cursor back to the starting position
    virtual void Rewind(void) = 0;

    /*!
     * \brief Step to the next message in the iterator.
     * \return true if successful, false otherwise (end of iteration)
     */
    virtual bool Next(void) = 0;

    /*!
     * \brief Randomise the order of iteration
     *
     * May throw flame::exceptions::not_implemented if backend does not
     * support randomisation, e.g. with immutable backends.
     *
     * Front-end class should always check using IsMutable().
     */
    virtual bool Randomise(void) = 0;

    /*!
     * \brief Returns an anonymous pointer to the memory location of the message
     *
     * The front-end code is expected to cast this to the correct type and
     * return a copy of the message to end-users. The returned memory location
     * should never be written to.
     *
     * It is advisable to use GetDataType() to validate the expected datatype
     * before using the returned value.
     */
    virtual void* Get(void) = 0;

    //! Returns true if the backend is mutable
    virtual bool IsMutable(void) const = 0;
    
    //! Returns a new mutable backend which addresses the same messages
    virtual MessageIteratorBackend* GetMutableVersion(void) const = 0;

    //! Returns a type_info of the underlying message type
    const std::type_info* GetDataType() const {
      return data_type_;
    } 
    
  private:
    //! Cache of the type_info of the underlying message type
    const std::type_info *data_type_;  
};

}}  // namespace flame::mb

#endif  // MB__MESSAGE_ITERATOR_BACKEND_HPP_
