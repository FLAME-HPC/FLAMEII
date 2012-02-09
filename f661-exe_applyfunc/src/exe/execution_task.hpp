#ifndef EXE__EXECUTION_TASK_HPP
#define EXE__EXECUTION_TASK_HPP

#include <vector>
#include "flame.h"
#include "boost/shared_ptr.hpp"

typedef std::vector<int> MemVector;
typedef boost::shared_ptr<MemVector> MemVectorPtr;
typedef std::pair<MemVector::iterator, MemVector::iterator> MemVectorIteratorPair;


namespace flame { namespace exe { namespace multicore {

class ExecutionTask
{
    public:
        // Dummy implementation. We really don't want to pass vectors around by value
        ExecutionTask(AgentFuncPtr func_ptr, MemVectorPtr mem_ptr);
        MemVectorIteratorPair get_mem_iter(void);
        AgentFuncPtr get_func_ptr(void);
    private:

    // TODO: Store task data as separate structure referenced using boost::shared_ptr
    //        so that we can split tasks into subtasks while sharing the same data
        AgentFuncPtr fptr_;
        MemVectorPtr mptr_;
};

}}}
#endif // EXE__EXECUTION_TASK_HPP
