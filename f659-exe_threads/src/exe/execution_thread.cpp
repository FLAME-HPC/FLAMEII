/*!
 * \file FILENAME
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief DESCRIPTION
 */
#include <string>
#include "include/flame.h"
#include "execution_thread.hpp"
#include "task_manager.hpp"

namespace flame { namespace exe {

void ExecutionThread::Run(std::string task_name) {
  Task& t = TaskManager::GetInstance().GetTask(task_name);
  flame::mem::MemoryIteratorPtr m_ptr = t.GetMemoryIterator();
  AgentFuncPtr f_ptr = t.GetFunction();

  m_ptr->Rewind();
  while (!m_ptr->AtEnd()) {
    f_ptr(static_cast<void*>(m_ptr.get()));
    // TODO(lsc): handle agent death (func returns != 0)
    m_ptr->Step();
  }
}

}}  // namespace flame::exe
