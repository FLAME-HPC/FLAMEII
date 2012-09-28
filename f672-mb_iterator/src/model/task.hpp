/*!
 * \file src/model/task.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Task: holds task information
 */
#ifndef MODEL__TASK_HPP_
#define MODEL__TASK_HPP_
#include <string>
#include <vector>
#include "./dependency.hpp"

namespace flame { namespace model {

class Task {
  public:
    enum TaskType { xfunction = 0, sync_start, sync_finish, io_pop_write };
    Task();
    ~Task();
    void setTaskID(size_t id);
    size_t getTaskID();
    void setParentName(std::string name);
    std::string getParentName();
    void setName(std::string name);
    std::string getName();
    std::string getFullName();
    void setTaskType(TaskType type);
    TaskType getTaskType();
    void setLevel(size_t level);
    size_t getLevel();
    void addParent(std::string name,
            Dependency::DependencyType type, Task * task);
    void addDependency(Dependency * d);
    std::vector<Dependency*> getParents();
    void setPriorityLevel(size_t l);
    size_t getPriorityLevel();

  private:
    /* Task identifier: a unique handle for each task */
    size_t taskID_;
    /* Task type: a label to determine which queue the task belongs to */
    TaskType taskType_;
    /* Dependency list: references of tasks that must be completed before
     * this task can be executed */
    std::vector<Dependency*> parents_;
    /* Priority level: determines the priority of this task should there
     * be more than one task in the queue */
    size_t priorityLevel_;
    /* Agent/Message name */
    std::string parentName_;
    /* Function name/Message name/Memory variable name */
    std::string name_;
    /* Level number: used to initially order tasks in the queue */
    size_t level_;
};
}}  // namespace flame::model
#endif  // MODEL__TASK_HPP_
