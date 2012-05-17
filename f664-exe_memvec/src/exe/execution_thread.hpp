/*!
 * \file FILENAME
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief DESCRIPTION
 */
#ifndef EXE__EXECUTION_THREAD_HPP_
#define EXE__EXECUTION_THREAD_HPP_
#include <string>

namespace flame { namespace exe {

// NOTE: This is currently a dummy implementation to test out access to
// mem vectors within a task.
// The interface to this object is subjec to change

class ExecutionThread {
  public:
    void Run(std::string task_name);
};

}}  // namespace flame::exe
#endif  // EXE__EXECUTION_THREAD_HPP_
