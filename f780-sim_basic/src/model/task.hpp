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
#include <utility>  // for pair<>
#include <vector>

namespace flame { namespace model {

// Define type used to hold variable writing tasks
typedef std::map<std::string, std::set<size_t> > VarMapToVertices;

class Task {
  public:
    enum TaskType { xfunction = 0, sync_start, sync_finish, xstate,
                    io_pop_write, start_agent, finish_agent, xcondition,
                    xvariable, xmessage, start_model };
    Task(std::string parentName, std::string name, TaskType type);
    void setTaskID(size_t id);
    size_t getTaskID();
    void setParentName(std::string parentName);
    std::string getParentName();
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
    VarMapToVertices * getLastWrites();
    std::set<size_t> * getLastConditions();

  private:
    std::string parentName_;
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
    VarMapToVertices lastWrites_;
    std::set<size_t> lastConditions_;
};
}}  // namespace flame::model
#endif  // MODEL__TASK_HPP_
