/*!
 * \file src/mb/message_iterator_backend.hpp
 * \author Shawn Chin
 * \date October 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Definition of the Abstract base class for MessageIteratorBackend
 */
#ifndef MB__MESSAGE_ITERATOR_BACKEND_HPP_
#define MB__MESSAGE_ITERATOR_BACKEND_HPP_
#include <string>
#include "boost/shared_ptr.hpp"
#include "boost/any.hpp"
#include "mb_common.hpp"

namespace flame { namespace mb {

//! Abstract base class for a message iterator backend
class MessageIteratorBackend {
  public:
    //! Shared pointer handle to store MessageIteratorBackend
    typedef boost::shared_ptr<MessageIteratorBackend> Handle;

    //! Class method to build a MessageIteratorBackend
    template <typename T>
    static Handle Factory(MemoryMap* vec_map_ptr, TypeValidator *tv) {
      return Handle(new T(vec_map_ptr, tv));
    }

    //! Destructor
    virtual ~MessageIteratorBackend() {}

    //! Indicates end of iteration
    virtual bool AtEnd(void) const = 0;

    //! Returns total number of messages in the iterator
    virtual size_t GetCount(void) const = 0;

    //! \brief Step through to the next message in the iteration
    virtual bool Step(void) = 0;

    //! Restarts the iteration
    virtual void Rewind(void) = 0;

    //! Returns next message in the iteration
    MessageHandle GetMessage(void);

    /*!
     * \brief Determine if a this backend is mutable
     * \return true/false
     *
     * Immutable backends access raw pointers directly and is therefore fast.
     *
     * Mutable version access via a index array and is slower compared to
     * immutable ones. However, the list of indices can be modified and so
     * the iterator is randomisable, sortable, and can be used to iterate a
     * through a non-contiguous subset of messages
     */
    virtual bool IsMutable(void) const = 0;

    /*!
     * \brief Return a mutable backend with the same content
     * \return Handle to new backend
     *
     * For an immutable backend, this will generate and return a mutable
     * counterpart.
     *
     * If called on a mutable backend, a handle to the same
     */
    //  virtual Handle GetMutableVersion(void) = 0;


    //! \todo (lsc) Set-like operations to form new iterators for existing ones

  protected:
    MemoryMap* vec_map_;  //! Pointer to the map of vectors holding messages
    TypeValidator* validator_;  //! Pointer to the type validator object
    size_t position_;  //! Counter indicating the current iteration position
    size_t count_;  //! Number of messages referenced by the iterator

    //! Constructor
    MessageIteratorBackend(MemoryMap* vec_map_ptr, TypeValidator *tv);

    /*! Returns the raw poiter to the current message variable
     *
     * This should return NULL if the there are no messages to return.
     */
    virtual void* Get(std::string var_name) = 0;
};

}}  // namespace flame::mb
#endif  // MB__MESSAGE_ITERATOR_BACKEND_HPP_
