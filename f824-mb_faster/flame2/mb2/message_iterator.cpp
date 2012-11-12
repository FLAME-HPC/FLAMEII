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
  return _backend->AtEnd();
}

size_t MessageIterator::GetCount(void) const {
  return _backend->GetCount();
}

void MessageIterator::Rewind(void) {
  _backend->Rewind();
}

bool MessageIterator::Next(void) {
  return _backend->Next();
}

void MessageIterator::Randomise(void) {
  _RequireMutableBackend(); 
  _backend->Randomise();
}


void MessageIterator::_RequireMutableBackend(void) {
  if (_backend->IsMutable()) return;
  boost::scoped_ptr<MessageIteratorBackend> b(_backend->GetMutableVersion());
  _backend.swap(b);
}

}}  // namespace::mb2
