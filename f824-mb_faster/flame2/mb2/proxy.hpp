/*!
 * \file flame2/mb2/proxy.cpp
 * \author Shawn Chin
 * \date October 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Declaration of Proxy
 */
#ifndef MB__PROXY_HPP_
#define MB__PROXY_HPP_
#include <string>
#include <boost/shared_ptr.hpp>
#include "client.hpp"

namespace flame { namespace mb2 {

/*!
 * \brief Proxy object to produce Clients that can access message board
 *
 * Clients generated from a Proxy instance can only access boards that have
 * permission explicitly granted using AllowRead() and AllowPost().
 *
 * A Proxy class should be assigned to a Task to determine its permissions.
 * At execution time, a Client should be generated for each execution thread
 * to avoid race conditions.
 */
class Proxy {
  public:
    typedef boost::shared_ptr<Client> client;
    
    client GetClient(void) {
      return client(new Client(acl_read_, acl_post_));
    }
    
    inline AllowRead(const std::string& msg_name) {
#ifndef NDEBUG
      // perform this check only in debug mode. This routine is never called by
      // end users so unless there's a bug in the framework the test shouldn't
      // fail.
      if (_can_post(msg_name)) {  // Check for read/post conflicts
        throw flame::exceptions::invalid_operation("read and post to same msg");
      } else if (!MessageBoardManager::GetInstance().BoardExists(msg_name)) {
        throw flame::exceptions::invalid_argument("Unknown message name");
      }
#endif
      acl_read_.insert(msg_name);
    }

    inline AllowPost(const std::string& msg_name) {
#ifndef NDEBUG
      // perform this check only in debug mode. This routine is never called by
      // end users so unless there's a bug in the framework the test shouldn't
      // fail.
      if (_can_read(msg_name)) {  // Check for read/post conflicts
        throw flame::exceptions::invalid_operation("read and post to same msg");
      } else if (!MessageBoardManager::GetInstance().BoardExists(msg_name)) {
        throw flame::exceptions::invalid_argument("Unknown message name");
      }
#endif
      acl_post_.insert(msg_name);
    }
    
  private:
    // acl_set_type defined in client.hpp
    
    acl_set_type acl_read_;
    acl_set_type acl_post_;

    inline bool _can_read(const std::string& msg_name) {
      return (acl_read_.find(msg_name) != acl_read_.end())
    }

    inline bool _can_post(const std::string& msg_name) {
      return (acl_post_.find(msg_name) != acl_post_.end())
    }
};

}}  // namespace flame::mb2
#endif  // MB__PROXY_HPP_
