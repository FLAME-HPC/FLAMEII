/*!
 * \file flame2/mb2/message_board_manager.hpp
 * \author Shawn Chin
 * \date November 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief MessageBoardManager: management and storage class for message boards
 */
#ifndef MB__MESSAGE_BOARD_MANAGER_HPP_
#define MB__MESSAGE_BOARD_MANAGER_HPP_
#include <boost/ptr_container/ptr_map.hpp>
#include "message_board.hpp"

namespace flame { namespace mb2 {

class MessageBoardManager {
  public:
  
    //! Class method to retrive a reference to the singleton object
    static MessageBoardManager& GetInstance() {
      static MessageBoardManager instance;
      return instance;
    }

    template <typename T>
    void RegisterMessage(std::string msg_name) {
      std::pair<MessageBoard::iterator, bool> ret;
      MessageBoard *board = MessageBoard::create<T>(msg_name);
      ret = map_.insert(msg_name, board);
      if (!ret.second) {
        delete board;
        throw flame::exceptions::logic_error("Message with that name exists");
      }
    }

    inline MessageBoard::writer GetBoardWriter(const std::string& msg_name) {
      return _GetMessageBoard(msg_name).GetBoardWriter();
    }
    
    inline MessageBoard::iterator GetMessages(const std::string& msg_name) {
      return _GetMessageBoard(msg_name).GetMessages();
    }
    
    inline void Sync(const std::string& msg_name) {
      _GetMessageBoard(msg_name).Sync();
    }
    
    inline void Clear(const std::string& msg_name) {
      _GetMessageBoard(msg_name).Sync();
    }
    
    inline size_t GetCount(const std::string& msg_name) {
      return _GetMessageBoard(msg_name).GetCount();
    }
    
    inline bool BoardExists(const std::string& msg_name) const {
      return (map_.find(msg_name) != map_.end());
    }

#ifdef TESTBUILD
    //! Delete all registered agents and vars
    void Reset(void) {
      map_.clear();
    }
#endif

  private:
    typedef boost::ptr_map<std::string, MessageBoard> board_map;

    board_map map_;
    
    //! This is a singleton class. Disable manual instantiation
    MessageBoardManager() {}
    //! This is a singleton class. Disable copy constructor
    MessageBoardManager(const MessageBoardManager&);
    //! This is a singleton class. Disable assignment operation
    void operator=(const MessageBoardManager&);

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
