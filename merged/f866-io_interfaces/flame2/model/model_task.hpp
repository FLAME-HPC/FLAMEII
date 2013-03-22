/*!
 * \file flame2/model/model_task.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief ModelModelTask: holds task information
 */
#ifndef MODEL__MODEL_TASK_HPP_
#define MODEL__MODEL_TASK_HPP_
#include <string>
#include <set>
#include <map>
#include <utility>  // for pair<>
#include <vector>

namespace flame { namespace model {

// Define type used to hold variable writing tasks
typedef std::map<std::string, std::set<size_t> > VarMapToVertices;

class ModelTask {
  public:
    //! Type of the task
    enum TaskType { xfunction = 0, xstate, xmessage_sync, xmessage_clear,
      io_pop_write, start_agent, finish_agent, xcondition,
      xvariable, start_model, finish_model, xmessage};
    //! Constructor
    //! \param parentName agent or model name
    //! \param name function/variable/message name
    //! \param type the task type
    ModelTask(std::string parentName, std::string name, TaskType type);
    //! \return The task name
    std::string getTaskName();
    //! Set the parent name
    void setParentName(std::string parentName);
    //! \return The parent name
    std::string getParentName() const;
    //! Set the task name
    void setName(std::string name);
    //! \return the task name
    std::string getName() const;
    //! Set the task type
    void setTaskType(TaskType type);
    //! Return the task type
    TaskType getTaskType();
    //! Set the priority level
    void setPriorityLevel(size_t l);
    //! \return the priority level
    size_t getPriorityLevel();
    //! Add a read write variable
    void addReadWriteVariable(std::string name);
    //! Add a read only variable
    void addReadOnlyVariable(std::string name);
    //! \return Set of read only variables
    std::set<std::string>* getReadOnlyVariables();
    //! Add a read variable
    void addReadVariable(std::string name);
    //! \return Set of read variables
    std::set<std::string>* getReadVariables();
    //! Add a write variable
    void addWriteVariable(std::string name);
    //! \return Set of write variables
    std::set<std::string>* getWriteVariables();
    //! Set if the task has a condition
    void setHasCondition(bool hasCondition);
    //! \return If the task has a condition
    bool hasCondition();
    //! \return Map of variables to last writing vertices
    VarMapToVertices * getLastWrites();
    //! \return Map of variables to last reading vertices
    VarMapToVertices * getLastReads();
    //! Add output message
    void addOutputMessage(std::string name);
    //! \return Set of output messages
    std::set<std::string>* getOutputMessages();
    //! Add input message
    void addInputMessage(std::string name);
    //! \return Set of input messages
    std::set<std::string>* getInputMessages();
    //! \return Set of read only variables
    std::set<std::string> getReadOnlyVariablesConst() const;
    //! \return Set of write variables
    std::set<std::string> getWriteVariablesConst() const;
    //! \return Set of output messages
    std::set<std::string> getOutputMessagesConst() const;
    //! \return Set of input messages
    std::set<std::string> getInputMessagesConst() const;
    //! Set if task is start task
    void setStartTask(bool b);
    //! \return If task is start task
    bool startTask();
    //! Set if task is end task
    void setEndTask(bool b);
    //! \return If task is end task
    bool endTask();

  private:
    //! Agent name or model name
    std::string parentName_;
    //! Function/variable/message name
    std::string name_;
    //! Task type
    TaskType taskType_;
    //! Priority level: determines the priority of this task should there
    //! be more than one task in the queue
    size_t priorityLevel_;
    //! Names of variables that are read only (RO variables)
    std::set<std::string> readOnlyVariables_;
    //! Names of variables that are read (RO and RW variables)
    std::set<std::string> readVariables_;
    //! Names of variables that are written (RW variables)
    std::set<std::string> writeVariables_;
    //! Does this task have an associated condition
    bool hasCondition_;
    //! Map of variables to last writing vertices
    VarMapToVertices lastWrites_;
    //! Map of variables to last reading vertices
    VarMapToVertices lastReads_;
    //! Names of messages that are output
    std::set<std::string> outputMessages_;
    //! Names of messages that are input
    std::set<std::string> inputMessages_;
    //! If task is a start task
    bool startTask_;
    //! If task is an end task
    bool endTask_;
};
}}  // namespace flame::model
#endif  // MODEL__MODEL_TASK_HPP_
