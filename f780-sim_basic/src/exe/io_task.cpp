/*!
 * \file src/exe/io_task.hpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Task that performs IO operations
 */
#include <string>
#include "mem/memory_manager.hpp"
#include "io/io_manager.hpp"
#include "io_task.hpp"

namespace flame { namespace exe {

IOTask::IOTask(std::string task_name, std::string agent_name,
               std::string var_name, Operation op)
    : agent_name_(agent_name), var_name_(var_name), op_(op) {
  if (op_ == OP_OUTPUT) {
    flame::mem::MemoryManager::GetInstance().AssertVarRegistered(agent_name,
                                                                 var_name);
  }
  task_name_ = task_name;
}

void IOTask::Run(void) {
  switch (op_) {
    case OP_OUTPUT:
      flame::io::IOManager::GetInstance().writePop(agent_name_, var_name_);
      break;
    case OP_INIT:
      flame::io::IOManager::GetInstance().initialiseData();
      break;
    case OP_FIN:
      flame::io::IOManager::GetInstance().finaliseData();
      break;
    default:
      throw flame::exceptions::not_implemented("Operation not implemented");
  }
}

}}  // namespace flame::exe
