/*!
 * \file FILENAME
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief DESCRIPTION
 */
#include "message.hpp"
#include "message_iterator_backend.hpp"

namespace flame { namespace mb {

MessageIteratorBackend::MessageIteratorBackend(MemoryMap* vec_map_ptr,
                                               TypeValidator *tv)
    : vec_map_(vec_map_ptr), validator_(tv), position_(0) {

  MemoryMap::const_iterator iter = vec_map_->begin();
  // Get size from the first vector. (0 if no variables)
  count_ = (iter == vec_map_->end()) ? 0 : iter->second->size();
}

MessageHandle MessageIteratorBackend::GetMessage(void) {
  if (AtEnd()) {
    return MessageHandle();  // return null handle
  }

  // instantiate and populate Message
  MessageHandle msg = MessageHandle(new Message(validator_));
  MemoryMap::iterator iter = vec_map_->begin();
  MemoryMap::iterator end = vec_map_->end();
  for (; iter != end; ++iter) {
    msg->Set(iter->first, iter->second->ConvertToBoostAny(Get(iter->first)));
  }
  return msg;
}
}}  // namespace flame::mb
