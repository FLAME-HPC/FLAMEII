/*!
 * \file flame2/exe/message_board_task.hpp
 * \author Shawn Chin
 * \date October 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Task that runs message board functions
 */
#ifndef EXE__MESSAGE_BOARD_TASK_HPP_
#define EXE__MESSAGE_BOARD_TASK_HPP_
#include <string>
#include "flame2/exceptions/all.hpp"
#include "task_interface.hpp"

namespace flame { namespace exe {

class MessageBoardTask : public Task {
  friend class TaskManager;
  public:
    //! Allowed operation types
    enum Operation {
      OP_SYNC,
      OP_CLEAR
    };

    //! Returns the task type
    TaskType get_task_type(void) const { return Task::MB_FUNCTION; }

    //! Returns a memory iterator for this task
    flame::mem::MemoryIteratorPtr GetMemoryIterator(void) const {
      throw flame::exceptions::not_implemented("method not applicable");
    }

    //! Enable access to a specific agent var (not applicable)
    void AllowAccess(const std::string& /*var_name*/, bool /*writeable*/) {
      throw flame::exceptions::not_implemented("method not applicable");
    }

    //! Returns a task splitter (not supported by MB task)
    TaskSplitterHandle SplitTask(size_t /*max_tasks*/,
                                 size_t /*min_task_size*/) {
      throw flame::exceptions::not_implemented("method not applicable");
    }

    /*!
     * \brief Operations to perform when task is executed
     *
     * Runs board operations defined by op_ on named message board.
     */
    void Run(void);

  protected:
    /*!
     * \brief Constructor (Limited to TaskManager)
     *
     * Initialies msg_name_ and op_, and checks that the specified board exists.
     */
    MessageBoardTask(std::string task_name,
                     std::string msg_name,
                     Operation op);

  private:
    std::string msg_name_;  //! Message name
    Operation op_;  //! Opearation to perform
};

}}  // namespace flame::exe
#endif  // EXE__MESSAGE_BOARD_TASK_HPP_
