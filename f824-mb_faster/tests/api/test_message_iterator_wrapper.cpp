/*!
 * \file tests/api/test_message_iterator_wrapper.cpp
 * \author Shawn Chin
 * \date November 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Test suite for the user API (C++)
 */
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/scoped_ptr.hpp>
#include "flame2/exceptions/api.hpp"
#include "flame2/exceptions/all.hpp"
#include "flame2/mb/message_board.hpp"
#include "flame2/api/message_iterator_wrapper.hpp"

BOOST_AUTO_TEST_SUITE(APIModule)

namespace e = flame::exceptions;
namespace mb = flame::mb;

typedef boost::scoped_ptr<mb::MessageBoard> board_ptr_type;

BOOST_AUTO_TEST_CASE(api_test_null_iterator_wrapper) {
  flame::api::MessageIteratorWrapper iterator;
  BOOST_CHECK_THROW(iterator.AtEnd(), e::flame_api_exception);
  BOOST_CHECK_THROW(iterator.Next(), e::flame_api_exception);
  BOOST_CHECK_THROW(iterator.GetCount(), e::flame_api_exception);
  BOOST_CHECK_THROW(iterator.Rewind(), e::flame_api_exception);
  BOOST_CHECK_THROW(iterator.Randomise(), e::flame_api_exception);
  BOOST_CHECK_THROW(iterator.GetMessage<int>(), e::flame_api_exception);
}

BOOST_AUTO_TEST_CASE(api_test_iterator_wrapper_empty) {
  board_ptr_type board(mb::MessageBoard::create<int>("msg1"));
  flame::api::MessageIteratorWrapper  iterator(board->GetMessages());
  BOOST_CHECK_EQUAL(iterator.GetCount(), (size_t)0);
  BOOST_CHECK_EQUAL(iterator.AtEnd(), true);
  BOOST_CHECK_THROW(iterator.GetMessage<int>(), e::flame_api_exception);
  BOOST_CHECK_THROW(iterator.Next(), e::flame_api_exception);  // out of range
  iterator.Rewind();  // should not raise any exceptions

  // randomisation not yet implemented
  BOOST_CHECK_THROW(iterator.Randomise(), e::flame_api_exception);
}

BOOST_AUTO_TEST_CASE(api_iterate_iterator_wrapper_simple) {
  board_ptr_type board(mb::MessageBoard::create<int>("msg1"));

  // Post messages and sync
  mb::MessageBoard::writer writer = board->GetBoardWriter();
  for (int i = 0; i < 10; ++i) writer->Post<int>(i);
  board->Sync();

  // Iterate and check
  flame::api::MessageIteratorWrapper  iterator(board->GetMessages());
  BOOST_CHECK_EQUAL(iterator.GetCount(), (size_t)10);
  BOOST_CHECK_EQUAL(iterator.AtEnd(), false);

  // Manually iterate to 10 and check iter routines
  for (int i = 0; i < 10; ++i) {
    BOOST_REQUIRE_EQUAL(iterator.AtEnd(), false);
    BOOST_CHECK_EQUAL(iterator.GetMessage<int>(), i);
    BOOST_CHECK_EQUAL(iterator.Next(), ((i == 9) ? false : true));
  }

  // Rewind
  BOOST_CHECK_EQUAL(iterator.AtEnd(), true);
  iterator.Rewind();
  BOOST_CHECK_EQUAL(iterator.AtEnd(), false);
  
  /* Iterate with rc from Next() as loop term flag */
  int i = 0;
  do {
    BOOST_CHECK_EQUAL(iterator.GetMessage<int>(), i);
    ++i;
  } while (iterator.Next());

  /* Standard loop */
  for (i = 0, iterator.Rewind(); !iterator.AtEnd(); iterator.Next(), ++i) {
    BOOST_CHECK_EQUAL(iterator.GetMessage<int>(), i);
  }
}

BOOST_AUTO_TEST_SUITE_END()

