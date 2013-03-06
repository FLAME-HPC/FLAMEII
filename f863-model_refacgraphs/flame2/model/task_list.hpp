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

class TaskList {
  public:
    //! Constructor
    TaskList();
    //! Destructor
    ~TaskList();
    //! Add a task
    void addTask(ModelTaskPtr ptr);
    //! Remove a task
    //! \param index the task index
    void removeTask(size_t index);
    //! \return The index of the task
    size_t getIndex(ModelTask * t) const;
    //! \return The task given the index
    ModelTask * getTask(size_t index) const;
    //! \return The task pointer object given the index
    ModelTaskPtr getTaskPtr(size_t index) const;
    //! \return The task list size
    size_t getTaskCount() const;
    //! Replace the entire task vector with the one given
    void replaceTaskVector(std::vector<ModelTaskPtr> * vertex2task);

  private:
    //! Pointer to the task vector so that mappings can be swapped
    std::vector<ModelTaskPtr> * vertex2task_;
};

}}  // namespace flame::model
#endif  // MODEL__TASKLIST_HPP_
