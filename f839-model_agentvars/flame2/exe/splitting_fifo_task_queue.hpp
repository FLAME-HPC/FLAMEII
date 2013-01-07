/*!
 * \file flame2/exe/splitting_fifo_task_queue.cpp
 * \author Shawn Chin
 * \date Oct 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Declaration of SplittingFIFOTaskQueue
 */
#ifndef EXE__SPLITTING_FIFO_TASK_QUEUE_HPP_
#define EXE__SPLITTING_FIFO_TASK_QUEUE_HPP_
#include <map>
#include <set>
#include <queue>
#include <boost/ptr_container/ptr_vector.hpp>
#include "task_queue_interface.hpp"
#include "worker_thread.hpp"
#include "task_splitter.hpp"

namespace flame { namespace exe {

class SplittingFIFOTaskQueue : public TaskQueue {
  public:
    //! Default value for minimum vector size for each split task
    static size_t const DEFAULT_MIN_VECTOR_SIZE = 50;

    explicit SplittingFIFOTaskQueue(size_t slots);
    ~SplittingFIFOTaskQueue();

    //! Specify tasks than can be split
    void SetSplittable(Task::TaskType task_type);

    //! Specify maximum splits per task
    void SetMaxTasksPerSplit(size_t max_tasks_per_split);

    //! Returns maximum splits per task
    size_t GetMaxTasksPerSplit(void) const;

    //! Specify minimum vector size after split
    void SetMinVectorSize(size_t min_vector_size);

    //! Returns minimum vector size after split
    size_t GetMinVectorSize(void) const;

    //! \brief Adds a task to the queue
    //!
    //! This method is meant to be called by the Scheduler
    void Enqueue(Task::id_type task_id);

    //! \brief Indicate that a task has been completed
    //!
    //! This method is meant to be called by a Worker Thread
    void TaskDone(Task::id_type task_id);

    //! \brief Returns the next available task.
    //! If there are none available, the calling thread will be blocked
    //!
    //! This method is meant to be called by a Worker Thread
    Task::id_type GetNextTask();

    //! \brief Returns true if the queue is empty
    bool empty() const;

    //! Returns a task reference given a task id
    //! Overload so we can intercept calls
    Task& GetTaskById(Task::id_type task_id);

  private:
    typedef boost::ptr_vector<WorkerThread> WorkerVector;
    typedef std::map<Task::id_type, TaskSplitterHandle> SplitMap;

    std::set<Task::TaskType> splittable_;  //! Types of task to split
    std::queue<Task::id_type> queue_;  //! FIFO task queue
    WorkerVector workers_;  //! Collection of worker threads
    SplitMap split_map_;  //! Collection of tasks that have been split
    size_t slots_;  //! Number of processing slots (worker threads)
    size_t max_splits_;  //! Maximum number of tasks per split
    size_t min_vector_size_;  //! Minimum vector size after split
};

}}  // namespace flame::exe
#endif  // EXE__SPLITTING_FIFO_TASK_QUEUE_HPP_
