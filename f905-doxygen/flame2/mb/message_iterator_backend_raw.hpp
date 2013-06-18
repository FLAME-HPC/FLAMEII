/*!
 * \file flame2/mb/message_iterator_backend_raw.hpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Implementation of an Iterator Backend that references the original
 * message data and steps through using pointer arithmetic.
 */
#ifndef MB__MESSAGE_ITERATOR_BACKEND_RAW_HPP_
#define MB__MESSAGE_ITERATOR_BACKEND_RAW_HPP_
#include "flame2/exceptions/all.hpp"
#include "flame2/mem/vector_wrapper.hpp"
#include "message_iterator_backend.hpp"

namespace flame { namespace mb {

/*!
 * \brief Default message iterator backend
 *
 * This backend should be one or the more efficient ones, but because it
 * references the actual data, iteration must be in order. This backend is
 * therefore immutable.
 *
 * To access messages out of order (sorted/randomised), the parent iterator
 * needs to first generate a new backend which is mutable using
 * GetMutableVersion().
 */
class MessageIteratorBackendRaw : public MessageIteratorBackend {
  public:
    /*!
     * \brief Constructor
     * \param vw_ptr Pointer to the VectorWrapper instance of the board
     *
     * We cache the number of messages at the point of creation and store
     * the pointer to the actual VectorWrapper within the message board.
     *
     * Messages added to the board after this point will be beyond the scope of
     * iteration.
     *
     * If the board is cleared, this backend should be considered invalid and
     * accessing it will result in undefined behaviour. It is the framework's
     * responsibility to ensure that this does not happen.
     */
    explicit MessageIteratorBackendRaw(flame::mem::VectorWrapperBase* vw_ptr)
        : v_(vw_ptr), count_(vw_ptr->size()), pos_(0),
          current_(vw_ptr->GetRawPtr()) {}

    //! Returns true if end of iteration reached
    inline bool AtEnd(void) const {
      return (pos_ >= count_);
    }

    //! Returns the number of messages within the scope of iteration
    inline size_t GetCount(void) const {
      return count_;
    }

    //! Move the cursor back to the starting position
    inline void Rewind(void) {
      pos_ = 0;
      current_ = v_->GetRawPtr();
    }

    /*!
     * \brief steps to the next message
     * \return true if cursor moved, false if already at the end.
     *
     * Step to the next message if we're not yet at the end. If there's nothing
     * to step to, do nothing and return false.
     */
    inline bool Next(void) {
      pos_++;
      if (AtEnd()) {
        pos_ = count_;
        current_ = NULL;
        return false;
      } else {
        current_ = v_->StepRawPtr(current_);
        return true;  // indicate stepping was successful
      }
    }

    /*!
     * \brief (Randomisation not supported by this backend)
     *
     * Always throws flame::exceptions::not_implemented.
     */
    inline void Randomise(void) {
      throw flame::exceptions::not_implemented(
          "Raw backend cannot be randomised");
    }

    /*! 
     * \brief Returns a pointer to the current message, or NULL if end of iteration
     * \return pointer to current message
     */
    inline void* Get(void) {
      return current_;
    }

    /*! 
     * \brief Query if this backend is mutable
     * \return false. This backend is not mutable.
     */
    inline bool IsMutable(void) const {
      return false;
    }

    /*! 
     * \brief Create mutable version of this backend
     * \return pointer to new MessageIteratorBackend instance
     * 
     * (not yet implemented)
     */
    inline MessageIteratorBackend* GetMutableVersion(void) const {
      throw flame::exceptions::not_implemented("not yet implemented");
    }

  private:
    //! Pointer to the VectorWrapper of the board
    flame::mem::VectorWrapperBase* v_;
    size_t count_;  //!< Number of messages within the scope of iteration
    size_t pos_;  //!< Current cursor position
    void* current_;  //!< Pointer to current message
};

}}  // namespace flame::mb

#endif  // MB__MESSAGE_ITERATOR_BACKEND_RAW_HPP_
