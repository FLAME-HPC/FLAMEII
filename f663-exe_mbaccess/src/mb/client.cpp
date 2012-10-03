/*!
 * \file FILENAME
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief DESCRIPTION
 */
#include <utility>
#include "message_board_manager.hpp"
#include "board_writer.hpp"
#include "client.hpp"

namespace flame { namespace mb {

/*!
 * \brief Constructor
 *
 * Copies acl_read and acl_post.
 */
Client::Client(const Proxy::StringSet& acl_read,
               const Proxy::StringSet& acl_post)
    : acl_read_(acl_read), acl_post_(acl_post) {}

bool Client::_can_read(const std::string& msg_name) {
  return (acl_read_.find(msg_name) != acl_read_.end());
}

bool Client::_can_post(const std::string& msg_name) {
  return (acl_post_.find(msg_name) != acl_post_.end());
}

//! Returns an iterator to read all messages in the board
MessageIteratorHandle Client::GetMessages(const std::string& msg_name) {
  if (!_can_read(msg_name)) {
    throw flame::exceptions::invalid_operation("No read access for this msg");
  }
  return MessageBoardManager::GetInstance().GetMessageIterator(msg_name);
}

//! Returns a handle to a board writer
//! More efficient than NewWriter() if retrieving multiple message instances
BoardWriterHandle Client::GetWriter(const std::string& msg_name) {
  // First, check the cache
  WriterMap::iterator lb = writer_cache_.lower_bound(msg_name);
  if (lb != writer_cache_.end() &&
        !(writer_cache_.key_comp()(msg_name, lb->first))) { // found
    if (!lb->second->IsConnected()) {  // writer disconnected. Recreate
      lb->second = MessageBoardManager::GetInstance().GetBoardWriter(msg_name);
    }
    return lb->second;
  }
  // not found in cache. First, check permissions
  if (!_can_post(msg_name)) {
    throw flame::exceptions::invalid_operation("No post access for this msg");
  }
  // then create new, add to cache and return
  lb = writer_cache_.insert(lb, WriterMap::value_type(msg_name,
              MessageBoardManager::GetInstance().GetBoardWriter(msg_name)));
  return lb->second;
}

//! Returns a handle to a new message which which can post to a board
MessageHandle Client::NewMessage(const std::string& msg_name) {
  return GetWriter(msg_name)->NewMessage();
}

}}  // namespace flame::mb
