/*!
 * \file flame2/mb/board_writer.hpp
 * \author Shawn Chin
 * \date November 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Implementation of BoardWriter class
 */
#include "board_writer.hpp"

namespace flame { namespace mb2 {

size_t BoardWriter::GetCount(void) {
  return _data->size();
}

bool BoardWriter::IsConnected(void) {
  return _connected;
}

void BoardWriter::Disconnect(void) {
  _connected = false;
}
    
}}  // namespace flame::mb2
