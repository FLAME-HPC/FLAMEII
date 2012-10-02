/*!
 * \file src/mb/board_writer.cpp
 * \author Shawn Chin
 * \date September 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Implementation of BoardWriter
 */
#include <string>
#include <utility>  // std::pair
#include <cassert>
#include "boost/bind.hpp"
#include "boost/foreach.hpp"
#include "exceptions/all.hpp"
#include "message.hpp"
#include "board_writer.hpp"
namespace flame { namespace mb {

/*!
 * \brief BoardWriter constructor
 * \param[in] message_name Name of message associated with this writrer
 *
 * Initialises count_ and msg_name_
 */
BoardWriter::BoardWriter(const std::string message_name, TypeValidator* tv)
  : count_(0), msg_name_(message_name), validator_(tv), connected_(true) {}

/*!
 * \brief Returns the number of messages posted so far
 * \return Number of posted messages
 */
size_t BoardWriter::GetCount(void) {
  return count_;
}

/*!
 * \brief Registers a message var and assign it to the given vector
 * \param[in] var_name Variable name
 * \param[in] vec Pointer to an empty VectorWrapper.
 *
 * vec must be assigned from the output of VectorWrapperBase::clone_empty()
 * from the main message vector so that it is of the correct type.
 * BoardWriter will take over ownership of the vector object and will deallocate
 * it when the object is destroyed. Calling routines should not access the
 * \c vec pointer and should certainly not deallocate the object.
 *
 * The name and type is also registered with the TypeValidator interface.
 */
void BoardWriter::RegisterVar(std::string var_name, GenericVector* vec) {
  std::pair<MemoryMap::iterator, bool> ret;
  ret = mem_map_.insert(var_name, vec);
  if (!ret.second) {  // if replacement instead of insertion
    throw flame::exceptions::logic_error("variable already registered");
  }
}

/*!
 * \brief Creates a Message instance and returns its handle
 * \return A handle (shared_ptr) to a Message instance
 * A Message object is created and wrapped in a boost::shared_ptr so that its
 * lifetime is automatically managed.
 *
 * The BoardWriter::PostCallback class method is assign as the callback function
 * for Message::Post (which is how the Message is associated with a Board Writer
 * and in turn the Message Board).
 */
MessageHandle BoardWriter::GetMessage(void) {
  MessageHandle msg = MessageHandle(new Message(validator_));
  msg->AssignPostCallback(boost::bind(&BoardWriter::PostCallback, this, _1));
  return msg;
}

/*!
 * \brief Callback function for handling a Message::Post()
 * \param[in] msg Pointer to message instance
 *
 * Unless run-time type checking is disabled, the Message is first inspected
 * to ensure that all variables are set. (The type is not checked as we expect
 * type-checking to be done by Message::Set()).
 *
 * Throws flame::exceptions::insufficient_data if not all variables in the
 * Message has been set.
 *
 * If everything is in order, we iterate through the variables and append all
 * values from the Message to the internal vectors.
 *
 * Throws flame::exceptions::invalid_operation if the writer is no longer
 * connected to the parent board. This will occur if Sync() is called on the
 * parent board or if the board is deleted.
 *
 * \note The internal maps in BoardWriter and Message MUST be ordered and of the
 * same container type. For efficiency, we iterate through their entries
 * concurrently and expect the keys to be returned in the same order.
 */
void BoardWriter::PostCallback(Message* msg) {
  if (!connected_) {
    throw flame::exceptions::invalid_operation("Writer no longer connected");
  }

  // First, check that all values are value so we don't end up within
  // inconsistent data vectors on error
#ifndef DISABLE_RUNTIME_TYPE_CHECKING
  BOOST_FOREACH(const MemoryMap::value_type &p, mem_map_) {
    if (msg->data_.find(p.first) == msg->data_.end()) {
      throw flame::exceptions::insufficient_data("not all values defined");
    }
    // type should already be checked on Message.Set(). It will also be
    // checked in mem_map_[].push_back(...);
  }
#endif

  // Both mem_map_ and msg->data_ should have the set of keys so we should
  // be able to iterate through both simultaneously and get the same keys
  MemoryMap::iterator m_iter = mem_map_.begin();
  Message::DataMap::iterator d_iter = msg->data_.begin();

  for (; m_iter != mem_map_.end(); ++m_iter, ++d_iter) {
    assert(m_iter->first == d_iter->first);  // ensure that keys are the same
    m_iter->second->push_back(d_iter->second);  // append message to board
  }

  // increment msg counter
  ++count_;
}

//! Indicate if writer is still connected to the board
bool BoardWriter::IsConnected(void) {
  return connected_;
}

//! Sets flag to indicate that writer is not disconnected from the board
void BoardWriter::Disconnect(void) {
  connected_ = false;
}

}}  // namespace flame::mb
