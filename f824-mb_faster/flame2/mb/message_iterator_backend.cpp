/*!
 * \file flame2/mb/message_iterator_backend.hpp
 * \author Shawn Chin
 * \date October 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Implementation of the Abstract base class for MessageIteratorBackend
 */
#include "flame2/config.hpp"
#include "message.hpp"
#include "message_iterator_backend.hpp"

namespace flame { namespace mb {

/*!
 * \brief Constructor
 *
 * Initialises object members.
 *
 * \c count_ is initialised based on the first vector in the map referenced
 * by \c vec_map_ptr. We expect all vectors in the map to be of equal sizes.
 */
MessageIteratorBackend::MessageIteratorBackend(MemoryMap* vec_map_ptr,
                                               TypeValidator *tv)
    : vec_map_(vec_map_ptr), validator_(tv), position_(0) {

  MemoryMap::const_iterator iter = vec_map_->begin();
  // Get size from the first vector. (0 if no variables)
  count_ = (iter == vec_map_->end()) ? 0 : iter->second->size();
}

/*!
  * \brief Returns next message in the iteration
  * \return Message handle
  *
  * Creates a new Message instance and populates with the data from the
  * current iterator position. A shared_ptr to the instance is returned.
  *
  * Returns a null handle if the iteration has ended.
  */
MessageHandle MessageIteratorBackend::GetMessage(void) {
  if (AtEnd()) {
    return MessageHandle();  // null handle
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
