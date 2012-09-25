/*!
 * \file FILENAME
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief DESCRIPTION
 */
#include <boost/foreach.hpp>
#include "message_board.hpp"
#include "board_writer.hpp"
namespace flame { namespace mb {

MessageBoard::MessageBoard(const std::string message_name)
  : msg_name_(message_name), finalised_(false) {}

void MessageBoard::_sync(void) {}  // noop for base class

void MessageBoard::Sync(void) {
  // merge contents from all write boards

  // run custom sync actions if called from derived class
  _sync();

  // Run actions for all filter plugins
}

BoardWriter* MessageBoard::GetBoardWriter(void) {
  // board definition should no longer be modified
  finalised_ = true;

  // create new board and give ownership of obj to writers_.
  BoardWriter* b = new BoardWriter(msg_name_);
  writers_.push_back(b);

  // for each board variable, create an empty clone of data vectors
  BOOST_FOREACH(const MemoryMap::value_type &p, mem_map_) {
    b->RegisterVar(p.first, p.second->clone_empty());
  }

  return b;
}

}}  // namespace flame::mb
