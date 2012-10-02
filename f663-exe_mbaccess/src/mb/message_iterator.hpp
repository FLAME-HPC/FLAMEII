/*!
 * \file src/mb/message_iterator.hpp
 * \author Shawn Chin
 * \date September 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Declaration of MessageIterator
 */
#ifndef MB__MESSAGE_ITERATOR_HPP_
#define MB__MESSAGE_ITERATOR_HPP_
#include <string>
#include "exceptions/all.hpp"
#include "type_validator.hpp"
#include "mb_common.hpp"
#include "message.hpp"
#include "message_iterator_backend.hpp"

namespace flame { namespace mb {

class MessageIterator {
  public:
    explicit MessageIterator(MessageIteratorBackend::Handle backend);

    //! Indicates end of iteration
    bool AtEnd(void) const;

    //! Returns total number of messages in the iterator
    size_t GetCount(void) const;

    //! Restarts the iteration
    void Rewind(void);

    // Randomisation can only be done with mutable backends. To be implemented.
    // An immutable backend will be converted with a mutable one before
    // randomisation is performed.
    //   void Randomise(void);

    //! Returns a handle to the current message
    MessageHandle GetMessage(void);

    //! \brief Step through to the next message in the iteration
    bool Next(void);

    /*!
     * \brief Returns a specific variable from the current message
     * \return Message variable value
     *
     * Throws flame::exceptions::out_of_range if there are no more messages
     * to return (end of iteration or empty iterator).
     */
    template <typename T>
    T Get(const std::string& var_name) {
      if (AtEnd()) {
        throw flame::exceptions::out_of_range("End of iteration");
      }
      if (!current_) {  // if no cached message, initialise.
        current_ = backend_->GetMessage();
      }
      return current_->Get<T>(var_name);
    }

  private:
    //! Cache of the current message
    MessageHandle current_;

    /*!
     * \brief Backend with handles the actual message iteration
     *
     * We offload the actual operation to a separate object so we can use
     * different derived classes for different iteration types. We also have
     * the opportunity to replace the backend at run-time without affecting
     * the exising object user.
     *
     * Backends will be swapped when we need a different class of iterators
     * (from immutable to mutable) or if we need to change the contents
     * (filter/union/intersection operations).
     */
    MessageIteratorBackend::Handle backend_;
};

}}  // namespace flame::mb
#endif  // MB__MESSAGE_ITERATOR_HPP_
