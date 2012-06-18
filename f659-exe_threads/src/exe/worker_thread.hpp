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
    void join();
    void Init();
    void ProcessQueue();
    void RunTask(Task::id_type task_id);
    void RegisterCompletedTask(Task::id_type task_id);

  private:
    Task::id_type WaitForNextTask();
    boost::thread thread_;
    TaskQueue* tq_;
};

}}  // namespace flame::exe
#endif // EXE__WORKER_THREAD_HPP_
