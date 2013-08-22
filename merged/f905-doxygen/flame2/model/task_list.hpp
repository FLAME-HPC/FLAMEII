/*!
 * \file flame2/model/task_list.hpp
 * \author Simon Coakley
 * \date 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief TaskList: used to hold tasks
 */
#ifndef MODEL__TASK_LIST_HPP_
#define MODEL__TASK_LIST_HPP_
#include <vector>
#include <boost/shared_ptr.hpp>
#include "model_task.hpp"

namespace flame { namespace model {

//! Use a shared pointer to automatically handle Task pointers
typedef boost::shared_ptr<ModelTask> ModelTaskPtr;

/*!
 * \brief Class to hold list of tasks
 *
 * The list is held as a vector as position of the task relates
 * to the vertex it is associated with
 */
class TaskList {
  public:
    /*!
     * \brief Constructor
     */
    TaskList();
    /*!
     * \brief Destructor
     */
    ~TaskList();
    /*!
     * \brief Add a task
     * \param[in] ptr Pointer to a task
     */
    void addTask(ModelTaskPtr ptr);
    /*!
     * \brief Remove a task
     * \param index The task index
     */
    void removeTask(size_t index);
    /*!
     * \brief Get the task index
     * \param[in] t The task
     * \return The index of the task
     */
    size_t getIndex(ModelTask * t) const;
    /*!
     * \brief Get the task via an index
     * \param[in] index The index
     * \return The task given the index
     */
    ModelTask * getTask(size_t index) const;
    /*!
     * \brief Get the task pointer object given an index
     * \param[in] index The index
     * \return The task pointer object given the index
     */
    ModelTaskPtr getTaskPtr(size_t index) const;
    /*!
     * \brief Get the number of tasks
     * \return The task list size
     */
    size_t getTaskCount() const;
    /*!
     * \brief Replace the entire task vector with the one given
     * \param[in] vertex2task The new task vector
     */
    void replaceTaskVector(std::vector<ModelTaskPtr> * vertex2task);

  private:
    //! \brief Pointer to the task vector so that mappings can be swapped
    std::vector<ModelTaskPtr> * vertex2task_;
    //! \brief This class has pointer members so disable copy constructor
    TaskList(const TaskList&);
    //! \brief This class has pointer members so disable assignment operation
    void operator=(const TaskList&);
};

}}  // namespace flame::model
#endif  // MODEL__TASKLIST_HPP_
