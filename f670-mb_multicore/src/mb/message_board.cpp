/*!
 * \file src/mb/message_board.cpp
 * \author Shawn Chin
 * \date September 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief MessageBoard
 */
#include <cassert>
#include <string>
#include "boost/foreach.hpp"
#include "message_board.hpp"
#include "mem/vector_wrapper.hpp"
#include "board_writer.hpp"

namespace flame { namespace mb {

MessageBoard::MessageBoard(const std::string message_name)
  : count_(0), msg_name_(message_name), finalised_(false) {}

//! Returns the number of messages
size_t MessageBoard::GetCount(void) {
#ifdef TESTBUILD
  // When built for testing, validate the size of all data vectors
  BOOST_FOREACH(const MemoryMap::value_type &p, mem_map_) {
    if (p.second->size() != count_) {
      throw flame::exceptions::flame_exception("inconsistent vector sizes");
    }
  }
#endif

  return count_;
}

void MessageBoard::_sync(void) {}  // noop for base class

void MessageBoard::Sync(void) {
  // merge contents from all write boards
  _merge_boards();

  // run custom sync actions if called from derived class
  _sync();

  // Run actions for all filter plugins
}

BoardWriterHandle MessageBoard::GetBoardWriter(void) {
  // board definition should no longer be modified
  finalised_ = true;

  // create new board and give ownership of obj to writers_.
  BoardWriterHandle b = BoardWriterHandle(new BoardWriter(msg_name_));
  writers_.push_back(b);

  // for each board variable, create an empty clone of data vectors
  BOOST_FOREACH(const MemoryMap::value_type &p, mem_map_) {
    b->RegisterVar(p.first, p.second->clone_empty());
  }

  return b;
}

void MessageBoard::_merge_boards(void) {
  size_t message_count = 0;

  // determine number of new messages
  WriterVector::iterator iter = writers_.begin();
  for (; iter != writers_.end(); ++iter) {
    message_count += (*iter)->GetCount();
  }

  // update each data vector based on contents in boardwriters
  message_count += count_;  // determine new size
  BOOST_FOREACH(const MemoryMap::value_type &p, mem_map_) {
    assert(p.second->size() == count_);
    p.second->reserve(message_count);  // reserve memory in internal vectors

    for (iter = writers_.begin(); iter != writers_.end(); ++iter) {
      // append contents of writers_.mem_map_[var] to mem_map[var]
      p.second->Extend(&(*iter)->mem_map_.at(p.first));
    }
  }

  // update internal message counter
  count_ = message_count;

  // delete all writers
  writers_.clear();
}

}}  // namespace flame::mb
