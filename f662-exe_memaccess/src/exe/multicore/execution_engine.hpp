/*!
 * \file src/exe/multicore/execution_engine.hpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Multicore version of the ExecutionEngine
 */
#ifndef EXE__MULTICORE__EXECUTIONENGINE_H
#define EXE__MULTICORE__EXECUTIONENGINE_H

#include "boost/unordered_map.hpp"
#include "include/flame.h"
#include "execution_thread.hpp"


namespace flame { namespace exe { namespace multicore {

/*! Datatype used to store function name */
typedef  const char* AgentFuncName;

class ExecutionEngine {
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
}}}  // namespace flame::exe::multicore
#endif  // EXE__MULTICORE__EXECUTIONENGINE_H
