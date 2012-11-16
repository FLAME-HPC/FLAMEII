/*!
 * \file tests/mb2/test_message_iteration.cpp
 * \author Shawn Chin
 * \date November 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Test suite for message boards
 */
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/scoped_ptr.hpp>
#include "flame2/exceptions/all.hpp"
#include "flame2/mb/message_board.hpp"

BOOST_AUTO_TEST_SUITE(MBModule)

namespace e = flame::exceptions;
namespace mb = flame::mb;
namespace mem = flame::mem;

typedef boost::scoped_ptr<mb::MessageBoard> board_ptr_type;

BOOST_AUTO_TEST_CASE(mb_iterate_empty) {
  board_ptr_type board(mb::MessageBoard::create<int>("msg1"));
  BOOST_CHECK_EQUAL(board->GetCount(), (size_t)0);

  // Check requirements for iterator of empty board
  mb::MessageBoard::iterator iter = board->GetMessages();
  BOOST_CHECK_EQUAL(iter->GetCount(), (size_t)0);
  BOOST_CHECK_EQUAL(iter->AtEnd(), true);
  BOOST_CHECK_THROW(iter->Get<int>(), e::out_of_range);
  BOOST_CHECK_THROW(iter->Next(), e::out_of_range);
  iter->Rewind();  // should not raise any exceptions

  BOOST_CHECK_THROW(iter->Get<double>(), e::invalid_type);
}

BOOST_AUTO_TEST_CASE(mb_iterate_simple) {
  board_ptr_type board(mb::MessageBoard::create<int>("msg1"));

  // Post messages and sync
  mb::MessageBoard::writer writer = board->GetBoardWriter();
  for (int i = 0; i < 10; ++i) writer->Post<int>(i);
  board->Sync();

  // Iterate and check
  mb::MessageBoard::iterator iter = board->GetMessages();
  BOOST_CHECK_EQUAL(iter->GetCount(), (size_t)10);
  BOOST_CHECK_EQUAL(iter->AtEnd(), false);

  // Manually iterate to 10 and check iter routines
  for (int i = 0; i < 10; ++i) {
    BOOST_REQUIRE_EQUAL(iter->AtEnd(), false);
    BOOST_CHECK_EQUAL(iter->Get<int>(), i);
    BOOST_CHECK_EQUAL(iter->Next(), ((i == 9) ? false : true));
  }

  // Rewind
  BOOST_CHECK_EQUAL(iter->AtEnd(), true);
  iter->Rewind();
  BOOST_CHECK_EQUAL(iter->AtEnd(), false);
  
  /* Iterate with rc from Next() as loop term flag */
  int i = 0;
  do {
    BOOST_CHECK_EQUAL(iter->Get<int>(), i);
    ++i;
  } while (iter->Next());

  /* Standard loop */
  for (i = 0, iter->Rewind(); !iter->AtEnd(); iter->Next(), ++i) {
    BOOST_CHECK_EQUAL(iter->Get<int>(), i);
  }
}

BOOST_AUTO_TEST_SUITE_END()
