/*!
 * \file FILENAME
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief DESCRIPTION
 */
#ifndef EXE__SCHEDULER_HPP_
#define EXE__SCHEDULER_HPP_
#include <map>
#include <queue>
#include <vector>
#include "boost/bind.hpp"
#include "boost/ptr_container/ptr_vector.hpp"
#include "task_interface.hpp"
#include "task_queue_interface.hpp"

namespace flame { namespace exe {

class TaskQueue;

class Scheduler {
  public:
    typedef size_t QueueId;
    typedef std::map<Task::TaskType, QueueId> RouteMap;

    //! \brief Allocates a task queue of a given type and returns its id
    template <typename T>
    QueueId CreateQueue(size_t slots) {
      TaskQueue* q = new T(slots);
      q->SetCallback(boost::bind(&Scheduler::TaskDoneCallback, this, _1));
      queues_.push_back(q);
      return (queues_.size() - 1);
    }

    //! \brief assigns a task type to a given queue
    void AssignType(QueueId qid, Task::TaskType type);

    //! \brief Specity tasks that can be split
    void SetSplittable(Task::TaskType type);

    //! \brief Callback function used to indicate that a task is completed
    //!
    //! Assigned to each associated task queue for reverse communication
    void TaskDoneCallback(Task::id_type task_id);

    //! \brief Runs a single iteration
    void RunIteration();

  private:
    //! \brief Equeues task within the associated queue
    //!
    //! The queue is selected based on the task type and contents of
    //! route_ map which is populated by AssignType()
    void EnqueueTask(Task::id_type task_id);

    //! \brief Returns true if the given id is a valid queue id
    bool IsValidQueueId(QueueId id);

    boost::mutex doneq_mutex_;  //! mutex to control access to done queue
    boost::condition_variable doneq_cond_;  //! condition variable for done Q
    boost::ptr_vector<TaskQueue> queues_;  //! Collection of task queues
    RouteMap route_;  //! Map which associates task type to task queue

    //! \brief Done queue
    //!
    //! Temporary staging area for tasks that have been completed
    std::vector<Task::id_type> doneq_;
};

}}  // namespace flame::exe
#endif  // EXE__SCHEDULER_HPP_
