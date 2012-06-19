/*!
 * \file src/exe/task.cpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Agent function task
 */
#include <utility>
#include <string>
#include "agent_task.hpp"
#include "exceptions/all.hpp"
#include "mem/memory_manager.hpp"

namespace flame { namespace exe {

namespace mem = flame::mem;
typedef std::pair<std::string, mem::VectorWrapperBase*>  VectorMapValue;

AgentTask::AgentTask(std::string task_name, std::string agent_name,
                     TaskFunction func)
    : agent_name_(agent_name), func_(func) {
  task_name_ = task_name;
  mem::MemoryManager& mm = mem::MemoryManager::GetInstance();
  if (!mm.IsRegisteredAgent(agent_name)) {
    throw flame::exceptions::invalid_agent("Invalid agent");
  }
  if (!func) {
    throw flame::exceptions::invalid_argument("NULL function provided");
  }

  shadow_ptr_ = mm.GetAgentShadow(agent_name);
}

flame::mem::MemoryIteratorPtr AgentTask::GetMemoryIterator() const {
  return shadow_ptr_->GetMemoryIterator();
}

void AgentTask::AllowAccess(const std::string& var_name, bool writeable) {
  shadow_ptr_->AllowAccess(var_name, writeable);
}

void AgentTask::Run() {
  mem::MemoryIteratorPtr m_ptr = GetMemoryIterator();
  m_ptr->Rewind();
  while (!m_ptr->AtEnd()) {  // run function for each agent
    func_(static_cast<void*>(m_ptr.get()));
    // TODO(lsc): check rc == 0 to handle agent death
    m_ptr->Step();
  }
}

}}  // namespace flame::exe
