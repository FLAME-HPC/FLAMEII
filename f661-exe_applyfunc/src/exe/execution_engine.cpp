#include "execution_engine.hpp"

namespace flame { namespace exe { namespace multicore {

ExecutionEngine::ExecutionEngine() {}

void ExecutionEngine::RegisterFunction(AgentFuncName func_name, AgentFuncPtr func_ptr) {
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

}}} // namespace flame::exe::multicore
