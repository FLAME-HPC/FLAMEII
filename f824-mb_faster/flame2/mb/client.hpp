/*!
 * \file flame2/mb2/client.hpp
 * \author Shawn Chin
 * \date November 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Definition of the Client class
 *
 * For efficiency, Client intances cache BoardWriters when possible. This means
 * that it is not thread-safe; multiple execution threads sharing a client may
 * result in a race condition when messages are created and posted.
 *
 * Each execution thread should have its own Client instance (which can be
 * generated from the same Proxy instance).
 */
#ifndef MB__CLIENT_HPP_
#define MB__CLIENT_HPP_
#include <string>
#include <set>
#include <boost/container/flat_set.hpp>
#include <boost/container/flat_map.hpp>
#include "message_board.hpp"

namespace flame { namespace mb2 {

// Use flat_set/flat_map instead of set/map. This should yield better
// performance in our case where:
//  * we expect to have a relatively small number of elements
//  * All insertion done before any lookups, and only at the init phase
//  * It is possible to engineer a usage where insertion is in-order
//  * Lookup performance more important than insertion
typedef std::set<std::string> acl_set_type;
typedef boost::container::flat_map<std::string,
                                   MessageBoard::writer> writer_map_type;

class Client {
  public:
    Client(acl_set_type acl_read, acl_set_type acl_post);
    
    MessageBoard::writer GetBoardWriter(const std::string& msg_name);
    // TODO(lsc): GetBoardWriter(msg_name, query);  // when filtering enabled

    MessageBoard::iterator GetMessages(const std::string& msg_name);
    
  private:
    writer_map_type writers_;
    acl_set_type acl_read_;
};

}}  // namespace flame::mb2
#endif  // MB__CLIENT_HPP_
