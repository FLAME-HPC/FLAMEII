/*!
 * \file src/exe/execution_thread.hpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Multicore version of the ExecutionThread
 */
#ifndef EXE__EXECUTION_THREAD_H
#define EXE__EXECUTION_THREAD_H
#include <cassert>
#include "include/flame.h"
#include "execution_task.hpp"

namespace flame { namespace exe { namespace multicore {

// Note: Mock implementation. No threading involved.
//       For now, we're mainly concerned applying agent transition functions
class ExecutionThread {
    public:
        ExecutionThread();
        void RunTask(ExecutionTask task);
};
}}}  // namespace flame::exe::multicore
#endif  // EXE__EXECUTION_THREAD_H
