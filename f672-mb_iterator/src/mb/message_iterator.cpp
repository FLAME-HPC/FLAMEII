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

void MessageIterator::Next(void) {
  return backend_->Next();
}

void MessageIterator::Rewind(void) {
  return backend_->Rewind();
}

MessageHandle MessageIterator::GetMessage(void) const {
  return backend_->GetMessage();
}

}}  // namespace flame:mb
