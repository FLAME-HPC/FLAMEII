/*!
 * \file FILENAME
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief DESCRIPTION
 */
#ifndef EXE__TASK_SPLITTER_HPP_
#define EXE__TASK_SPLITTER_HPP_
#include <vector>
#include "boost/thread/mutex.hpp"
#include "task_interface.hpp"

namespace flame { namespace exe {

class TaskSplitter {
  public:
    typedef std::vector<Task::Handle> TaskVector;
    TaskSplitter(Task::id_type id, const TaskVector& tasks);
    bool IsComplete(void) const;

  protected:
  private:
    Task::id_type id_;
    size_t running_;
    TaskVector tasks_;
    boost::mutex mutex_;
};

}}  // namespace flame::exe
#endif  // EXE__TASK_SPLITTER_HPP_
