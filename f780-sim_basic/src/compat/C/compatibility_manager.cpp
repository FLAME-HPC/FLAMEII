/*!
 * \file src/compat/c/compatibility_manager.cpp
 * \author Shawn Chin
 * \date October 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Container to handle type mappings and conversions for the C API
 */
#include <string>
#include "compatibility_manager.hpp"

namespace flame { namespace compat { namespace c {

MessageBoardWrapper* CompatibilityManager::GetBoardWrapper(std::string msg) {
  try {
    return &mb_map_.at(msg);
  } catch(const boost::bad_ptr_container_operation& E) {
    throw flame::exceptions::invalid_variable("Unknown message");
  }
}

#ifdef TESTBUILD
    //! Delete all registered types
    void CompatibilityManager::Reset(void) {
      mb_map_.clear();
    }
#endif

}}}  // namespace flame::compat::c
