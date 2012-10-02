/*!
 * \file src/mb/message_board_manager.cpp
 * \author Shawn Chin
 * \date October 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief MessageBoardManager: management and storage class for message boards
 */
#include <utility>
#include "exceptions/all.hpp"
#include "message_board_manager.hpp"

namespace flame { namespace mb {

/*!
 * \brief Registers a new message
 * \param[in] msg_name Name of message to register
 *
 * If an existing message is provided, the registered board of with the same
 * name will replace and a flame::exception::logic_error is thrown.
 */
void MessageBoardManager::RegisterMessage(std::string msg_name) {
  std::pair<BoardMap::iterator, bool> ret;
  ret = board_map_.insert(msg_name, new MessageBoard(msg_name));
  if (!ret.second) {  // replacement instead of insertion
    throw flame::exceptions::logic_error("Name exists. Prev board replaced");
  }
}

/*!
 * \brief Internal routine to retrieve a reference to a registered board
 * \param[in] msg_name Name of message to retrieve board of
 * \return Reference to a MessageBoard instance
 *
 * Throws flame::exception::invalid_argument if msg_name is invalid.
 */
MessageBoard& MessageBoardManager::GetMessageBoard(std::string msg_name) {
  try {
    return board_map_.at(msg_name);
  } catch(const boost::bad_ptr_container_operation& E) {
    throw flame::exceptions::invalid_argument("Unknown message name");
  }
}


//! Creates and returns a handle to a board writer
MessageBoard::Writer MessageBoardManager::GetBoardWriter(
                                                  const std::string& msg_name) {
  return GetMessageBoard(msg_name).GetBoardWriter();
}

//! Creates and returns a handle to a message iterator
MessageBoard::Iterator MessageBoardManager::GetMessageIterator(
                                                  const std::string& msg_name) {
  return GetMessageBoard(msg_name).GetMessageIterator();
}

//! Performs sync operation on board
void MessageBoardManager::Sync(const std::string& msg_name) {
  return GetMessageBoard(msg_name).Sync();
}

//! Returns the number of messages that have been synched
size_t MessageBoardManager::GetCount(const std::string& msg_name) {
  return GetMessageBoard(msg_name).GetCount();
}


#ifdef TESTBUILD
void MessageBoardManager::Reset() {
  board_map_.clear();
}
#endif
}}  // namespace flame::mb
