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

    //! Returns a registered Task
    Task& GetTask(RunnableTask::id_type task_id);
    Task& GetTask(std::string task_name);

    size_t get_task_count();

    typedef std::vector<RunnableTask::id_type> IdVector;

#ifdef TESTBUILD
    //! Delete all tasks
    void Reset();

    size_t get_nodeps_size() { return nodeps_.size(); }
    size_t get_node_count() {

      return children_.size();
    }
#endif

  private:
    //! This is a singleton class. Disable manual instantiation
    TaskManager() {}
    //! This is a singleton class. Disable copy constructor
    TaskManager(const TaskManager&);
    //! This is a singleton class. Disable assignment operation
    void operator=(const TaskManager&);

    boost::ptr_vector<Task> tasks_;
    TaskNameMap name_map_;

    // Datastructures to manage task dependencies
    std::set<RunnableTask::id_type> nodeps_;
    std::vector<IdVector> children_;
    std::vector<IdVector> parents_;
};

}}  // namespace flame::exe
#endif  // EXE__TASK_MANAGER_HPP_
