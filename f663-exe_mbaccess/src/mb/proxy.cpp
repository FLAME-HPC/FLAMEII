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

/*!
 * \brief Grant read access to named message board
 *
 * If read permission his already granted to msg_name, this method
 * returns successfully and nothing changes.
 *
 * If an unknown message name is provided, flame::exceptions::invalid_argument
 * is thrown.
 *
 * If msg_name already has post permission (FLAME disallows reads and posts to
 * the same board from the same task), flame::exceptions::invalid_operation
 * is thrown.
 */
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

/*!
 * \brief Grant post access to named message board
 *
 * If post permission his already granted to msg_name, this method
 * returns successfully and nothing changes.
 *
 * If an unknown message name is provided, flame::exceptions::invalid_argument
 * is thrown.
 *
 * If msg_name already has read permission (FLAME disallows reads and posts to
 * the same board from the same task), flame::exceptions::invalid_operation
 * is thrown.
 */
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

//! Determines if read permission has been granted
bool Proxy::_can_read(const std::string& msg_name) {
  return (acl_read_.find(msg_name) != acl_read_.end());
}

//! Determines if post permission has been granted
bool Proxy::_can_post(const std::string& msg_name) {
  return (acl_post_.find(msg_name) != acl_post_.end());
}

//! Returns a handle to a new client instance
ClientHandle Proxy::GetClient(void) {
  return ClientHandle(new Client(acl_read_, acl_post_));
}

}}  // namespace flame::mb
