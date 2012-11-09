/*!
 * \file flame2/exe/task_manager.cpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief DESCRIPTION
 *
 * \note This object is meant to be used only by the main thread and is
 * therefore not currently thread-safe.
 */
#ifndef EXE__TASK_MANAGER_HPP_
#define EXE__TASK_MANAGER_HPP_
#include <vector>
#include <string>
#include <set>
#include <map>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <boost/thread/mutex.hpp>
#include "message_board_task.hpp"
#include "io_task.hpp"
#include "task_interface.hpp"

namespace flame { namespace exe {

typedef std::map<std::string, size_t> TaskNameMap;

/*!
 * \brief Task Manager object.
 *
 * This is a singleton class - only one instance should exist throughtout
 * the simulation. Instances are accessed using TaskManager::GetInstance().
 * Apart from the GetTask methods, all others should be called during the
 * initialisation stage before threads are spawned, or guarded by mutexes
 */
class TaskManager {
  public:
    typedef Task::id_type TaskId;
    typedef std::set<TaskId> IdSet;
    typedef std::vector<TaskId> IdVector;

    //! \brief Returns instance of singleton object
    static TaskManager& GetInstance() {
      static TaskManager instance;
      return instance;
    }

    //! \brief Registers and returns a new Agent Task
    Task& CreateAgentTask(std::string task_name,
                          std::string agent_name,
                          TaskFunction func_ptr);

    //! \brief Registers and returns a new MessageBoard Task
    Task& CreateMessageBoardTask(std::string task_name,
                                 std::string msg_name,
                                 MessageBoardTask::Operation op);

    //! \brief Registers and returns a new IO Task
    Task& CreateIOTask(std::string task_name,
                       std::string agent_name,
                       std::string var_name,
                       IOTask::Operation op);

    //! \brief Returns a registered Task given a task id
    Task& GetTask(TaskId task_id);

    //! \brief Returns a registered Task given a task name
    Task& GetTask(std::string task_name);

    //! \brief Returns the number of registered tasks
    size_t GetTaskCount() const;

    //! \brief Adds a dependency to a task.
    void AddDependency(std::string task_name, std::string dependency_name);

    //! \brief Adds a dependency to a task
    void AddDependency(TaskId task_id, TaskId dependency_id);

    //! \brief Retrieves a set of dependencies for a given task
    IdSet& GetDependencies(TaskId task_id);
    //! \brief Retrieves a set of dependencies for a given task
    IdSet& GetDependencies(std::string task_name);
    //! \brief Retrieves a set of dependents for a given task
    IdSet& GetDependents(TaskId task_id);
    //! \brief Retrieves a set of dependents for a given task
    IdSet& GetDependents(std::string task_name);

    //! \brief Indicates that all task data have been entered and iteration
    //! can begin.
    //!
    //! Methods that modify the list of tasks and dependency data will no
    //! longer be allowed.
    void Finalise();

    //! \brief Returns true if Finalise() has been called
    bool IsFinalised() const;

    //! \brief Resets control data so a new iteration of tasks can begin
    void IterReset();

    //! \brief Returns true if there are tasks ready for execution
    bool IterTaskAvailable() const;

    //! \brief Returns true if all tasks have been executed
    bool IterCompleted() const;

    //! \brief Returns the number of tasks ready for execution
    size_t IterGetReadyCount() const;

    //! \brief Returns the number of tasks that are still waiting for their
    //! dependencies to be met
    size_t IterGetPendingCount() const;

    //! \brief Returns the number of tasks that have been assigned but not
    //! completed.
    size_t IterGetAssignedCount() const;

    //! \brief Indicates that a specific task has been completed
    void IterTaskDone(TaskId task_id);

    //! \brief Pops and returns a task that is ready for execution
    TaskId IterTaskPop();


#ifdef TESTBUILD
    //! \brief Delete all tasks
    void Reset();

    //! \brief Returns the number of tasks that has no dependencies
    size_t get_num_roots() { return roots_.size(); }

    //! \brief Returns the number of tasks that has no dependents
    size_t get_num_leaves() { return leaves_.size(); }

    TaskId get_id(std::string task_name) {
      return GetId(task_name);
    };
#endif

  private:
    // This is a singleton class. Disable manual instantiation
    TaskManager() : finalised_(false) {}
    // This is a singleton class. Disable copy constructor
    TaskManager(const TaskManager&);
    // This is a singleton class. Disable assignment operation
    void operator=(const TaskManager&);

    void RegisterTask(std::string task_name, Task* task_ptr);

    //! \brief Returns true if given id is a valid task id
    bool IsValidID(TaskId task_id) const;

    //! \brief Returns the corresponding task id given a task name
    TaskId GetId(std::string task_name) const;

#ifdef DEBUG
    //! \brief Determines whether the proposed dependency will create a cycle
    bool WillCauseCyclicDependency(TaskId task_id, TaskId dependency_id);
#endif

    //! \brief Mutex used to control access to task data
    boost::mutex mutex_task_;

    //! \brief Vector of tasks objects. The vector index is used as the task id.
    boost::ptr_vector<Task> tasks_;

    //! \brief Map associating task name to task id (index within tasks_)
    TaskNameMap name_map_;

    //! \brief Set of tasks with no dependencies
    std::set<TaskId> roots_;  // nodes with no dependencies

    //! \brief Set of tasks with no dependents
    std::set<TaskId> leaves_;  // nodes with no dependents

    //! \brief Set of dependencies for each task
    //!
    //! Used for backward traversal as well as determining next available task
    //! upon task completion.
    std::vector<IdSet> parents_;  // set of dependencies for each node

    //! \brief Set of dependents for each task
    //!
    //! Adjacency list representing the directed acyclic dependency graph
    std::vector<IdSet> children_;  // set of dependents for each node

    //! \brief Flag indicating whether Finalise() has been called
    bool finalised_;

    // ---- data used for managing task iteration -------
    //! \brief dependencies for pending tasks
    std::vector<IdSet> pending_deps_;

    //! \brief tasks that are ready for execution
    IdVector ready_tasks_;

    //! \brief tasks that are not yet ready for execution
    IdSet pending_tasks_;

    //! \brief tasks that have been assigned but not completed
    IdSet assigned_tasks_;
};

}}  // namespace flame::exe
#endif  // EXE__TASK_MANAGER_HPP_
