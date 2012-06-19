/*!
 * \file src/exe/worker_thread.hpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Basic worker thread
 */
#ifndef EXE__WORKER_THREAD_HPP_
#define EXE__WORKER_THREAD_HPP_
#include "boost/thread.hpp"
#include "task_interface.hpp"

namespace flame { namespace exe {

class TaskQueue;

class WorkerThread {
  public:
    explicit WorkerThread(TaskQueue* taskqueue_ptr);

    //! Waits for all threads to complete
    void join();

    //! Starts the thread
    void Init();

    //! Business logic for the thread
    void ProcessQueue();

    //! Runs a given task
    void RunTask(Task::id_type task_id);

  private:
    boost::thread thread_;
    TaskQueue* tq_;
};

}}  // namespace flame::exe
#endif  // EXE__WORKER_THREAD_HPP_
