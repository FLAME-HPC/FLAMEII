/*!
 * \file flame2/exe/splitting_fifo_task_queue.hpp
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

//!
//!
//! \todo SplittingPriorityTaskQueue - a queue that schedules based on priority
//!       instead of FIFO. Use either std::priority_queue or a custom one based
//!       on a binary heap.
//!
//! \todo Improve load balance by trailing off splits with smaller chunks (for FIFO)
//!       or support subsplitting if available threads > queued tasks (for priority queue)
//!
class SplittingFIFOTaskQueue : public TaskQueue {
  public:
    //! Default value for minimum vector size for each split task
    static size_t const DEFAULT_MIN_VECTOR_SIZE = 50;

    /*!
     * \brief Constructor
     *
     * Sets the max_split_ to the number of slots and
     * min_vector_size to DEFAULT_MIN_VECTOR_SIZE.
     *
     * Throws flame::exceptions::invalid_argument if an invalid value for slot is
     * given.
     */
    explicit SplittingFIFOTaskQueue(size_t slots);
    /*!
     * \brief Destructor
     *
     * Enqueue a termination tasks for each worker thread and wait for them to
     * complete.
     */
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

    /*!
     * \brief Adds a task to the queue
     *
     * This method is meant to be called by the Scheduler
     *
     * Appends the tasks to the FIFO queue. If the task is a splittable task,
     * attempt the split the task. If the split is successful, add the resulting
     * TaskSplitter instance to the split_map_ with the task_id as key.
     */
    void Enqueue(Task::id_type task_id);

    /*!
     * \brief Indicate that a task has been completed
     *
     * This method is meant to be called by a Worker Thread.
     *
     * If the task does not have an entry in split_map_, run the callback function
     * and return.
     *
     * If the task has an entry in split_map_, retrieve the task splitter instance
     * and indicate that a task segment has been completed. If there are still
     * pending or running subtasks, do nothing else.
     *
     * If all subtasks have been completed, remove the task from the split_map_ and
     * run the scheduler callback function to signal that the whole task has been
     * completed.
     */
    void TaskDone(Task::id_type task_id);

    /*!
     * \brief Returns the next available task.
     *
     * If there are none available, the calling thread will be blocked
     *
     * This method is meant to be called by a Worker Thread.
     *
     * If the task is split (has an entry in split_map_), only pop the task from
     * the FIFO queue when all subtasks have been assigned.
     */
    Task::id_type GetNextTask();

    //! \brief Returns true if the queue is empty
    bool empty() const;

    /*!
     * \brief Returns a task reference given a task id
     *
     * This is used by the worker thread to get at the actual task.
     *
     * If the task is a split task (has an entry in split_map_) we return a
     * reference to a subtask. Otherwise, request for the task from the
     * Task Manager.
     *
     * Overload so we can intercept calls
     */
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
