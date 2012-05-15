/*!
 * \file src/exe/task.cpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief DESCRIPTION
 */
#ifndef EXE__TASK_HPP_
#define EXE__TASK_HPP_
#include <string>
#include <utility>
#include <map>
#include <set>
#include "include/flame.h"
#include "mem/memory_manager.hpp"

//! \TODO since the number of entries in vec_map_ and rw_set_ will be
//! relatively small, the tree-based search used by std::map and
//! std::set may not be ideal. Consider implementing variants that
//! uses a sorted vector instead. See:
//!  - AssocVector: http://loki-lib.sourceforge.net/html/a00645.html
//!  - http://lafstern.org/matt/col1.pdf

namespace flame { namespace exe {

// forward declaration to allow reference without including headers
class VectorWrapperBase;
class TaskManager;

typedef std::map<std::string, flame::mem::VectorWrapperBase*>  VectorMap;

class Task {
  friend class TaskManager;

  public:
    void AllowAccess(std::string var_name, bool writeable = false);
    std::string get_task_name();

  protected:
    // Tasks should only be created via Task Manager
    Task(std::string task_name, std::string agent_name, AgentFuncPtr func_ptr);

  private:
    std::string task_name_;
    std::string agent_name_;
    AgentFuncPtr func_ptr_;
    VectorMap vec_map_;
    std::set<std::string> rw_set_;
};

}}  // namespace flame::exe
#endif  // EXE__TASK_HPP_
