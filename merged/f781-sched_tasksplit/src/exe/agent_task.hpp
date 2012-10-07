/*!
 * \file src/exe/agent_task.hpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Task that runs agent functions
 */
#ifndef EXE__AGENT_TASK_HPP_
#define EXE__AGENT_TASK_HPP_
#include <string>
#include <utility>
#include <map>
#include <set>
#include "mem/memory_manager.hpp"
#include "mem/memory_iterator.hpp"
#include "task_interface.hpp"

namespace flame { namespace exe {

class AgentTask : public Task {
  friend class TaskManager;

  public:
    //! Enable access to a specific agent var
    void AllowAccess(const std::string& var_name, bool writeable = false);

    //! Returns a new instance of a MemoryIterator
    flame::mem::MemoryIteratorPtr GetMemoryIterator() const;

    //! Returns the name of the task
    std::string get_task_name() const;

    //! Returns the the task type
    TaskType get_task_type() const { return Task::AGENT_FUNCTION; }

    //! Runs the task
    void Run();

    //! \brief Split this task based on population size arguments provided
    TaskSplitterHandle SplitTask(size_t max_tasks, size_t min_task_size);

  protected:
    // Tasks should only be created via Task Manager
    AgentTask(std::string task_name, std::string agent_name,
              TaskFunction func_ptr);

    std::string agent_name_;  //! Name of associated agent
    TaskFunction func_;  //! Function associated with task
    flame::mem::AgentShadowPtr shadow_ptr_;  //! Pointer to AgentShadow

    bool is_split_;  //! Flag indicating task is a subtask (split task)
    size_t offset_;  //! Memory iterator offset (only used if is_split_)
    size_t count_;  //! Number of agents to iterate (only used if is_split_)

    //! Constructor used internally to produce split task
    AgentTask(const AgentTask& parent, size_t offset, size_t count);
};

}}  // namespace flame::exe
#endif  // EXE__AGENT_TASK_HPP_
