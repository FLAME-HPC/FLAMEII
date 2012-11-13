/*!
 * \file flame2/mb2/client.hpp
 * \author Shawn Chin
 * \date November 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Implementation of the Client class
 */
#include <stdexcept>
#include "flame2/config.hpp"
#include "flame2/exceptions/all.hpp"
#include "message_board_manager.hpp"
#include "client.hpp"

namespace flame { namespace mb2 {

Client::Client(acl_set_type acl_read, acl_set_type acl_post)
    : acl_read_(acl_read) {
  // we don't cache iterators since they can be modified by users, e.g.
  // randomisation, sorting, etc.
  // Instead we keep a set of msg names with read privs(acl_read_)
  
  // writers we can cache. As long as each Client instance is never used
  // concurrently (one per thread) we can get away with reusing the same writer
  MessageBoardManager &m = MessageBoardManager::GetInstance();
  acl_set_type::const_iterator it = acl_post.begin();
  acl_set_type::const_iterator end = acl_post.end();
  for (; it != end; ++it) {
    writers_.insert(writer_map_type::value_type(*it, m.GetBoardWriter(*it)));
  }
}

MessageBoard::writer Client::GetBoardWriter(const std::string& msg_name) {
  try {
    return writers_.at(msg_name);
  } catch(const std::out_of_range& E) {
    throw flame::exceptions::invalid_operation("No post access for this msg");
  }
}

MessageBoard::iterator Client::GetMessages(const std::string& msg_name) {
  if (acl_read_.find(msg_name) == acl_read_.end()) {
    throw flame::exceptions::invalid_operation("No read access for this msg");
  }
  return MessageBoardManager::GetInstance().GetMessages(msg_name);
}

}}  // namespace flame::mb2
