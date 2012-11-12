/*!
 * \file flame2/mb2/message_iterator.hpp
 * \author Shawn Chin
 * \date November 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Implementation of of MessageIterator
 */
#include <boost/scoped_ptr.hpp>
#include "flame2/config.hpp"
#include "message_iterator.hpp"

namespace flame { namespace mb2 {

bool MessageIterator::AtEnd(void) const {
  return backend_->AtEnd();
}

size_t MessageIterator::GetCount(void) const {
  return backend_->GetCount();
}

void MessageIterator::Rewind(void) {
  backend_->Rewind();
}

bool MessageIterator::Next(void) {
  return backend_->Next();
}

void MessageIterator::Randomise(void) {
  _RequireMutableBackend(); 
  backend_->Randomise();
}


void MessageIterator::_RequireMutableBackend(void) {
  if (backend_->IsMutable()) return;
  boost::scoped_ptr<MessageIteratorBackend> b(backend_->GetMutableVersion());
  backend_.swap(b);
}

}}  // namespace::mb2
