/*!
 * \file flame2/mb/proxy.hpp
 * \author Shawn Chin
 * \date November 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Declaration of Proxy
 */
#ifndef MB__PROXY_HPP_
#define MB__PROXY_HPP_
#include <string>
#include <boost/shared_ptr.hpp>
#include "message_board_manager.hpp"
#include "client.hpp"

namespace flame { namespace mb {

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
    //! Datatype for shared pointer to a Client instance
    typedef boost::shared_ptr<Client> client;

    //! Returns a shared pointer to a client instance that has the apropriate
    //! access permissions
    client GetClient(void) {
      return client(new Client(acl_read_, acl_post_));
    }

    /*!
     * \brief Grant read access a specific board to clients of this proxy
     *
     * Throws flame::exceptions::invalid_argument if the provided message name
     * is not valid.
     *
     * Based on the rule that each agent transition function cannot read AND
     * write to the same board, this method throws
     * flame::exceptions::invalid_operation if post access has already been
     * granted for the given message.
     */
    inline void AllowRead(const std::string& msg_name) {
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

    /*!
     * \brief Grant post access a specific board to clients of this proxy
     *
     * Throws flame::exceptions::invalid_argument if the provided message name
     * is not valid.
     *
     * Based on the rule that each agent transition function cannot read AND
     * write to the same board, this method throws
     * flame::exceptions::invalid_operation if read access has already been
     * granted for the given message.
     */
    inline void AllowPost(const std::string& msg_name) {
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

    /*!
     * \brief Const version of the assignment operator
     *
     * This is required because we use boost::container::flat_set for our class
     * members. For details, see http://bit.ly/SZg7dZ
     */
    Proxy& operator=(const Proxy& source) {
      acl_read_ = source.acl_read_;
      acl_post_ = source.acl_post_;
      return *this;
    }

  private:
    // acl_set_type defined in client.hpp
    acl_set_type acl_read_;  //! Set of message names with read access
    acl_set_type acl_post_;  //! Set of message names with post access

    //! Returns true if read access has been granted to given message
    inline bool _can_read(const std::string& msg_name) {
      return (acl_read_.find(msg_name) != acl_read_.end());
    }

    //! Returns true if post access has been granted to given message
    inline bool _can_post(const std::string& msg_name) {
      return (acl_post_.find(msg_name) != acl_post_.end());
    }
};


}}  // namespace flame::mb
#endif  // MB__PROXY_HPP_
