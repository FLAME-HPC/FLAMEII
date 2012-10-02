/*!
 * \file FILENAME
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief DESCRIPTION
 */
#ifndef MB__MESSAGE_ITERATOR_BACKEND_HPP_
#define MB__MESSAGE_ITERATOR_BACKEND_HPP_
#include "boost/shared_ptr.hpp"
#include "boost/any.hpp"
#include "mb_common.hpp"

namespace flame { namespace mb {

//! Abstract base class for a message iterator backend
class MessageIteratorBackend
{
  public:
    typedef boost::shared_ptr<MessageIteratorBackend> Handle;

    template <typename T>
    static Handle Factory(MemoryMap* vec_map_ptr, TypeValidator *tv) {
      return Handle(new T(vec_map_ptr, tv));
    }

    virtual ~MessageIteratorBackend() {}
    virtual bool AtEnd(void) const = 0;
    virtual size_t GetCount(void) const = 0;
    virtual bool Step(void) = 0;
    virtual void Rewind(void) = 0;

    /*!
     * \brief Returns next message in the iteration
     * \return Message handle
     *
     * Creates a new Message instance and populates with the data from the
     * current iterator position. A shared_ptr to the instance is returned.
     *
     * Throws flame::exceptions::out_of_range if the iteration has ended
     * or is empty.
     */
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
    //virtual Handle GetMutableVersion(void) = 0;


    //! \todo (lsc) Set-like operations to form new iterators for existing ones

  protected:
    MemoryMap* vec_map_;
    TypeValidator* validator_;
    size_t position_;
    size_t count_;

    MessageIteratorBackend(MemoryMap* vec_map_ptr, TypeValidator *tv);
    virtual void* Get(std::string var_name) = 0;
};

}}  // namespace flame::mb
#endif  // MB__MESSAGE_ITERATOR_BACKEND_HPP_
