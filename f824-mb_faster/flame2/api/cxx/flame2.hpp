/*!
 * \file flame2/api/cxx/flame2.hpp
 * \author Shawn Chin
 * \date November 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief C++ API for FLAME2 Models
 */
#ifndef FLAME2__API__CXX__FLAME2_HPP_
#define FLAME2__API__CXX__FLAME2_HPP_
#include "flame2/mb/message_board.hpp"

#define FLAME_AGENT_ALIVE 0
#define FLAME_AGENT_DEAD  1

namespace flame2 { namespace api {

// Expose flame::mb::MessageBoard::iterator as flame::api::MessageIterator
namespace internal {
  using flame::mb::MessageBoard;
  typedef MessageBoard::iterator  MessageIterator;
}
using internal::MessageIterator;

}}  // namespace flame2::api

#endif  // FLAME2__API__CXX__FLAME2_HPP_
