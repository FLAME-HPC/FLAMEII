/*!
 * \file FILENAME
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief DESCRIPTION
 */
#ifndef MB__MESSAGE_BOARD_HPP_
#define MB__MESSAGE_BOARD_HPP_
#include <string>
#include "boost/ptr_container/ptr_map.hpp"
#include "exceptions/mem.hpp"
#include "mem/vector_wrapper.hpp"

namespace flame { namespace mb {

//! Map container used to store memory vectors
typedef boost::ptr_map<std::string, flame::mem::VectorWrapperBase> MemoryMap;

class MessageBoard {
  public:
    explicit MessageBoard(const std::string message_name);
    void Sync(void);
    virtual ~MessageBoard() {};

    //! Registers a message variable of a specific data type
    template <typename T>
    void RegisterVar(const std::string var_name) {
      if (finalised_) {
        throw exc::logic_error("variables can no longer be registered");
      }

      std::pair<MemoryMap::iterator, bool> ret;
      ret = mem_map_.insert(var_name, new VectorWrapper<T>());
      if (!ret.second) { // if replacement instead of insertion
        throw exc::logic_error("variable already registered");
      }
    }

  protected:
    virtual void _sync();  // overload this for custom sync routines

  private:
    std::string msg_name;  //! Name of message
    MemoryMap mem_map_;  //! map to assing VectorWrapper to var names
    bool finalised_;
};

}}  // namespace flame::msg
#endif // MB__MESSAGE_BOARD_HPP_
