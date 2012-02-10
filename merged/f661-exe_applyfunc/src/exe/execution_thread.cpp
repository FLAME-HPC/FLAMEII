/*!
 * \file src/exe/execution_thread.cpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Multicore version of the ExecutionThread
 */
#include "execution_thread.hpp"
#include <algorithm>

namespace flame { namespace exe { namespace multicore {

ExecutionThread::ExecutionThread() {}

void ExecutionThread::RunTask(ExecutionTask task) {
    AgentFuncPtr agent_function = task.get_func_ptr();
    MemVectorIteratorPair iter_pair = task.get_mem_iter();

    for (MemVectorIterator i = iter_pair.first; i != iter_pair.second; i++) {
        agent_function(&i[0]);
    }
}
}}}  // namespace flame::exe::multicore
