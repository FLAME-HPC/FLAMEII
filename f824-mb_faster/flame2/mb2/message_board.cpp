/*!
 * \file flame2/mb/message_board.cpp
 * \author Shawn Chin
 * \date November 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Implementation of MessageBoard class
 */
#include "message_board.hpp"

namespace flame { namespace mb2 {

void MessageBoard::Clear(void) {
  _DeleteWriters();
  _data.clear();
}

// Number of messages that "has been synched"
size_t MessageBoard::GetCount(void) {
  return _data->size();
}

BoardWriter::handle MessageBoard::GetBoardWriter(void) {
  BoardWriter::handle writer(new BoardWriter(_writer_template->get()));
  _writers.push_back(writer);
  return writer;
}

void MessageBoard::Sync(void) {
  size_t num_messages = GetCount();
  
  // Determine new message total based on messages in writers
  WriterVector::iterator iter;
  for (iter = _writers.begin(); iter != _writers.end(); ++iter) {
    (*iter)->Disconnect();  // no more writing into this board
    num_messages += (*iter)->GetCount();
  }

  // Resize memory to fit in all messages from writers
  _data->reserve(num_messages);

  // Copy messages from writers into board
  WriterVector::iterator iter;
  for (iter = _writers.begin(); iter != _writers.end(); ++iter) {
    _data->Extend((*iter)->_data);
  }

  // discard all writers. We don't use _DeleteWriters() as we've already
  // disconnected the writers. All that's left to do is clear the writer vector.
  _writers->clear();
}

void MessageBoard::_DeleteWriters(void) {
  // First, disconnect all writers so clients holding a handle to the writer
  // can no longer post to the board
  WriterVector::iterator iter;
  for (iter = _writers.begin(); iter != _writers.end(); ++iter) {
    (*iter)->Disconnect();
  }

  // Clear the writer vector
  _writers->clear();
}

}}  // namespace flame::mb2
