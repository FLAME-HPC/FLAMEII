/*!
 * \file src/compat/c/compatibility_manager.hpp
 * \author Shawn Chin
 * \date October 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Container to handle type mappings and conversions for the C API
 */
#ifndef COMPAT__C__COMPATIBILITY_MANAGER_HPP_
#define COMPAT__C__COMPATIBILITY_MANAGER_HPP_
#include <string>
#include <utility>
#include "exceptions/all.hpp"
#include "boost/ptr_container/ptr_map.hpp"
#include "mb/message_board_manager.hpp"
#include "message_board_wrapper.hpp"

namespace flame { namespace compat { namespace c {

namespace mb = flame::mb;

class CompatibilityManager {
  public:
    //! Returns instance of singleton object
    //!  When used in a multithreaded environment, this should be called
    //!  at lease once before threads are spawned.
    static CompatibilityManager& GetInstance() {
      static CompatibilityManager instance;
      return instance;
    }

    template <typename T>
    void RegisterMessage(std::string msg_name) {
      mb::MessageBoardManager& mm = mb::MessageBoardManager::GetInstance();
      mm.RegisterMessage(msg_name);
      mm.RegisterMessageVar<T>(msg_name, Constants::MESSAGE_VARNAME);
      mb_map_.insert(msg_name, new MessageBoardWrapperImpl<T>(msg_name));
    }

    MessageBoardWrapper* GetBoardWrapper(std::string msg);

#ifdef TESTBUILD
    //! Delete all registered types
    void Reset(void);
#endif

  private:
    typedef boost::ptr_map<std::string, MessageBoardWrapper> BoardWrapperMap;
    BoardWrapperMap mb_map_;

    //! This is a singleton class. Disable manual instantiation
    CompatibilityManager() {}
    //! This is a singleton class. Disable copy constructor
    CompatibilityManager(const CompatibilityManager&);
    //! This is a singleton class. Disable assignment operation
    void operator=(const CompatibilityManager&);
};

}}}  // namespace flame::compat::c
#endif  // COMPAT__C__COMPATIBILITY_MANAGER_HPP_
