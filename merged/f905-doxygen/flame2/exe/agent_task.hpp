/*!
 * \file flame2/exe/agent_task.hpp
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
#include "flame2/mem/memory_manager.hpp"
#include "flame2/mem/memory_iterator.hpp"
#include "task_interface.hpp"

namespace flame { namespace exe {

//! Task that runs agent transition functions
class AgentTask : public Task {
  friend class TaskManager;

  public:
    //! Grant memory access to a specific agent variable
    void AllowAccess(const std::string& var_name, bool writeable = false);

    /*!
     * \brief Returns a new instance of a MemoryIterator
     * \return pointer to memory iterator
     *
     * Creates and returns a memory iterator using the assigned agent shadow.
     *
     * If is_split_ is true, offset values are provided such that only the
     * apropriate subset of the memory is iterated.
     */
    flame::mem::MemoryIteratorPtr GetMemoryIterator() const;

    //! Returns the name of the task
    std::string get_task_name() const;

    //! Returns the the task type
    TaskType get_task_type() const { return Task::AGENT_FUNCTION; }

    /*!
     * \brief Runs the task
     *
     * A memory iterator is retrived and a message board client is created.
     *
     * The agent memory is then iterated, and each attached function is run for
     * each agent. The memory iterator and message board client is passed to the
     * function to all them controlled access to memory and messages.
     *
     * \todo (lsc) Mark agent for deletion if the function returns FLAME_AGENT_DEAD.
     */
    void Run();

    /*!
     * \brief Split this task based on population size arguments provided
     * \param[in] max_tasks Maximum subtasks that should be created
     * \param[in] min_task_size Minimum population size per task after split
     * \return A handle to a TaskSplitter object (or null handle if no split)
     *
     * Task splitting which allows task to be executed in segments. If the vectors
     * are too small (depends on min_task_size) to split, a null handle is returned.
     *
     * If a split is possible, subtasks are created using the alternative
     * constructor and wrapped up in a TaskSplitter instance.
     */
    TaskSplitterHandle SplitTask(size_t max_tasks, size_t min_task_size);

  protected:
    /*!
     * \brief Constructor (Tasks should only be created via Task Manager)
     * \param task_name Task name
     * \param agent_name Agent name
     * \param func_ptr Agent transition function
     * 
     * Initialises all parameters.
     *
     * An agent shadow instance is created and stored in shadow_ptr_. This stores
     * memory access permissions and can be used to retrieve a memory iterator
     * when running the task.
     *
     * Throws throw flame::exceptions::invalid_agent if the agent name is invalid.
     *
     * Throws flame::exceptions::invalid_argument if the function pointer is
     * invalid.
     */
    AgentTask(std::string task_name, std::string agent_name,
              TaskFunction func_ptr);

    std::string agent_name_;  //!< Name of associated agent
    TaskFunction func_;  //!< Function associated with task
    flame::mem::AgentShadowPtr shadow_ptr_;  //!< Pointer to AgentShadow

    bool is_split_;  //!< Flag indicating task is a subtask (split task)
    size_t offset_;  //!< Memory iterator offset (only used if is_split_)
    size_t count_;  //!< Number of agents to iterate (only used if is_split_)

    /*!
     * \brief Alternative constructor used internally to produce split task
     * \param parent parent task
     * \param offset memory vector offset
     * \param count number of agents addressed by this task
     *
     * Copies all internal variables but changes the values for offset_ and count_.
     *
     * is_split_ is set to true to indicate that this is split from a parent task.
     */
    AgentTask(const AgentTask& parent, size_t offset, size_t count);

  private:
    //! Derive and return the transition function name from task name
    std::string get_transition_function_name(void) const {
      // Since we're yet to determine how task names are actually
      // built, for now we return task name as is.
      return task_name_;
    }
};

}}  // namespace flame::exe
#endif  // EXE__AGENT_TASK_HPP_
