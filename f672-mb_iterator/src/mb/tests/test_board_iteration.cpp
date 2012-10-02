/*!
 * \file src/mb/tests/test_board_writer.cpp
 * \author Shawn Chin
 * \date September 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Test suite for message boards
 */
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "../message_board.hpp"
#include "../message_iterator.hpp"
#include "../message.hpp"

BOOST_AUTO_TEST_SUITE(MBModule)

namespace e = flame::exceptions;
namespace mb = flame::mb;
namespace mem = flame::mem;

BOOST_AUTO_TEST_CASE(mb_iterate_empty) {
  mb::MessageBoard board = mb::MessageBoard("msg1");
  board.RegisterVar<int>("int");
  board.RegisterVar<double>("dbl");

  BOOST_CHECK_EQUAL(board.GetCount(), (size_t)0);

  mb::MessageBoard::Iterator iter = board.GetMessageIterator();
  BOOST_CHECK_EQUAL(iter->GetCount(), (size_t)0);

  /* board should be finalised once iterator is requested */
  BOOST_CHECK_THROW(board.RegisterVar<int>("i2"), e::logic_error);

  /* AtEnd() == true for empty iterator */
  BOOST_CHECK_EQUAL(iter->AtEnd(), true);

  /* so should a Get() operation */
  BOOST_CHECK_THROW(iter->Get<int>("int"), e::out_of_range);

  /* so should GetMessage() */
  BOOST_CHECK_THROW(iter->GetMessage(), e::out_of_range);

  /* Next() return false */
  BOOST_CHECK_EQUAL(iter->Next(), false);

  /* Rewind should not break anything */
  iter->Rewind();

  /* should work even for boards with no vars */
  mb::MessageBoard board2 = mb::MessageBoard("msg2");
  mb::MessageBoard::Iterator iter2 = board2.GetMessageIterator();
  BOOST_CHECK_EQUAL(iter2->GetCount(), (size_t)0);

}


BOOST_AUTO_TEST_CASE(mb_iterate_simple) {
  mb::MessageBoard board = mb::MessageBoard("msg1");
  board.RegisterVar<int>("int");

  /* Post messages and Sync */
  mb::MessageBoard::Writer writer = board.GetBoardWriter();
  mb::MessageHandle msg = writer->GetMessage();
  msg->Set<int>("int", 1); msg->Post();
  msg->Set<int>("int", 2); msg->Post();
  msg->Set<int>("int", 3); msg->Post();
  msg->Set<int>("int", 4); msg->Post();
  msg->Set<int>("int", 5); msg->Post();
  board.Sync();

  mb::MessageBoard::Iterator iter = board.GetMessageIterator();
  BOOST_CHECK_EQUAL(iter->GetCount(), (size_t)5);
  BOOST_CHECK_EQUAL(iter->AtEnd(), false);

  for (int i = 1; i <= 5; ++i) {
    BOOST_REQUIRE_EQUAL(iter->AtEnd(), false);

    /* check direct access method */
    BOOST_CHECK_EQUAL(iter->Get<int>("int"), i);

    /* check GetMessage() */
    msg = iter->GetMessage();
    BOOST_CHECK_EQUAL(msg->Get<int>("int"), i);

    /* Next message */
    BOOST_CHECK_EQUAL(iter->Next(), ((i == 5) ? false : true));
  }

  /* check Rewind() */
  BOOST_CHECK_EQUAL(iter->AtEnd(), true);
  iter->Rewind();
  BOOST_CHECK_EQUAL(iter->AtEnd(), false);

  /* Iterate with rc from Next() as loop term flag */
  int i = 1;
  do {
    BOOST_CHECK_EQUAL(iter->Get<int>("int"), i);
    ++i;
  } while (iter->Next());

  /* Standard loop */
  for (i = 1, iter->Rewind(); !iter->AtEnd(); iter->Next(), ++i) {
    BOOST_CHECK_EQUAL(iter->Get<int>("int"), i);
  }
}

BOOST_AUTO_TEST_SUITE_END()
