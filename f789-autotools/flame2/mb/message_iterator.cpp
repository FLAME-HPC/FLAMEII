/*!
 * \file flame2/mb/message_iterator.cpp
 * \author Shawn Chin
 * \date September 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Implementation of MessageIterator
 */
#include "message_iterator_backend.hpp"
#include "message_iterator_backend_raw.hpp"
#include "message_iterator.hpp"

namespace flame { namespace mb {

MessageIterator::MessageIterator(MessageIteratorBackend::Handle backend)
    : backend_(backend) {}

/*!
 * \brief Indicates end of iteration
 * \return true if end of iteration, false otherwise
 *
 * This simply calls the AtEnd() method of the backend.
 */
bool MessageIterator::AtEnd(void) const {
  return backend_->AtEnd();
}

/*!
 * \brief Returns the number of messages in addressed by the iterator
 * \return Number of messages
 *
 * This simply calls the GetCount() method of the backend.
 */
size_t MessageIterator::GetCount(void) const {
  return backend_->GetCount();
}

/*!
  * \brief Step through to the next message in the iteration
  * \return false if end of iteration, true otherwise
  *
  * Also caches the current message data in current_. This allows us to
  * quickly return the current message data as well as retrieve individual
  * message variables using Get()
  */
bool MessageIterator::Next(void) {
  bool status = backend_->Step();
  current_ = backend_->GetMessage();
  return status;
}

/*!
 * \brief Restarts the iteration
 *
 * Calls the Rewind() method of the backend, then updates the local
 * message cache.
 */
void MessageIterator::Rewind(void) {
  backend_->Rewind();
  current_ = backend_->GetMessage();  // Update cache
}

/*!
 * \brief Returns a handle to the current message
 * \return Message handle to the current message
 *
 * Returns a shared_ptr to the same message pointed to by current_.
 *
 * The message will be read-only.
 */
MessageHandle MessageIterator::GetMessage(void) {
  if (backend_->AtEnd()) {
    throw flame::exceptions::out_of_range("End of iteration");
  }
  if (!current_) {  // if no cached message, initialise.
    current_ = backend_->GetMessage();
  }
  return current_;
}

}}  // namespace flame:mb
