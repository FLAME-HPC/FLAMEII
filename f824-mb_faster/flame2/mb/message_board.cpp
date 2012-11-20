/*!
 * \file flame2/mb/message_board.cpp
 * \author Shawn Chin
 * \date November 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Implementation of MessageBoard class
 */
#include "flame2/config.hpp"
#include "message_board.hpp"

namespace flame { namespace mb {

void MessageBoard::Clear(void) {
  _DeleteWriters();  // Disconnects and delete all writers
  data_->clear();  // Empty internal vector of messages
}

size_t MessageBoard::GetCount(void) const {
  return data_->size();
}

// This is the only method that's protected by a mutex since multiple
// worker threads may request for a board writer at the same time.
// The actual message posting is not locked since each thread would then
// write into their own board writer and not directly into the message board
MessageBoard::writer MessageBoard::GetBoardWriter(void) {
  boost::lock_guard<boost::mutex> lock(mutex_);  // mutex guard

  // clone an empty copy of a board writer that's assigned to the correct type
  MessageBoard::writer w(writer_template_->clone_empty());
  writers_.push_back(w);  // add to collection of writers
  return w;
}

MessageBoard::iterator MessageBoard::GetMessages(void) {
  return MessageBoard::iterator(MessageIterator::create(data_.get()));
}

void MessageBoard::Sync(void) {
  size_t num_messages = GetCount();
  
  // Determine new message total based on messages in writers
  WriterVector::iterator iter;
  for (iter = writers_.begin(); iter != writers_.end(); ++iter) {
    (*iter)->Disconnect();  // no more writing into this board
    num_messages += (*iter)->GetCount();
  }

  // Resize memory to fit in all messages from writers
  data_->reserve(num_messages);

  // Copy messages from writers into board
  for (iter = writers_.begin(); iter != writers_.end(); ++iter) {
    data_->Extend((*iter)->data_.get());
    (*iter)->data_->clear();
  }

  // discard all writers. We don't use _DeleteWriters() as we've already
  // disconnected the writers. All that's left to do is clear the writer vector.
  writers_.clear();
}

void MessageBoard::_DeleteWriters(void) {
  // First, disconnect all writers so clients holding a handle to the writer
  // can no longer post to the board
  WriterVector::iterator iter;
  for (iter = writers_.begin(); iter != writers_.end(); ++iter) {
    (*iter)->Disconnect();
  }

  // Clear the writer vector
  writers_.clear();
}

}}  // namespace flame::mb
