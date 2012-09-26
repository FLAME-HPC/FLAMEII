/*!
 * \file src/mb/board_writer.hpp
 * \author Shawn Chin
 * \date September 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief BoardWriter instance. Used staging object for thread-specific posts
 */
#ifndef MB__BOARD_WRITER_HPP
#define MB__BOARD_WRITER_HPP
#include <string>
#include "boost/function.hpp"
#include "boost/ptr_container/ptr_map.hpp"
#include "mem/vector_wrapper.hpp"
#include "type_validator.hpp"
#include "message_board.hpp"

namespace flame { namespace mb {

class Message;  // forward declaration

typedef boost::shared_ptr<Message> MessageHandle;
typedef flame::mem::VectorWrapperBase GenericVector;
typedef boost::function<void (Message*)> MessagePostCallback;

//! Map container used to store memory vectors
typedef boost::ptr_map<std::string, GenericVector> MemoryMap;

class BoardWriter : public TypeValidator {
  friend class MessageBoard;

  public:
    //! Returns a Message instance that can .Post() to this writer
    MessageHandle GetMessage(void);

    //! Callback function for storing posted messages
    void PostCallback(Message* msg);

    //! Return the number of messages posted so far
    size_t GetCount(void);

  protected:
    // Limit constructor to MessageBoard
    explicit BoardWriter(const std::string message_name);
    // Limit var registration to MessageBoard
    void RegisterVar(std::string var_name, GenericVector* vec);
    // MessageBoard needs access to messages
    MemoryMap mem_map_;  //! map to assign VectorWrapper to var names

  private:
    size_t count_;
    std::string msg_name_;  //! Message name
};

}}  // namespace flame::mb
#endif  // MB__BOARD_WRITER_HPP
