/*!
 * \file flame2/exe/task_interface.hpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Interface class for tasks than can be executed by workers
 */
#ifndef EXE__RUNNABLE_TASK_HPP_
#define EXE__RUNNABLE_TASK_HPP_
#include <limits>
#include <string>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include "flame2/mem/memory_iterator.hpp"
#include "flame2/api/agent_api.hpp"
#include "flame2/mb/proxy.hpp"

namespace flame { namespace exe {

//! \brief wrapper class for agent transition functions
typedef boost::function<
          flame::api::FLAME_AgentFunctionReturnType
          (flame::api::FLAME_AgentFunctionParamType)> TaskFunction;

class TaskSplitter;  // forward declaration

//! Shared pointer to TaskSplitter instance
typedef boost::shared_ptr<TaskSplitter> TaskSplitterHandle;

//! Abstract class for Task object
class Task {
  public:
    typedef size_t id_type;  //!< datatype for task id
    //! Shorthand for message board Client
    typedef flame::mb::Proxy::client MessageBoardClient;
    //! Shared pointer to Task instance
    typedef boost::shared_ptr<Task> Handle;

    //! Identifier for different task types
    enum TaskType {
      AGENT_FUNCTION,
      IO_FUNCTION,
      MB_FUNCTION
    };

    virtual ~Task() {}

    //! Runs the task
    virtual void Run() = 0;

    //! Returns the task type
    virtual TaskType get_task_type() const = 0;

    /*! 
     * \brief Returns a memory iterator for this task
     * \return shared pointer to message iterator
     *
     * \todo Perhaps move this into AgentTask?
     */
    virtual flame::mem::MemoryIteratorPtr GetMemoryIterator() const = 0;

    /*!
     * \brief Defines access control to agent memory variables
     * \param var_name variable name
     * \param writeable is variable writeable (default = false)
     * 
     * \todo Perhaps move this into AgentTask?
     */
    virtual void AllowAccess(const std::string& var_name,
                             bool writeable = false) = 0;

    /*! 
     * \brief Returns a task splitter which allows task to be exected in segments
     * \param max_tasks maximum number of tasks resulting from this split
     * \param min_task_size minimum size of task after split
     * \return handle to TaskSplitter instance
     *
     * This Should return a null handle if task cannot be split.
     */
    virtual TaskSplitterHandle SplitTask(size_t max_tasks,
                                         size_t min_task_size) = 0;

    /*! 
     * \brief Adds read access to message board
     * \param msg_name message name
     *
     * \todo Perhaps move this into AgentTask?
     */
    void AllowMessageRead(const std::string& msg_name) {
      if (!mb_proxy_) {
        mb_proxy_ = ProxyHandle(new flame::mb::Proxy);
      }
      mb_proxy_->AllowRead(msg_name);
    }

    /*! 
     * \brief Adds post access to message board
     * \param msg_name message name
     *
     * \todo Perhaps move this into AgentTask?
     */
    void AllowMessagePost(const std::string& msg_name) {
      if (!mb_proxy_) {
        mb_proxy_ = ProxyHandle(new flame::mb::Proxy);
      }
      mb_proxy_->AllowPost(msg_name);
    }

    /*! 
     * \brief Returns message board access client
     * \return client than can be used to access message board
     *
     * \todo Perhaps this into AgentTask?
     */
    MessageBoardClient GetMessageBoardClient(void) {
      if (!mb_proxy_) {
        mb_proxy_ = ProxyHandle(new flame::mb::Proxy);
      }
      return mb_proxy_->GetClient();
    }

    //! Returns the task id
    id_type get_task_id() const { return task_id_; }

    //! Sets the task id
    void set_task_id(id_type id) { task_id_ = id; }

    //! Retuqrns the task name
    std::string get_task_name() const { return task_name_; }

    //! Returns true if the given task id is a termination signal
    inline static bool IsTermTask(id_type task_id) {
      return (task_id == GetTermTaskId());
    }

    //! Returns the task id that represent the termination signal
    inline static id_type GetTermTaskId() {
      return std::numeric_limits<id_type>::max();
    }

  protected:
    //! Shared pointer to message board Proxy
    typedef boost::shared_ptr<flame::mb::Proxy> ProxyHandle;

    id_type task_id_;  //!< Task id
    std::string task_name_;  //!< Task name
    ProxyHandle mb_proxy_;  //!< message board proxy
};



}}  // namespace flame::exe
#endif  // EXE__RUNNABLE_TASK_HPP_

