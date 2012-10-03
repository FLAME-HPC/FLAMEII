#ifndef EXE__MESSAGE_BOARD_TASK_HPP_
#define EXE__MESSAGE_BOARD_TASK_HPP_
#include <string>
#include "exceptions/all.hpp"
#include "mem/memory_iterator.hpp"  // only for unnecessary GetMemoryIterator
#include "task_interface.hpp"

namespace flame { namespace exe {

class MessageBoardTask : public Task {
  friend class TaskManager;
  public:
    enum Operation {
      OP_SYNC,
      OP_CLEAR
    };

    //! Returns the task type
    TaskType get_task_type(void) const { return Task::MB_FUNCTION; }

    //! Returns a memory iterator for this task
    flame::mem::MemoryIteratorPtr GetMemoryIterator(void) const {
      throw flame::exceptions::logic_error("method not applicable");
    }

    //! Enable access to a specific agent var (not applicable)
    void AllowAccess(const std::string& var_name, bool writeable = false) {
      throw flame::exceptions::logic_error("method not applicable");
    }

    //! Runs the task
    void Run(void);

  protected:
    MessageBoardTask(std::string task_name,
                     std::string msg_name,
                     Operation op);

  private:
    std::string msg_name_;
    Operation op_;
};

}}  // namespace flame::exe
#endif // EXE__MESSAGE_BOARD_TASK_HPP_
