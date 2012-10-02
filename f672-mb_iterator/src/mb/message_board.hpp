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
#include <typeinfo>
#include "exceptions/mem.hpp"
#include "type_validator.hpp"
#include "mb_common.hpp"
#include "message_iterator_backend.hpp"
#include "message_iterator_backend_raw.hpp"

namespace flame { namespace mb {


/*!
 * \brief A Message Board instance
 *
 * This class holds the data vectors that represent a list of messages.
 * Access the messages are provided via BoardWriters and Iterators.
 *
 * This class can be inherited to allow custom sync operations.
 *
 * Inherits the TypeValidator interface so it can be used to validate
 * the datatype of message variables (used by Message::Set()).
 */
class MessageBoard : public TypeValidator {
  public:
    //! Vector storing shared pointers to BoardWriters
    typedef std::vector<BoardWriterHandle> WriterVector;

    //! Shorthand for message iterator handle
    typedef MessageIteratorHandle Iterator;

    //! Shorthand for message writer handle
    typedef BoardWriterHandle Writer;

    //! Shorthand for message handle
    typedef MessageHandle Message;

    //! Constructor
    explicit MessageBoard(const std::string& message_name);

    //! Virtual destructor
    virtual ~MessageBoard() {}

    //! Performs Sync opeation
    void Sync(void);

    //! Returns the number of messages that have been synched
    size_t GetCount(void) const;

    //! Creates and returns a new board writer
    BoardWriterHandle GetBoardWriter(void);

    //! Creates and returns a message iterator
    MessageIteratorHandle GetMessageIterator(void);

    //! Registers a message variable of a specific data type
    template <typename T>
    void RegisterVar(std::string var_name) {
      if (finalised_) {
        throw flame::exceptions::logic_error(
          "variables can no longer be registered");
      }

      // Create and store type-specific data Vector
      std::pair<MemoryMap::iterator, bool> ret;
      GenericVector* vec_ptr = new flame::mem::VectorWrapper<T>();
      ret = mem_map_.insert(var_name, vec_ptr);  // takes ownership of obj
      if (!ret.second) {  // if replacement instead of insertion
        throw flame::exceptions::logic_error("variable already registered");
      }

      // Cache datatype for so instance can be used as TypeValidator
      RegisterType(var_name, &typeid(T));
    }

  protected:
    //! Custom sync operations. To be overloaded by derived classes.
    virtual void _sync();

  private:
    size_t count_;  //! Total number of messages that have been synched
    std::string msg_name_;  //! Name of message
    MemoryMap mem_map_;  //! map to assign VectorWrapper to var names
    //VectorRefMap ro_map_; //! map to store read-only ptrs to vectors
    WriterVector writers_;  //! Registered board writers
    bool finalised_;  //! Flag to indicate new vars can no longer be registered

    //! Collates all messages and clears writers_
    void _merge_boards(void);
};

}}  // namespace flame::msg
#endif  // MB__MESSAGE_BOARD_HPP_
