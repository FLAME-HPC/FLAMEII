/*!
 * \file src/exe/task_manager.cpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief DESCRIPTION
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

//! Task Manager object.
//! This is a singleton class - only one instance should exist throughtout
//! the simulation. Instances are accessed using TaskManager::GetInstance().
//! Apart from the GetTask methods, all others should be called during the
//! initialisation stage before threads are spawned, or guarded by mutexes
class TaskManager {
  public:

    typedef std::set<RunnableTask::id_type> IdSet;

    //! Returns instance of singleton object
    //!  When used in a multithreaded environment, this should be called
    //!  at lease once before threads are spawned.
    static TaskManager& GetInstance() {
      static TaskManager instance;
      return instance;
    }

    //! Registers and returns a new Task
    Task& CreateTask(std::string task_name,
                     std::string agent_name,
                     TaskFunction func_ptr);

    //! Returns a registered Task given a task id
    Task& GetTask(RunnableTask::id_type task_id);

    //! Returns a registered Task given a task name
    Task& GetTask(std::string task_name);

    //! Returns the number of registered tasks
    size_t get_task_count();

    void AddDependency(std::string task_name, std::string dependency_name);
    void AddDependency(RunnableTask::id_type task_id,
                       RunnableTask::id_type dependency_id);

    IdSet& GetDependencies(RunnableTask::id_type task_id);
    IdSet& GetDependencies(std::string task_name);
    IdSet& GetDependents(RunnableTask::id_type task_id);
    IdSet& GetDependents(std::string task_name);

#ifdef TESTBUILD
    //! Delete all tasks
    void Reset();

    //! Returns the number of tasks that has no dependencies
    size_t get_num_roots() { return roots_.size(); }

    //! Returns the number of tasks that has no dependents
    size_t get_num_leaves() { return leaves_.size(); }
#endif

  private:
    //! This is a singleton class. Disable manual instantiation
    TaskManager() {}
    //! This is a singleton class. Disable copy constructor
    TaskManager(const TaskManager&);
    //! This is a singleton class. Disable assignment operation
    void operator=(const TaskManager&);

    bool IsValidID(RunnableTask::id_type task_id);
    RunnableTask::id_type GetId(std::string task_name);

#ifdef DEBUG
    bool WillCauseCyclicDependency(RunnableTask::id_type task_id,
                                   RunnableTask::id_type dependency_id);
#endif

    boost::ptr_vector<Task> tasks_;
    TaskNameMap name_map_;

    // children_ alone should be sufficient to represent a graph, however we
    // want to be able to quickly traverse backwards so we also store the
    // reverse relationship (parents_)
    std::set<RunnableTask::id_type> roots_; // nodes with no dependencies
    std::set<RunnableTask::id_type> leaves_; // nodes with no dependents
    std::vector<IdSet> children_; // set of dependents for each node
    std::vector<IdSet> parents_;  // set of dependencies for each node
};

}}  // namespace flame::exe
#endif  // EXE__TASK_MANAGER_HPP_
