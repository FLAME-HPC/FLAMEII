/*!
 * \file src/mb/board_writer.hpp
 * \author Shawn Chin
 * \date September 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Declaration of BoardWriter
 */
#ifndef MB__BOARD_WRITER_HPP
#define MB__BOARD_WRITER_HPP
#include <string>
#include "boost/ptr_container/ptr_map.hpp"
#include "mem/vector_wrapper.hpp"
#include "type_validator.hpp"
#include "message_board.hpp"

namespace flame { namespace mb {

/*!
 * \brief Proxy object used to post messages to a board
 *
 * Messages are not posted to the board directly as concurrent posts will
 * incur too much locking overheads. Instead, each thread can be assigned
 * a BoardWrite instance into which messages are posted. These messages
 * are eventually incorporated into the board by MessageBoard::Sync().
 *
 * This class should not be manually instantiated as it is only useful when
 * associated with a MessageBoard. The constructor is therefore protected
 * and callable only by MessageBoard.
 *
 * Inherits the TypeValidator interface so it can be used to validate
 * the datatype of message variables (used by Message::Set()).
 */
class BoardWriter : public TypeValidator {
  //! Only MessageBoard can call the constructor
  friend class MessageBoard;

  public:
    //! Returns a Message instance that can .Post() to this writer
    MessageHandle GetMessage(void);

    //! Callback function for storing posted messages
    void PostCallback(Message* msg);

    //! Return the number of messages posted so far
    size_t GetCount(void);

  protected:
    //! Constructor. Limited to friend classes
    explicit BoardWriter(const std::string message_name);

    //! Var registration. Limited to friend classes
    void RegisterVar(std::string var_name, GenericVector* vec);

    //! Internal data structure. Accessible by friend classes
    MemoryMap mem_map_;

  private:
    size_t count_;  //! Number of messages posted
    std::string msg_name_;  //! Message name
};

}}  // namespace flame::mb
#endif  // MB__BOARD_WRITER_HPP
