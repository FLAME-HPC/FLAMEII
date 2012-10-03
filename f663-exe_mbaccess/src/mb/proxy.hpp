/*!
 * \file src/mb/proxy.cpp
 * \author Shawn Chin
 * \date October 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Declaration of Proxy
 */
#ifndef MB__PROXY_HPP_
#define MB__PROXY_HPP_
#include <set>
#include "mb_common.hpp"

namespace flame { namespace mb {
/*!
 * \brief Proxy object to produce Clients that can access message board
 */
class Proxy
{
  public:
    typedef std::set<std::string> StringSet;

    //! Allows read access from a specific message board
    void AllowRead(const std::string& msg_name);

    //! Allows post access to a specific message board
    void AllowPost(const std::string& msg_name);

    //! Returns a Client than can be used to access boards
    ClientHandle GetClient(void);

  private:
    //! Defines set of messages that can be read from
    StringSet acl_read_;

    //! Defines set of messages that can be posted to
    StringSet acl_post_;

    //! Checks if read access is set for a message
    bool _can_read(const std::string& msg_name);

    //! Checks if post access is set for a message
    bool _can_post(const std::string& msg_name);
};
}}  // namespace flame::mb
#endif  // MB__PROXY_HPP_
