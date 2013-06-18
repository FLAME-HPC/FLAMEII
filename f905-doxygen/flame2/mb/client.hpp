/*!
 * \file flame2/mb/client.hpp
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

namespace flame { namespace mb {

// Use flat_set/flat_map instead of set/map. This should yield better
// performance in our case where:
//  * we expect to have a relatively small number of elements
//  * All insertion done before any lookups, and only at the init phase
//  * It is possible to engineer a usage where insertion is in-order
//  * Lookup performance more important than insertion

/*!
 * \brief datatype to store a set of message names for managing access control
 *
 * We Use boost::container::flat_set instead of std::set. This should yield
 * better performance in our case where:
 *
 * - we expect to have a relatively small number of elements
 * - All insertion done before any lookups, and only at the init phase
 * - It is possible to engineer a usage where insertion is in-order
 * - Lookup performance more important than insertion
 *
 * Reference: http://lafstern.org/matt/col1.pdf
 */
typedef std::set<std::string> acl_set_type;

/*!
 * \brief datatype to map a small collection of names to writers
 *
 * For the same reasons stated in the description of acl_set_type, we use
 * boost::container::flat_map instead of std::map.
 */
typedef boost::container::flat_map<std::string,
                                   MessageBoard::writer> writer_map_type;

//! Message board client used by end-user API for all message board interactions
class Client {
  public:
    /*!
     * \brief
     * \param acl_read names of messages client can read
     * \param acl_post names of messages client can post to
     */
    Client(acl_set_type acl_read, acl_set_type acl_post);

    /*!
     * \brief Returns a board writer instance for posting message
     * \param msg_name message name
     * \return board writer
     *
     * Throws flame::exceptions::invalid_argument if given message name is
     * invalid.
     *
     * Throws flame::exceptions::invalid_operation if client does not have
     * permissions to write to given message
     */
    MessageBoard::writer GetBoardWriter(const std::string& msg_name);

    /*!
     * \brief Returns an iterator to read messages from the given message board
     * \param msg_name message name
     * \return message iterator
     *
     * Throws flame::exceptions::invalid_argument if given message name is
     * invalid.
     *
     * Throws flame::exceptions::invalid_operation if client does not have
     * permissions to read given message.
     */
    MessageBoard::iterator GetMessages(const std::string& msg_name);

    // TODO: GetMessages(msg_name, query);  // when filtering enabled

  private:
    writer_map_type writers_;  //!< Cache of writers for each allowed message
    acl_set_type acl_read_;  //!< Names of messages client can read from
};

}}  // namespace flame::mb
#endif  // MB__CLIENT_HPP_
