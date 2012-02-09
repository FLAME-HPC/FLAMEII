#ifndef EXECUTIONENGINE_H
#define EXECUTIONENGINE_H

#include "flame.h"
#include "boost/unordered_map.hpp"

namespace flame { namespace exe { namespace multicore {

/*! Datatype used to store function name */
typedef  const char* AgentFuncName;

/*! Function pointer type for agent transition functions */
typedef FLAME_AGENT_FUNC((*AgentFuncPtr));


class ExecutionEngine
{
    public:
        ExecutionEngine();
        void RegisterFunction(AgentFuncName, AgentFuncPtr);
        AgentFuncPtr GetFunction(AgentFuncName);
    private:
        boost::unordered_map<AgentFuncName, AgentFuncPtr> func_map_;
};

}}} // namespace flame::exe::multicore
#endif // EXECUTIONENGINE_H
