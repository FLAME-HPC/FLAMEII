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
#include <set>
#include <map>
#include "./dependency.hpp"

namespace flame { namespace model {

class Task;

typedef std::set< std::pair<std::string, size_t> > SetWritingTasks;

class Task {
  public:
    enum TaskType { xfunction = 0, sync_start, sync_finish, xstate,
                    io_pop_write, init_agent, xcondition, xvariable, xmessage };
    Task(std::string name, TaskType type);
    void setTaskID(size_t id);
    size_t getTaskID();
    void setName(std::string name);
    std::string getName();
    void setTaskType(TaskType type);
    TaskType getTaskType();
    void setLevel(size_t level);
    size_t getLevel();
    void setPriorityLevel(size_t l);
    size_t getPriorityLevel();
    void addReadVariable(std::string name);
    std::set<std::string>* getReadVariables();
    void addWriteVariable(std::string name);
    std::set<std::string>* getWriteVariables();
    void setHasCondition(bool hasCondition);
    bool hasCondition();
    SetWritingTasks * getLastWrites();
    std::set<size_t> * getLastConditions();

  private:
    /* Function name/Message name/Memory variable name */
    std::string name_;
    /* Task identifier: a unique handle for each task */
    size_t taskID_;
    /* Task type: a label to determine which queue the task belongs to */
    TaskType taskType_;
    /* Priority level: determines the priority of this task should there
     * be more than one task in the queue */
    size_t priorityLevel_;
    /* Level number: used to initially order tasks in the queue */
    size_t level_;
    /*! \brief Names of variables that are read (RO and RW variables) */
    std::set<std::string> readVariables_;
    /*! \brief Names of variables that are written (RW variables) */
    std::set<std::string> writeVariables_;
    /*! \brief Does this task have an associated condition */
    bool hasCondition_;
    SetWritingTasks lastWrites_;
    std::set<size_t> lastConditions_;

};
}}  // namespace flame::model
#endif  // MODEL__TASK_HPP_
