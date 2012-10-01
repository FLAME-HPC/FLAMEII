/*!
 * \file src/mb/message_iterator.cpp
 * \author Shawn Chin
 * \date September 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Implementation of MessageIterator
 */
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
  return backend_->Rewind();
}

/*!
 * \brief Returns a handle to the current message
 * \return Message handle to the current message
 *
 * Returns a shared_ptr to the same message pointed to by current_.
 *
 * The message will be read-only.
 */
MessageHandle MessageIterator::GetMessage(void) const {
  return current_;
}

}}  // namespace flame:mb
