#ifndef MB__BOARD_WRITER_HPP
#define MB__BOARD_WRITER_HPP
#include <string>
#include <typeinfo>
#include "boost/ptr_container/ptr_map.hpp"
#include "mem/vector_wrapper.hpp"
#include "message_board.hpp"

namespace flame { namespace mb {

typedef std::map<std::string, const std::type_info*> TypeMap;
typedef flame::mem::VectorWrapperBase GenericVector;

//! Map container used to store memory vectors
typedef boost::ptr_map<std::string, GenericVector> MemoryMap;

class BoardWriter {
  friend class MessageBoard;

  public:

  protected:
    // Limit constructor to MessageBoard
    explicit BoardWriter(const std::string message_name);
    // Limit var registration to MessageBoard
    void RegisterVar(std::string var_name, GenericVector* vec);

  private:
    std::string msg_name_;  //! Message name
    MemoryMap mem_map_;  //! map to assing VectorWrapper to var names
    TypeMap type_map_;  //! lookup table for datatypes
};

}}  // namespace flame::mb
#endif // MB__BOARD_WRITER_HPP
