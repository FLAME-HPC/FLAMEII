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

//! \brief Define type used to hold variable writing tasks
typedef std::map<std::string, std::set<size_t> > VarMapToVertices;

/*!
 * \brief Class to hold a graph model task
 */
class ModelTask {
  public:
    /*!
     * \brief Type of the task
     */
    enum TaskType { xfunction = 0, xstate, xmessage_sync, xmessage_clear,
      io_pop_write, start_agent, finish_agent, xcondition,
      xvariable, start_model, finish_model, xmessage};
    /*!
     * \brief Constructor
     * \param parentName agent or model name
     * \param name function/variable/message name
     * \param type the task type
     */
    ModelTask(std::string parentName, std::string name, TaskType type);
    /*!
     * \brief Get the task name
     * \return The task name
     */
    std::string getTaskName();
    /*!
     * \brief Set the parent name
     * \param[in] parentName The parent name
     */
    void setParentName(std::string parentName);
    /*!
     * \brief Get the parent name
     * \return The parent name
     */
    std::string getParentName() const;
    /*!
     * \brief Set the task name
     * \param[in] name The task name
     */
    void setName(std::string name);
    /*!
     * \brief Get the task name
     * \return The task name
     */
    std::string getName() const;
    /*!
     * \brief Set the task type
     * \param[in] type The task type
     */
    void setTaskType(TaskType type);
    /*!
     * \brief Get the task type
     * \return The task type
     */
    TaskType getTaskType();
    /*!
     * \brief Set the priority level
     * \param[in] l The priority level
     */
    void setPriorityLevel(size_t l);
    /*!
     * \brief Get the priority level
     * \return the priority level
     */
    size_t getPriorityLevel();
    /*!
     * \brief Add a read write variable
     * \param[in] name Variable name
     */
    void addReadWriteVariable(std::string name);
    /*!
     * \brief Add a read only variable
     * \param name Variable name
     */
    void addReadOnlyVariable(std::string name);
    /*!
     * \brief Get the set of read only variables
     * \return Set of read only variables
     */
    std::set<std::string>* getReadOnlyVariables();
    /*!
     * \brief Add a read variable
     * \param[in] name Variable name
     */
    void addReadVariable(std::string name);
    /*!
     * \brief Get the set of read variables
     * \return Set of read variables
     */
    std::set<std::string>* getReadVariables();
    /*!
     * \brief Add a write variable
     * \param[in] name Variable name
     */
    void addWriteVariable(std::string name);
    /*!
     * \brief Get the set of write variables
     * \return Set of write variables
     */
    std::set<std::string>* getWriteVariables();
    /*!
     * \brief Set if the task has a condition
     * \param[in] hasCondition Boolean value
     */
    void setHasCondition(bool hasCondition);
    /*!
     * \brief If condition
     * \return If the task has a condition
     */
    bool hasCondition();
    /*!
     * \brief Get the last writes
     * \return Map of variables to last writing vertices
     */
    VarMapToVertices * getLastWrites();
    /*!
     * \brief Get the last writes
     * \return Map of variables to last reading vertices
     */
    VarMapToVertices * getLastReads();
    /*!
     * \brief Add output message
     * \param[in] name Message name
     */
    void addOutputMessage(std::string name);
    /*!
     * \brief Get output messages
     * \return Set of output messages
     */
    std::set<std::string>* getOutputMessages();
    /*!
     * \brief Add input message
     * \param[in] name Message name
     */
    void addInputMessage(std::string name);
    /*!
     * \brief Get input messages
     * \return Set of input messages
     */
    std::set<std::string>* getInputMessages();
    /*!
     * \brief Get read only variables
     * \return Set of read only variables
     */
    std::set<std::string> getReadOnlyVariablesConst() const;
    /*!
     * \brief Get write only variables
     * \return Set of write variables
     */
    std::set<std::string> getWriteVariablesConst() const;
    /*!
     * \brief Get output messages
     * \return Set of output messages
     */
    std::set<std::string> getOutputMessagesConst() const;
    /*!
     * \brief Get input messages
     * \return Set of input messages
     */
    std::set<std::string> getInputMessagesConst() const;
    /*!
     * \brief Set if task is start task
     * \param[in] b Boolean value
     */
    void setStartTask(bool b);
    /*!
     * \brief Check is task is a start task
     * \return If task is start task
     */
    bool startTask();
    /*!
     * \brief Set if task is end task
     * \param[in] b Boolean value
     */
    void setEndTask(bool b);
    /*!
     * \brief Is task an end task
     * \return If task is end task
     */
    bool endTask();

  private:
    //! \brief Agent name or model name
    std::string parentName_;
    //! \brief Function/variable/message name
    std::string name_;
    //! \brief Task type
    TaskType taskType_;
    //! \brief Priority level: determines the priority of this task should
    //!        there be more than one task in the queue
    size_t priorityLevel_;
    //! \brief Names of variables that are read only (RO variables)
    std::set<std::string> readOnlyVariables_;
    //! \brief Names of variables that are read (RO and RW variables)
    std::set<std::string> readVariables_;
    //! \brief Names of variables that are written (RW variables)
    std::set<std::string> writeVariables_;
    //! \brief Does this task have an associated condition
    bool hasCondition_;
    //! \brief Map of variables to last writing vertices
    VarMapToVertices lastWrites_;
    //! \brief Map of variables to last reading vertices
    VarMapToVertices lastReads_;
    //! \brief Names of messages that are output
    std::set<std::string> outputMessages_;
    //! \brief Names of messages that are input
    std::set<std::string> inputMessages_;
    //! \brief If task is a start task
    bool startTask_;
    //! \brief If task is an end task
    bool endTask_;
};
}}  // namespace flame::model
#endif  // MODEL__MODEL_TASK_HPP_
