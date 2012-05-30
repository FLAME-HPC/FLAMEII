/*!
 * \file src/exe/runnable_task.hpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Interface class for tasks than can be executed by workers
 */
#ifndef EXE__RUNNABLE_TASK_HPP_
#define EXE__RUNNABLE_TASK_HPP_
#include "include/flame.h"
#include "mem/memory_iterator.hpp"

namespace flame { namespace exe {

class RunnableTask {
  public:
    virtual ~RunnableTask() {}
    virtual AgentFuncPtr GetFunction() const = 0;
    virtual flame::mem::MemoryIteratorPtr GetMemoryIterator() const = 0;
    virtual void TaskDone() = 0;
};

}}  // namespace flame::exe
#endif  // EXE__RUNNABLE_TASK_HPP_
