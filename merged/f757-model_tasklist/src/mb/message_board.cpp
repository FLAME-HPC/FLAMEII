/*!
 * \file src/mb/message_board.cpp
 * \author Shawn Chin
 * \date September 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Implementation of MessageBoard
 */
#include <cassert>
#include <string>
#include "boost/foreach.hpp"
#include "message_board.hpp"
#include "mem/vector_wrapper.hpp"
#include "board_writer.hpp"

namespace flame { namespace mb {

/*!
 * \brief Constructor for MessageBoard
 * \param[in] message_name The name of the message held by the board
 *
 * Initialiases internal parameters count_, msg_name_ and finalised_.
 */
MessageBoard::MessageBoard(const std::string message_name)
  : count_(0), msg_name_(message_name), finalised_(false) {}

/*!
 * \brief Returns the number of messages
 * \return Number of messages in the board
 *
 * Returns the internal counter, count_. We're assuming that the internal
 * vectors hand sizes that are consistent with this counter.
 *
 * In the test build, all vectors are inspected to ensure that their sizes
 * match the counter.
 */
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

/*!
 * \brief Additional operations to run during a Sync()
 *
 * Derived classes should overload this to define custom operations triggered
 * during a Sync().
 *
 * This function is called after all writers are merged and the internal
 * vectors contain the complete collection of messages, but before callback
 * functions are triggered for filter plugins.
 *
 * In this base class, this function does nothing.
 */
void MessageBoard::_sync(void) {}  // noop for base class

/*!
 * \brief Performs a message board sync
 *
 * The following operations are called in order:
 * - Calls _merge_boards() to collate all posted messages
 * - Calls _sync() to perform custom operations from derived classes
 * - Runs the prep routines for all filter plugins so they can prepare their
 *   datastructure based on the new contents of the board
 */
void MessageBoard::Sync(void) {
  // merge contents from all write boards
  _merge_boards();

  // run custom sync actions if called from derived class
  _sync();

  // Run actions for all filter plugins
  // _filter_prep_callbacks();
}

/*!
 * \brief Returns a handle to a BoardWriter
 * \return Shared pointer to a BoardWriter instance
 *
 * Initiases a new BoardWriter with the list of message variables
 * and returns a boost::shared_ptr as its handle.
 *
 * All instances of BoardWriters are stored in writers_ and will be
 * deallocated once they are merged (by _merge_board()) or when
 * the class instance is destroyed.
 */
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

/*!
 * \brief Populate vectors with contents from board writers
 *
 * Once completed, writers_ is cleared and all instances of BoardWriters
 * will be deallocated.
 *
 * \todo (lsc) Consider the possibility of reusing BoardWriter instances.
 * This will involve checking that no other instances of the shared_ptr
 * are being held and is therefore safe to be reissued.
 */
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
