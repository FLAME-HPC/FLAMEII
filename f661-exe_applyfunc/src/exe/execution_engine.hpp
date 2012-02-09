#ifndef EXE__EXECUTIONENGINE_H
#define EXE__EXECUTIONENGINE_H

#include "flame.h"
#include "execution_thread.hpp"

#include "boost/unordered_map.hpp"

namespace flame { namespace exe { namespace multicore {

/*! Datatype used to store function name */
typedef  const char* AgentFuncName;


class ExecutionEngine
{
    public:
        ExecutionEngine();
        void RegisterFunction(AgentFuncName func_name, AgentFuncPtr func_ptr);
        void RunFunction(AgentFuncName func_name, MemVectorPtr mem_ptr);
        AgentFuncPtr GetFunction(AgentFuncName func_name);

    private:
        boost::unordered_map<AgentFuncName, AgentFuncPtr> func_map_;
        bool thread_started_;
        ExecutionThread thread_;
};

}}} // namespace flame::exe::multicore
#endif // EXE__EXECUTIONENGINE_H
