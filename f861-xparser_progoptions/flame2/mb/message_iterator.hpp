/*!
 * \file flame2/mb/message_iterator.hpp
 * \author Shawn Chin
 * \date November 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Declaration of MessageIterator
 */
#ifndef MB__MESSAGE_ITERATOR_HPP_
#define MB__MESSAGE_ITERATOR_HPP_
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include "flame2/mem/vector_wrapper.hpp"
#include "flame2/exceptions/all.hpp"
#include "message_iterator_backend.hpp"
#include "message_iterator_backend_raw.hpp"

namespace flame { namespace mb {

/*!
 *
 * \brief Iterator object to step through messages within a board
 *
 * This is essentially a wrapper class which relies on a MessageIteratorBackend
 * to store and interact with messages. This additional layer of indirection
 * allows us to swap backends transparently when the need arises.
 *
 * For example, the most efficient way to iterate through messages that are
 * contiguous in memory would be to use a raw pointer to the actual message
 * and use pointer arithmetic to step to the next message. This is done by the
 * default backend (see MessageIteratorBackendRaw).
 *
 * However, when operations such as filtering or randomisation is requested,
 * the iteration is no longer in order and we then need to manage a list of
 * indices to drive the iteration. This is when a different backend is required.
 * By swapping out the existing backend with a new one, the operation is
 * transparent to users and the only hint of this happening is a potential
 * performance degration after the less performant but more flexible backend
 * is swapped in.
 */
class MessageIterator {
  public:
    //! shared pointer type to be returned as handle to iterator instances
    typedef boost::shared_ptr<MessageIterator> handle;

    //! Factory method to instantiate a MessageIterator with the default backend
    static MessageIterator* create(flame::mem::VectorWrapperBase* vw_ptr) {
      return new MessageIterator(
          MessageIteratorBackend::create<MessageIteratorBackendRaw>(vw_ptr));
    }

    /*  --- we don't need this yet ---
    //  Factory function for specific backends
    template <typename BackendType>
    static MessageIterator* create(VectorWrapperBase* vw_ptr) {
      return new MessageIterator(
          MessageIteratorBackend::create<BackendType>(vw_ptr)
      );
    }
    */

    //! Returns true if end of iterator reached
    inline bool AtEnd(void) const {
      return backend_->AtEnd();
    }

    //! Returns the number of messages within the scope of the iterator
    inline size_t GetCount(void) const {
      return backend_->GetCount();
    }

    //! Move the cursor back to the beginning
    inline void Rewind(void) {
      backend_->Rewind();
    }

    /*!
     * \brief Step to the next message in the iteration
     * \return true if successful, false otherwise (end of iteration)
     */
    inline bool Next(void) {
      return backend_->Next();
    }

    /*!
     * \brief Randomise the order of iteration
     *
     * (Not yet implemented)
     *
     * Out of order iteration of messages requires the backend to
     * store an intermediate array of indices (a mutable backend).
     * _RequireMutableBackend() is therefore called to swap the backend if the
     * existing one is not mutable.
     *
     * Non-mutable backends are less flexible but more performant; we can
     * therefore expect randomisation to potentially increase the overheads
     * of message iteration.
     *
     * Randomisation of an in-progress iterator will rewind it.
     */
    inline void Randomise(void) {
      _RequireMutableBackend();
      backend_->Rewind();
      backend_->Randomise();
    }

    /*!
     * \brief Returns a copy of the current message
     *
     * Throws flame::exceptions::invalid_type if the specified type does not
     * match the message type.
     *
     * Throws flame::exceptions::out_of_range if the iterator is empty or if
     * end of iteration is reached.
     */
    template <typename T>
    T Get(void) {
#ifndef DISABLE_RUNTIME_TYPE_CHECKING
      if (*(backend_->GetDataType()) != typeid(T)) {
        throw flame::exceptions::invalid_type("mismatching type");
      }
#endif
      if (AtEnd()) {
        throw flame::exceptions::out_of_range("End of iteration");
      }
      return T(*static_cast<T*>(backend_->Get()));
    }

  private:
    /*!
     * \brief pointer to backend instance
     *
     * Using a scoped_ptr, the lifespan of the backend  is automatically
     * managed and the object is a deleted along with the iterator.
     */
    boost::scoped_ptr<MessageIteratorBackend> backend_;  //! pointer to backend

    //! Private constructor
    explicit MessageIterator(MessageIteratorBackend *b) : backend_(b) {}

    //! Swaps backed to a mutable version if not already mutable
    inline void _RequireMutableBackend(void) {
      if (backend_->IsMutable()) return;
      boost::scoped_ptr<MessageIteratorBackend>
          b(backend_->GetMutableVersion());
      backend_.swap(b);
    }
};

}}  // namespace flame::mb
#endif  // MB__MESSAGE_ITERATOR_HPP_
