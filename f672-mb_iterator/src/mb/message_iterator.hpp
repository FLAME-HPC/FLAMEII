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
    bool AtEnd(void) const;
    size_t GetCount(void) const;
    void Rewind(void);
    void Randomise(void);

    /*!
     * \brief Returns a handle to the current message
     * \return Message handle to the current message
     *
     * Returns a shared_ptr to the same message pointed to by current_.
     *
     * The message will be read-only.
     */
    MessageHandle GetMessage(void) const;

    /*!
     * \brief Step through to the next message in the backend iterator
     * \return false if end of iteration, true otherwise
     *
     * Also caches the current message data in current_. This allows us to
     * quickly return the current message data.
     */
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
     */
    MessageIteratorBackend::Handle backend_;
};

}}  // namespace flame::mb
#endif  // MB__MESSAGE_ITERATOR_HPP_
