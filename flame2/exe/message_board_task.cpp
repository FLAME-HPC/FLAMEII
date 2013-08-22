/*!
 * \file flame2/exe/message_board_task.cpp
 * \author Shawn Chin
 * \date October 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Task that runs message board functions
 */
#include <string>
#include "flame2/config.hpp"
#include "flame2/mb/message_board_manager.hpp"
#include "flame2/exceptions/all.hpp"
#include "message_board_task.hpp"

namespace flame { namespace exe {

MessageBoardTask::MessageBoardTask(std::string task_name,
                                   std::string msg_name,
                                   Operation op)
    : msg_name_(msg_name), op_(op) {
  if (!flame::mb::MessageBoardManager::GetInstance().BoardExists(msg_name)) {
    throw flame::exceptions::invalid_argument("Unknown message name");
  }
  task_name_ = task_name;
}

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
