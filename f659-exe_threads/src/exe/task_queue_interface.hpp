/*!
 * \file src/exe/task_queue_interface.hpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Interface class for Task Queues
 */
#ifndef EXE__TASK_QUEUE_INTERFACE_HPP_
#define EXE__TASK_QUEUE_INTERFACE_HPP_
#include "task_interface.hpp"

namespace flame { namespace exe {

class TaskQueueInterface {

  public:
    virtual ~TaskQueueInterface() {}
    virtual Task::TaskType GetTaskType() const = 0;
    virtual void PushTask(Task::id_type task_id) = 0;
};

}}  // namespace flame::exe
#endif  // EXE__TASK_QUEUE_INTERFACE_HPP_
