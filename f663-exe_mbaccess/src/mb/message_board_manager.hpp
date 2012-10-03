/*!
 * \file src/mb/message_board_manager.hpp
 * \author Shawn Chin
 * \date October 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief MessageBoardManager: management and storage class for message boards
 */
#ifndef MB__MESSAGE_BOARD_MANAGER_HPP_
#define MB__MESSAGE_BOARD_MANAGER_HPP_
#include <string>
#include "boost/ptr_container/ptr_map.hpp"
#include "message_board.hpp"

namespace flame { namespace mb {
class MessageBoardManager {
  public:
    //! Map type to store and manage lifetime of message board instances
    typedef boost::ptr_map<std::string, MessageBoard> BoardMap;

    //! Class method to retrive a reference to the singleton object
    static MessageBoardManager& GetInstance() {
      static MessageBoardManager instance;
      return instance;
    }

    //! Register a new message
    void RegisterMessage(std::string msg_name);

    //! Register a message variable of a certain type given a known message
    template <typename T>
    void RegisterMessageVar(const std::string& msg_name, std::string var_name) {
      GetMessageBoard(msg_name).RegisterVar<T>(var_name);
    }

    //! Registers a list of memory vars or a certain type for a given agent
    template <typename T>
    void RegisterMessageVar(const std::string& msg_name,
                            const std::vector<std::string>& var_names) {
      MessageBoard& board = GetMessageBoard(msg_name);
      std::vector<std::string>::const_iterator it;
      for (it = var_names.begin(); it != var_names.end(); ++it) {
        board.RegisterVar<T>(*it);
      }
    }

    //! Creates and returns a handle to a board writer
    MessageBoard::Writer GetBoardWriter(const std::string& msg_name);

    //! Creates and returns a handle to a message iterator
    MessageBoard::Iterator GetMessageIterator(const std::string& msg_name);

    //! Performs sync operation on board
    void Sync(const std::string& msg_name);

    //! Returns the number of messages that have been synched
    size_t GetCount(const std::string& msg_name);

    //! Determines if a specific board has been registered
    bool BoardExists(const std::string& msg_name);

#ifdef TESTBUILD
    //! Delete all registered agents and vars
    void Reset();
#endif

  private:
    //! Map used to associate a message name with a MessageBoard object
    BoardMap board_map_;

    //! This is a singleton class. Disable manual instantiation
    MessageBoardManager() {}
    //! This is a singleton class. Disable copy constructor
    MessageBoardManager(const MessageBoardManager&);
    //! This is a singleton class. Disable assignment operation
    void operator=(const MessageBoardManager&);

    //! Returns a refernce to a MessageBoard object given the message name
    MessageBoard& GetMessageBoard(std::string msg_name);
};

}}  // namespace flame::mb
#endif  // MB__MESSAGE_BOARD_MANAGER_HPP_
