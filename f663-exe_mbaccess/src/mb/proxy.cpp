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
#include "client.hpp"
#include "proxy.hpp"

namespace flame { namespace mb {

void Proxy::AllowRead(const std::string& msg_name) {
  // Check if msg_name_ already in acl_read_
  StringSet::iterator lb = acl_read_.lower_bound(msg_name);
  if (lb != acl_read_.end() && !(acl_read_.key_comp()(msg_name, *lb))) {
    return;  // already exist in set. nothing to do
  } else if (_can_post(msg_name)) { // Check for read/post conflicts
    throw flame::exceptions::invalid_operation("Cannot read AND post to the "
                                               "same message");
  } else if (!MessageBoardManager::GetInstance().BoardExists(msg_name)) {
    throw flame::exceptions::invalid_argument("Unknown message board name");
  }

  // Add to acl_read_
  acl_read_.insert(lb, msg_name);
}

void Proxy::AllowPost(const std::string& msg_name) {
  // Check if msg_name_ already in acl_post_
  StringSet::iterator lb = acl_post_.lower_bound(msg_name);
  if (lb != acl_post_.end() && !(acl_post_.key_comp()(msg_name, *lb))) {
    return;  // already exist in set. nothing to do
  } else if (_can_read(msg_name)) { // Check for read/post conflicts
    throw flame::exceptions::invalid_operation("Cannot read AND post to the "
                                               "same message");
  } else if (!MessageBoardManager::GetInstance().BoardExists(msg_name)) {
    throw flame::exceptions::invalid_argument("Unknown message board name");
  }

  // Add to acl_post_
  acl_post_.insert(lb, msg_name);
}

bool Proxy::_can_read(const std::string& msg_name) {
  return (acl_read_.find(msg_name) != acl_read_.end());
}

bool Proxy::_can_post(const std::string& msg_name) {
  return (acl_post_.find(msg_name) != acl_post_.end());
}

//! Returns a Client than can be used to access boards
ClientHandle Proxy::GetClient(void) {
  return ClientHandle(new Client(acl_read_, acl_post_));
}

}}  // namespace flame::mb
