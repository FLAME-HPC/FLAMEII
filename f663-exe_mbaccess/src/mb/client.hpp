/*!
 * \file src/mb/client.cpp
 * \author Shawn Chin
 * \date October 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Declaration of Client
 */
#ifndef MB__PROXY_HPP_
#define MB__PROXY_HPP_
#include <set>
#include "mb_common.hpp"

namespace flame { namespace mb {
/*!
 * \brief Proxy object for Clients to access message board
 *
 * Handles access control as well as caches message writers.
 */
class Client
{
  public:
    //! Allows read access from a specific message board
    void AllowRead(const std::string& msg_name);
    //! Allows post access to a specific message board
    void AllowPost(const std::string& msg_name);
    //! Returns a handle to a new message which which can post to a board
    MessageHandle NewMessage(const std::string& msg_name);
    //! Returns a handle to a board writer
    BoardWriterHandle GetWriter(const std::string& msg_name);
    //! Returns an iterator to read all messages in the board
    MessageIteratorHandle GetMessages(const std::string& msg_name);

    /* // (When filtering is enabled)
    //! Queries the board
    MessageIteratorHandle GetMessage(const std::string& msg_name,
                                     const std::string& query);
    */
  protected:
  private:
    typedef std::map<std::string, BoardWriterHandle> WriterMap;
    typedef std::set<std::string> StringSet;

    //! Defines set of messages this instance can read from
    StringSet acl_read_;
    //! Cache of writer handles. Keys also act as acl_post_
    WriterMap writer_cache_;

    //! Checks if read access is set for a message
    bool _can_read(const std::string& msg_name);

    //! Checks if post access is set for a message
    bool _can_post(const std::string& msg_name);
};
}}  // namespace flame::mb
#endif  // MB__PROXY_HPP_
