/*!
 * \file FILENAME
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief DESCRIPTION
 */
#include "task_splitter.hpp"

namespace flame { namespace exe {
TaskSplitter::TaskSplitter(Task::id_type id, const TaskVector& tasks)
    : id_(id), running_(0), tasks_(tasks) {}

bool TaskSplitter::IsComplete(void) const {
  return (running_ == 0 && tasks_.empty());
}
}}  // namespace flame::exe
