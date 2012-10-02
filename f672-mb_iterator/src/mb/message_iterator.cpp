/*!
 * \file src/mb/message_iterator.cpp
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

bool MessageIterator::AtEnd(void) const {
  return backend_->AtEnd();
}

size_t MessageIterator::GetCount(void) const {
  return backend_->GetCount();
}

bool MessageIterator::Next(void) {
  bool status = backend_->Step();
  current_ = backend_->GetMessage();
  return status;
}

void MessageIterator::Rewind(void) {
  backend_->Rewind();
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
  if (!current_) { // if no cached message, initialise.
    current_ = backend_->GetMessage();
  }
  return current_;
}

}}  // namespace flame:mb
