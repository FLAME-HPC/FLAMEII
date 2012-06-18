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

    Scheduler();
    ~Scheduler();

    template <typename T>
    QueueId CreateQueue(size_t slots) {
      TaskQueue* q = new T(slots);
      q->SetCallback(boost::bind(&Scheduler::TaskDoneCallback, this, _1));
      queues_.push_back(q);
      return (queues_.size() - 1);
    }

    void TaskDoneCallback(Task::id_type task_id);
    void AssignType(QueueId qid, Task::TaskType type);
    void RunIteration();

  private:
    void EnqueueTask(Task::id_type task_id);
    bool IsValidQueueId(QueueId id);

    boost::mutex doneq_mutex_;
    boost::condition_variable doneq_cond_;
    boost::ptr_vector<TaskQueue> queues_;
    RouteMap route_;



    std::vector<Task::id_type> doneq_;
};

}}  // namespace flame::exe
#endif //  EXE__SCHEDULER_HPP_
