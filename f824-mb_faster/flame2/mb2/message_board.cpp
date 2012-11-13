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

namespace flame { namespace mb2 {

void MessageBoard::Clear(void) {
  _DeleteWriters();
  data_->clear();
}

// Number of messages that "has been synched"
size_t MessageBoard::GetCount(void) const {
  return data_->size();
}

MessageBoard::writer MessageBoard::GetBoardWriter(void) {
  boost::lock_guard<boost::mutex> lock(mutex_);
  MessageBoard::writer w(writer_template_->clone_empty());
  writers_.push_back(w);
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

}}  // namespace flame::mb2
