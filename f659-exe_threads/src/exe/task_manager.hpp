/*!
 * \file src/exe/task_manager.cpp
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
#include <string>
#include <set>
#include "boost/ptr_container/ptr_vector.hpp"
#include "boost/ptr_container/ptr_map.hpp"
#include "task.hpp"

namespace flame { namespace exe {

typedef std::map<std::string, size_t> TaskNameMap;

//! \brief Task Manager object.
//!
//! This is a singleton class - only one instance should exist throughtout
//! the simulation. Instances are accessed using TaskManager::GetInstance().
//! Apart from the GetTask methods, all others should be called during the
//! initialisation stage before threads are spawned, or guarded by mutexes
class TaskManager {
  friend class TaskCoordinator;

  public:
    typedef std::set<RunnableTask::id_type> IdSet;

    //! \brief Returns instance of singleton object
    static TaskManager& GetInstance() {
      static TaskManager instance;
      return instance;
    }

    //! \brief Registers and returns a new Task
    Task& CreateTask(std::string task_name,
                     std::string agent_name,
                     TaskFunction func_ptr);

    //! \brief Returns a registered Task given a task id
    Task& GetTask(RunnableTask::id_type task_id);

    //! \brief Returns a registered Task given a task name
    Task& GetTask(std::string task_name);

    //! \brief Returns the number of registered tasks
    size_t get_task_count();

    //! \brief Adds a dependency to a task.
    void AddDependency(std::string task_name, std::string dependency_name);

    //! \brief Adds a dependency to a task
    void AddDependency(RunnableTask::id_type task_id,
                       RunnableTask::id_type dependency_id);

    //! \brief Retrieves a set of dependencies for a given task
    IdSet& GetDependencies(RunnableTask::id_type task_id);
    //! \brief Retrieves a set of dependencies for a given task
    IdSet& GetDependencies(std::string task_name);
    //! \brief Retrieves a set of dependents for a given task
    IdSet& GetDependents(RunnableTask::id_type task_id);
    //! \brief Retrieves a set of dependents for a given task
    IdSet& GetDependents(std::string task_name);

    //! \brief Indicates that all task data have been entered and iteration
    //! can begin.
    //!
    //! Methods that modify the list of tasks and dependency data will no
    //! longer be allowed.
    void Finalise();

    //! \brief Returns true if Finalise() has been called
    bool IsFinalised();

    //! \brief Resets control data so a new iteration of tasks can begin
    void ResetIterationData();

#ifdef TESTBUILD
    //! \brief Delete all tasks
    void Reset();

    //! \brief Returns the number of tasks that has no dependencies
    size_t get_num_roots() { return roots_.size(); }

    //! \brief Returns the number of tasks that has no dependents
    size_t get_num_leaves() { return leaves_.size(); }
#endif

  private:
    // This is a singleton class. Disable manual instantiation
    TaskManager() : finalised_(false) {}
    // This is a singleton class. Disable copy constructor
    TaskManager(const TaskManager&);
    // This is a singleton class. Disable assignment operation
    void operator=(const TaskManager&);

    //! \brief Returns true if given id is a valid task id
    bool IsValidID(RunnableTask::id_type task_id);

    //! \brief Returns the corresponding task id given a task name
    RunnableTask::id_type GetId(std::string task_name);

#ifdef DEBUG
    //! \brief Determines whether the proposed dependency will create a cycle
    bool WillCauseCyclicDependency(RunnableTask::id_type task_id,
                                   RunnableTask::id_type dependency_id);
#endif

    //! \brief Vector of tasks objects. The vector index is used as the task id.
    boost::ptr_vector<Task> tasks_;

    //! \brief Map associating task name to task id (index within tasks_)
    TaskNameMap name_map_;

    //! \brief Set of tasks with no dependencies
    std::set<RunnableTask::id_type> roots_; // nodes with no dependencies

    //! \brief Set of tasks with no dependents
    std::set<RunnableTask::id_type> leaves_; // nodes with no dependents

    //! \brief Set of dependencies for each task
    //!
    //! Used for backward traversal as well as determining next available task
    //! upon task completion.
    std::vector<IdSet> parents_;  // set of dependencies for each node

    //! \brief Set of dependents for each task
    //!
    //! Adjacency list representing the directed acyclic dependency graph
    std::vector<IdSet> children_; // set of dependents for each node

    //! \brief Flag indicating whether Finalise() has been called
    bool finalised_;

    // ---- data used for managing task iteration -------
    //! \brief dependencies for pending tasks
    std::vector<IdSet> pending_deps_;

    //! \brief tasks that are ready for execution
    std::vector<RunnableTask::id_type> ready_tasks_;

    //! \brief tasks that are not yet ready for execution
    std::set<RunnableTask::id_type> pending_tasks_;


};

}}  // namespace flame::exe
#endif  // EXE__TASK_MANAGER_HPP_
