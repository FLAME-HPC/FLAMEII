/*!
 * \file flame2/exe/worker_thread.hpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Basic worker thread
 */
#ifndef EXE__WORKER_THREAD_HPP_
#define EXE__WORKER_THREAD_HPP_
#include <boost/thread.hpp>
#include "task_interface.hpp"

namespace flame { namespace exe {

class TaskQueue;

//!
//! \todo Figure out how to propagate exceptions back to the main thread so we
//!       can handle exceptions raised by API calls more elegantly.
//!
//! \todo Provide option to explicitly set thread affinity. At present, it
//!       appears that on some platforms all threads get assigned to the same
//!       CPU core.
class WorkerThread {
  public:
    /*!
     * \brief Constructor
     * \param taskqueue_ptr Pointer to parent task queue
     */
    explicit WorkerThread(TaskQueue* taskqueue_ptr);

    //! Waits for thread to complete
    void join();

    //! Starts the thread
    void Init();

    /*!
     * \brief Business logic for the thread
     *
     * Retrieves tasks from the parent queue and runs them. Continue until a
     * Termination task is issued.
     */
    void ProcessQueue();

    //! Runs a given task
    void RunTask(Task::id_type task_id);

  private:
    boost::thread thread_;  //!< Thread instance
    TaskQueue* tq_;  //!< Pointer to parent task queue
};

}}  // namespace flame::exe
#endif  // EXE__WORKER_THREAD_HPP_
