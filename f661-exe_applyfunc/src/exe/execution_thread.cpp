#include "execution_thread.hpp"
#include <algorithm>

namespace flame { namespace exe { namespace multicore {

ExecutionThread::ExecutionThread()
{
    //ctor
}

void ExecutionThread::RunTask(ExecutionTask& task) {
    AgentFuncPtr agent_function = task.get_func_ptr();
    MemVectorIteratorPair iter_pair = task.get_mem_iter();

    for (MemVector::iterator i = iter_pair.first; i != iter_pair.second; i++) {
        agent_function(&i[0]);
    }

}

}}}
