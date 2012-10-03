/*!
 * \file src/mb/proxy.cpp
 * \author Shawn Chin
 * \date October 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Implementation of Proxy
 */
#include "exceptions/all.hpp"
#include "message_board_manager.hpp"
#include "board_writer.hpp"
#include "proxy.hpp"
namespace flame { namespace mb {

void Proxy::AllowRead(const std::string& msg_name) {
  // Check if msg_name_ already in acl_read_
  StringSet::iterator lb = acl_read_.lower_bound(msg_name);
  if (lb != acl_read_.end() && *lb == msg_name) {
    return;  // already exist in set. nothing to do
  }

  // Check that this does not conflict with post access
  if (_can_post(msg_name)) {
    throw flame::exceptions::invalid_operation("Cannot read AND post to the "
                                               "same message");
  }

  // Check that the board exists
  if (!MessageBoardManager::GetInstance().BoardExists(msg_name)) {
    throw flame::exceptions::invalid_argument("Unknown message board name");
  }

  // Add to acl_read_
  acl_read_.insert(lb, msg_name);
}

void Proxy::AllowPost(const std::string& msg_name) {
  // Check if msg_name_ already added
  WriterMap::iterator lb = writer_cache_.lower_bound(msg_name);
  if (lb != writer_cache_.end() &&
      !(writer_cache_.key_comp()(msg_name, lb->first))) {
    return; // already exist. nothing to do
  }

  // Check that this does not conflict with post access
  if (_can_read(msg_name)) {
    throw flame::exceptions::invalid_operation("Cannot read AND post to the "
                                               "same message");
  }

  // Check that the board exists
  if (!MessageBoardManager::GetInstance().BoardExists(msg_name)) {
    throw flame::exceptions::invalid_argument("Unknown message board name");
  }

  // insert key with default (null handle) value.
  writer_cache_.insert(lb, WriterMap::value_type(msg_name,
                                                 WriterMap::mapped_type()));
}

bool Proxy::_can_read(const std::string& msg_name) {
  return (acl_read_.find(msg_name) != acl_read_.end());
}

bool Proxy::_can_post(const std::string& msg_name) {
  return (writer_cache_.find(msg_name) != writer_cache_.end());
}


//! Returns an iterator to read all messages in the board
MessageIteratorHandle Proxy::GetMessages(const std::string& msg_name) {
  if (!_can_read(msg_name)) {
    throw flame::exceptions::invalid_operation("No read access for this msg");
  }
  return MessageBoardManager::GetInstance().GetMessageIterator(msg_name);
}

//! Returns a handle to a board writer
//! More efficient than NewWriter() if retrieving multiple message instances
BoardWriterHandle Proxy::GetWriter(const std::string& msg_name) {
  // retrieve cached handle with that key
  WriterMap::iterator iter = writer_cache_.find(msg_name);
  if (iter == writer_cache_.end()) {  // unknown key
    throw flame::exceptions::invalid_operation("No post access for this msg");
  }

  // if handle is null or writer is disconnected, get a new one
  if (!iter->second || !iter->second->IsConnected()) {
    BoardWriterHandle writer = \
          MessageBoardManager::GetInstance().GetBoardWriter(msg_name);
    writer_cache_[msg_name] = writer;  // update cache
    return writer;
  } else {  // return cached writer
    return iter->second;
  }
}

//! Returns a handle to a new message which which can post to a board
MessageHandle Proxy::NewMessage(const std::string& msg_name) {
  return GetWriter(msg_name)->NewMessage();
}

}}  // namespace flame::mb
