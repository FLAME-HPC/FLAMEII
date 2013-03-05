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
    TaskList();
    ~TaskList();
    void addTask(ModelTaskPtr ptr);
    void removeTask(size_t index);
    size_t getIndex(ModelTask * t) const;
    ModelTask * getTask(size_t index) const;
    ModelTaskPtr getTaskPtr(size_t index) const;
    size_t getTaskCount() const;
    void replaceTaskVector(std::vector<ModelTaskPtr> * vertex2task);

  private:
    /*! \brief Ptr to vertex task so that mappings can be swapped */
    std::vector<ModelTaskPtr> * vertex2task_;
};

}}  // namespace flame::model
#endif  // MODEL__TASKLIST_HPP_
