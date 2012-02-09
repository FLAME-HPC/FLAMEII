#include "execution_engine.hpp"

namespace flame { namespace exe { namespace multicore {

ExecutionEngine::ExecutionEngine() {
    thread_started_ = false;
}

void ExecutionEngine::RegisterFunction(AgentFuncName func_name, AgentFuncPtr func_ptr) {
    if (thread_started_) {
        throw std::domain_error("Function registration not allowed once "
                                "execution thread has started");
    }
    func_map_[func_name] = func_ptr;
}

AgentFuncPtr ExecutionEngine::GetFunction(AgentFuncName func_name) {
    try {
        return func_map_.at(func_name);
    }
    catch(std::out_of_range E) {
        throw std::out_of_range("No registered function with that name");
    }
}

void ExecutionEngine::RunFunction(AgentFuncName func_name, MemVectorPtr mem_ptr) {
    ExecutionTask task(GetFunction(func_name), mem_ptr);
    thread_.RunTask(task);
}

}}} // namespace flame::exe::multicore
