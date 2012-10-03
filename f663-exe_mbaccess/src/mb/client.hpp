/*!
 * \file FILENAME
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief DESCRIPTION
 */
#ifndef MB__CLIENT_HPP_
#define MB__CLIENT_HPP_
#include "mb_common.hpp"
#include "proxy.hpp"

namespace flame { namespace mb {
class Client
{
  public:
    friend class Proxy;

    //! Returns an iterator to read all messages in the board
    MessageIteratorHandle GetMessages(const std::string& msg_name);

    //! Returns a handle to a board writer
    BoardWriterHandle GetWriter(const std::string& msg_name);

    //! Returns a handle to a new message which which can post to a board
    MessageHandle NewMessage(const std::string& msg_name);

    /* // (When filtering is enabled)
    //! Queries the board
    MessageIteratorHandle GetMessage(const std::string& msg_name,
                                     const std::string& query);
    */
  protected:
    // constructor limited to mb::Proxy
    Client(const Proxy::StringSet& acl_read, const Proxy::StringSet& acl_post);

  private:
    typedef std::map<std::string, BoardWriterHandle> WriterMap;

    Proxy::StringSet acl_read_; //! msg with read access
    Proxy::StringSet acl_post_; //! msg with post acess
    WriterMap writer_cache_; //! Cache of writer handles

    //! Checks if read access is set for a message
    bool _can_read(const std::string& msg_name);

    //! Checks if post access is set for a message
    bool _can_post(const std::string& msg_name);

};

}}  // namespace flame::mb
#endif // MB__CLIENT_HPP_
