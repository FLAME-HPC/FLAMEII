/*!
 * \file src/exe/execution_task.hpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Multicore version of the ExecutionTask
 */
#ifndef EXE__EXECUTION_TASK_HPP
#define EXE__EXECUTION_TASK_HPP

#include <vector>
#include <utility>  // for std::pair
#include "include/flame.h"
#include "boost/shared_ptr.hpp"

typedef std::vector<int> MemVector;
typedef MemVector::iterator MemVectorIterator;
typedef boost::shared_ptr<MemVector> MemVectorPtr;
typedef std::pair<MemVectorIterator, MemVectorIterator> MemVectorIteratorPair;


namespace flame { namespace exe { namespace multicore {

class ExecutionTask {
    public:
        ExecutionTask(AgentFuncPtr func_ptr, MemVectorPtr mem_ptr);
        MemVectorIteratorPair get_mem_iter(void);
        AgentFuncPtr get_func_ptr(void);
    private:
        // TODO(lsc): Store task data as separate structure referenced using
        //       boost::shared_ptr so that we can split tasks into subtasks
        //       while sharing the same data
        AgentFuncPtr fptr_;
        MemVectorPtr mptr_;
};
}}}
#endif  // EXE__EXECUTION_TASK_HPP
