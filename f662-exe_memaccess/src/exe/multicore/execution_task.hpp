/*!
 * \file src/exe/multicore/execution_task.hpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Multicore version of the ExecutionTask
 */
#ifndef EXE__MULTICORE__EXECUTION_TASK_HPP
#define EXE__MULTICORE__EXECUTION_TASK_HPP
#include <vector>
#include <utility>  // for std::pair
#include "boost/shared_ptr.hpp"
#include "include/flame.h"

namespace flame { namespace exe { namespace multicore {

typedef std::vector<int> MemVector;
typedef MemVector::iterator MemVectorIterator;
typedef boost::shared_ptr<MemVector> MemVectorPtr;
typedef std::pair<MemVectorIterator, MemVectorIterator> MemVectorIteratorPair;

class ExecutionTask {
    public:
        ExecutionTask(AgentFuncPtr func_ptr, MemVectorPtr mem_ptr);
        MemVectorIteratorPair get_mem_iter(void);
        AgentFuncPtr get_func_ptr(void);
    private:
        AgentFuncPtr fptr_;
        MemVectorPtr mptr_;
};
}}}  // namespace flame::exe::multicore
#endif  // EXE__MULTICORE__EXECUTION_TASK_HPP
