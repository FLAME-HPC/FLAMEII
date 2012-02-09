#include "execution_task.hpp"
namespace flame { namespace exe { namespace multicore {

// This is a mock implementation. We shouldn't really be passing in a vector by vlue
ExecutionTask::ExecutionTask(AgentFuncPtr func_ptr, MemVectorPtr mem_ptr)
{
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
