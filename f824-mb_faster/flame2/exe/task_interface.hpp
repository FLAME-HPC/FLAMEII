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

typedef boost::function<flame::api::AgentFuncRetType
                        (flame::api::AgentFuncParamType)> TaskFunction;

class TaskSplitter;  // forward declaration
typedef boost::shared_ptr<TaskSplitter> TaskSplitterHandle;

class Task {
  public:
    typedef size_t id_type;
    typedef flame::mb::Proxy::client MessageBoardClient;
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

    //! Returns a memory iterator for this task
    //! TODO(lsc) Move this into AgentTask?
    virtual flame::mem::MemoryIteratorPtr GetMemoryIterator() const = 0;

    //! Defines access control to agent memory variables
    //! TODO(lsc) Move this into AgentTask?
    virtual void AllowAccess(const std::string& var_name,
                             bool writeable = false) = 0;

    //! Returns a task splitter which allows task to be exected in segments
    //! Should return null handle if cannot be split.
    virtual TaskSplitterHandle SplitTask(size_t max_tasks,
                                         size_t min_task_size) = 0;

    //! Adds read access to message board
    //! TODO(lsc) Move this into AgentTask?
    void AllowMessageRead(const std::string& msg_name) {
      if (!mb_proxy_) {
        mb_proxy_ = ProxyHandle(new flame::mb::Proxy);
      }
      mb_proxy_->AllowRead(msg_name);
    }

    //! Adds post access to message board
    //! TODO(lsc) Move this into AgentTask?
    void AllowMessagePost(const std::string& msg_name) {
      if (!mb_proxy_) {
        mb_proxy_ = ProxyHandle(new flame::mb::Proxy);
      }
      mb_proxy_->AllowPost(msg_name);
    }

    //! Returns message board access client
    //! TODO(lsc) Move this into AgentTask?
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

    //! Returns the task name
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
    typedef boost::shared_ptr<flame::mb::Proxy> ProxyHandle;

    id_type task_id_;
    std::string task_name_;
    ProxyHandle mb_proxy_;
};



}}  // namespace flame::exe
#endif  // EXE__RUNNABLE_TASK_HPP_
