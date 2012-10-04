/*!
 * \file src/exe/message_board_task.cpp
 * \author Shawn Chin
 * \date October 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Task that runs message board functions
 */
#include <string>
#include "mb/message_board_manager.hpp"
#include "exceptions/all.hpp"
#include "message_board_task.hpp"

namespace flame { namespace exe {

/*!
 * \brief constructor
 *
 * Initialies msg_name_ and op_, and checks that the specified board exists.
 */
MessageBoardTask::MessageBoardTask(std::string task_name,
                     std::string msg_name,
                     Operation op)
    : msg_name_(msg_name), op_(op) {
  if (!flame::mb::MessageBoardManager::GetInstance().BoardExists(msg_name)) {
    throw flame::exceptions::invalid_argument("Unknown message name");
  }
}

/*!
 * \brief Operations to perform when task is executed
 *
 * Runs board operations defined by op_ on named message board.
 */
void MessageBoardTask::Run(void) {
  switch (op_) {
    case OP_SYNC:
      flame::mb::MessageBoardManager::GetInstance().Sync(msg_name_);
      break;
    case OP_CLEAR:
      flame::mb::MessageBoardManager::GetInstance().Clear(msg_name_);
      break;
    default:
      throw flame::exceptions::not_implemented("Operation not implemented");
  }
}
}}  // namespace flame::exe
