#include "mb/message_board_manager.hpp"
#include "exceptions/all.hpp"
#include "message_board_task.hpp"

namespace flame { namespace exe {

MessageBoardTask::MessageBoardTask(std::string task_name,
                     std::string msg_name,
                     Operation op)
    : msg_name_(msg_name), op_(op) {
  if (!flame::mb::MessageBoardManager::GetInstance().BoardExists(msg_name)) {
    throw flame::exceptions::invalid_argument("Unknown message name");
  }
}

void MessageBoardTask::Run(void) {
  switch(op_) {
    case OP_SYNC:
      flame::mb::MessageBoardManager::GetInstance().Sync(msg_name_);
      break;
    default:
      throw flame::exceptions::not_implemented("Operation not implemented");
  }
}
}}  // namespace flame::exe
