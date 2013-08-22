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
    /*! 
     * \brief Factory method to instantiate a backend of a specific type
     * \param vw_ptr pointer to message vector wrapper
     * \return pointer to MessageIteratorBacked instance
     */
    template <typename BackendType>
    static MessageIteratorBackend* create(
          flame::mem::VectorWrapperBase* vw_ptr) {
      BackendType *b = new BackendType(vw_ptr);
      b->data_type_ = vw_ptr->GetDataType();
      return b;
    }

    //! Constructor, initialise data members
    MessageIteratorBackend(void) : data_type_(0) {}

    //! Destructor
    virtual ~MessageIteratorBackend(void) {}

    /*! 
     * \brief Query if end of iteration reached (or if iterator is empty)
     * \return true if end of iteration reached
     */
    virtual bool AtEnd(void) const = 0;

    /*! 
     * \brief Query the number of messages within the scope of the iterator
     * \return number of messages
     */
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
    virtual void Randomise(void) = 0;

    /*!
     * \brief Returns an anonymous pointer to the memory location of the message
     * \return raw pointer to message
     *
     * The front-end code is expected to cast this to the correct type and
     * return a copy of the message to end-users. The returned memory location
     * should never be written to.
     *
     * It is advisable to use GetDataType() to validate the expected datatype
     * before using the returned value.
     */
    virtual void* Get(void) = 0;

    /*! 
     * \brief Query if backend is mutable
     * \return true if mutable
     */
    virtual bool IsMutable(void) const = 0;

    /*! 
     * \brief Create a new mutable backend which addresses the same messages
     * \return pointer to new MessageIteratorBackend instance
     */
    virtual MessageIteratorBackend* GetMutableVersion(void) const = 0;

    /*! 
     * \brief Query type_info of the underlying message type
     * \return type_info of message datatype
     */
    const std::type_info* GetDataType() const {
      return data_type_;
    }

  private:
    //! Cache of the type_info of the underlying message type
    const std::type_info *data_type_;
    //! This class has pointer members so disable copy constructor
    MessageIteratorBackend(const MessageIteratorBackend&);
    //! This class has pointer members so disable assignment operation
    void operator=(const MessageIteratorBackend&);
};

}}  // namespace flame::mb

#endif  // MB__MESSAGE_ITERATOR_BACKEND_HPP_
