/*!
 * \file src/exe/execution_task.cpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Multicore version of the ExecutionTask
 */
#include "execution_task.hpp"
namespace flame { namespace exe { namespace multicore {

/* TODO(lsc): Include vector size and bounds. Store func/mem ptr as separate
 *   object; store using boost::shared_ptr. That way, we can split tasks into
 *   subtasks by replicating the ptrs and changing the boinds.
 */
ExecutionTask::ExecutionTask(AgentFuncPtr func_ptr, MemVectorPtr mem_ptr) {
    fptr_ = func_ptr;
    mptr_ = mem_ptr;
}

MemVectorIteratorPair ExecutionTask::get_mem_iter() {
    return MemVectorIteratorPair(mptr_->begin(), mptr_->end());
}

AgentFuncPtr ExecutionTask::get_func_ptr(void) {
    return fptr_;
}
}}}
