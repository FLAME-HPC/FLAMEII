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
#include "boost/ptr_container/ptr_vector.hpp"
#include "exceptions/mem.hpp"
#include "mem/vector_wrapper.hpp"

namespace flame { namespace mb {

class BoardWriter; // Forward declaration

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
        throw flame::exceptions::logic_error(
          "variables can no longer be registered");
      }

      std::pair<MemoryMap::iterator, bool> ret;
      ret = mem_map_.insert(var_name, new flame::mem::VectorWrapper<T>());
      if (!ret.second) { // if replacement instead of insertion
        throw flame::exceptions::logic_error("variable already registered");
      }

    }

    //! Creates and returns a new board writer
    BoardWriter* GetBoardWriter(void);

  protected:
    virtual void _sync();  // overload this for custom sync routines

  private:
    std::string msg_name_;  //! Name of message
    MemoryMap mem_map_;  //! map to assing VectorWrapper to var names
    boost::ptr_vector<BoardWriter> writers_;  //! Registered board writers

    //! Flag to indicate that vars can no longer be registered
    bool finalised_;
};

}}  // namespace flame::msg
#endif // MB__MESSAGE_BOARD_HPP_
