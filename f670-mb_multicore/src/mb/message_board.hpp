/*!
 * \file src/mb/message_board.cpp
 * \author Shawn Chin
 * \date September 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Declaration of MessageBoard class
 */
#ifndef MB__MESSAGE_BOARD_HPP_
#define MB__MESSAGE_BOARD_HPP_
#include <string>
#include <utility>
#include <vector>
#include "boost/ptr_container/ptr_map.hpp"
#include "boost/shared_ptr.hpp"
#include "exceptions/mem.hpp"
#include "mem/vector_wrapper.hpp"

namespace flame { namespace mb {

//! Forward declaration of BoardWriter
class BoardWriter;

//! Map container used to store memory vectors
typedef boost::ptr_map<std::string, flame::mem::VectorWrapperBase> MemoryMap;

//! Handle that is returned in place of a BoardWriter instance
typedef boost::shared_ptr<BoardWriter> BoardWriterHandle;


/*!
 * \brief A Message Board instance
 *
 * This class holds the data vectors that represent a list of messages.
 * Access the messages are provided via BoardWriters and Iterators.
 *
 * This class can be inherited to allow custom sync operations.
 */
class MessageBoard {
  public:
    //! Vector storing shared pointers to BoardWriters
    typedef std::vector<BoardWriterHandle> WriterVector;

    //! Constructor
    explicit MessageBoard(const std::string message_name);

    //! Virtual destructor
    virtual ~MessageBoard() {}

    //! Performs Sync opeation
    void Sync(void);

    //! Returns the number of messages that have been synched
    size_t GetCount(void);

    //! Creates and returns a new board writer
    BoardWriterHandle GetBoardWriter(void);

    //! Registers a message variable of a specific data type
    template <typename T>
    void RegisterVar(std::string var_name) {
      if (finalised_) {
        throw flame::exceptions::logic_error(
          "variables can no longer be registered");
      }

      std::pair<MemoryMap::iterator, bool> ret;
      ret = mem_map_.insert(var_name, new flame::mem::VectorWrapper<T>());
      if (!ret.second) {  // if replacement instead of insertion
        throw flame::exceptions::logic_error("variable already registered");
      }
    }

  protected:
    //! Custom sync operations. To be overloaded by derived classes.
    virtual void _sync();

  private:
    size_t count_;  //! Total number of messages that have been synched
    std::string msg_name_;  //! Name of message
    MemoryMap mem_map_;  //! map to assing VectorWrapper to var names
    WriterVector writers_;  //! Registered board writers
    bool finalised_; //! Flag to indicate new vars can no longer be registered

    //! Collates all messages and clears writers_
    void _merge_boards(void);
};

}}  // namespace flame::msg
#endif  // MB__MESSAGE_BOARD_HPP_
