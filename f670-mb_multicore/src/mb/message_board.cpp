/*!
 * \file FILENAME
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief DESCRIPTION
 */
#include "message_board.hpp"
namespace flame { namespace mb {

MessageBoard::MessageBoard(const std::string message_name)
  : msg_name(message_name), finalised_(false) {}

void MessageBoard::_sync(void) {}  // noop for base class

void MessageBoard::Sync(void) {
  // merge contents from all write boards

  // run custom sync actions if called from derived class
  _sync();

  // Run actions for all filter plugins
}



}}  // namespace flame::mb
