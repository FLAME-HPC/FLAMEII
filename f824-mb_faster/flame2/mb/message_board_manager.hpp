/*!
 * \file flame2/mb/message_board_manager.hpp
 * \author Shawn Chin
 * \date November 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief MessageBoardManager: management and storage class for message boards
 */
#ifndef MB__MESSAGE_BOARD_MANAGER_HPP_
#define MB__MESSAGE_BOARD_MANAGER_HPP_
#include <string>
#include <utility>
#include <boost/ptr_container/ptr_map.hpp>
#include "message_board.hpp"

namespace flame { namespace mb {

//! Singleton object to store and manage collection of message boards
class MessageBoardManager {
  public:
    //! Class method to retrive a reference to the singleton object
    static MessageBoardManager& GetInstance() {
      static MessageBoardManager instance;
      return instance;
    }

    /*!
     * \brief Creates and stores a message board of a given name and type
     *
     * Throws flame::exceptions::logic_error if a name that already exists
     * is provided.
     *
     * \note All types used must support the \c << operator. User defined types
     * (such as structs) must implement the \c operator<< method.
     */
    template <typename T>
    void RegisterMessage(std::string msg_name) {
      std::pair<board_map::iterator, bool> ret;
      MessageBoard *board = MessageBoard::create<T>(msg_name);
      ret = map_.insert(msg_name, board);
      if (!ret.second) {
        throw flame::exceptions::logic_error("Message with that name exists");
      }
    }

    /*!
     * \brief Returns a shared pointer to BoardWriter instance for board
     *
     * Throws flame::exceptions::invalid_argument if unknown message name
     * is provided.
     */
    inline MessageBoard::writer GetBoardWriter(const std::string& msg_name) {
      return _GetMessageBoard(msg_name).GetBoardWriter();
    }

    /*!
     * \brief Returns a shared pointer to MessageIterator instance for board
     *
     * Throws flame::exceptions::invalid_argument if unknown message name
     * is provided.
     */
    inline MessageBoard::iterator GetMessages(const std::string& msg_name) {
      return _GetMessageBoard(msg_name).GetMessages();
    }

    /*!
     * \brief Synchronises the specified board
     *
     * Throws flame::exceptions::invalid_argument if unknown message name
     * is provided.
     */
    inline void Sync(const std::string& msg_name) {
      _GetMessageBoard(msg_name).Sync();
    }

    /*!
     * \brief Clears the specified board
     *
     * Throws flame::exceptions::invalid_argument if unknown message name
     * is provided.
     */
    inline void Clear(const std::string& msg_name) {
      _GetMessageBoard(msg_name).Clear();
    }

    /*!
     * \brief Returns the number of messages in the specified board
     *
     * Throws flame::exceptions::invalid_argument if unknown message name
     * is provided.
     */
    inline size_t GetCount(const std::string& msg_name) {
      return _GetMessageBoard(msg_name).GetCount();
    }

    //! Returns true if specified name is a registered message name
    inline bool BoardExists(const std::string& msg_name) const {
      return (map_.find(msg_name) != map_.end());
    }

#ifdef TESTBUILD
    //! Delete all registered agents (TEST BUILD Only)
    void Reset(void) {
      map_.clear();
    }
#endif

  private:
    //! Datatype to map message name to board instances
    typedef boost::ptr_map<std::string, MessageBoard> board_map;

    board_map map_;  //! map of message board instances

    //! This is a singleton class. Disable manual instantiation
    MessageBoardManager() {}
    //! This is a singleton class. Disable copy constructor
    MessageBoardManager(const MessageBoardManager&);
    //! This is a singleton class. Disable assignment operation
    void operator=(const MessageBoardManager&);

    //! Utility routine to return a reference to a board give a message name
    inline MessageBoard& _GetMessageBoard(const std::string& msg_name) {
      try {
        return map_.at(msg_name);
      } catch(const boost::bad_ptr_container_operation& E) {
        throw flame::exceptions::invalid_argument("Unknown message name");
      }
    }
};
}}  // namespace flame::mb
#endif  // MB__MESSAGE_BOARD_MANAGER_HPP_
