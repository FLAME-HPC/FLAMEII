/*!
 * \file tests/mb/test_board_writer.cpp
 * \author Shawn Chin
 * \date November 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Test suite for message boards
 */
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "flame2/exceptions/all.hpp"
#include "flame2/mb/message_board.hpp"

BOOST_AUTO_TEST_SUITE(MB2Module)

namespace mb = flame::mb;
namespace e = flame::exceptions;

typedef boost::scoped_ptr<mb::MessageBoard> board_ptr_type;

BOOST_AUTO_TEST_CASE(mb_single_writer) {
  // Create message board of type "int"
  board_ptr_type board(mb::MessageBoard::create<int>("msg1"));

  // Board starts off empty
  BOOST_CHECK_EQUAL(board->GetCount(), (size_t)0);

  // Sync would be a NOOP
  board->Sync();
  BOOST_CHECK_EQUAL(board->GetCount(), (size_t)0);

  // Get a writer
  mb::MessageBoard::writer writer = board->GetBoardWriter();
  BOOST_CHECK(writer->IsConnected());

  // Try posting invalid type
  BOOST_CHECK_THROW(writer->Post<double>(0.2), e::invalid_type);

  // Post 4 messages
  writer->Post<int>(1);
  writer->Post<int>(2);
  writer->Post<int>(3);
  writer->Post<int>(4);

  // All messages still in writer before sync
  BOOST_CHECK_EQUAL(writer->GetCount(), (size_t)4);
  BOOST_CHECK_EQUAL(board->GetCount(), (size_t)0);

  // After sync, messages in board, writer disconnected
  board->Sync();
  BOOST_CHECK(!writer->IsConnected());
  BOOST_CHECK_EQUAL(writer->GetCount(), (size_t)0);
  BOOST_CHECK_EQUAL(board->GetCount(), (size_t)4);

  // Try posting to locked board
  BOOST_CHECK_THROW(writer->Post<int>(5), e::invalid_operation);

  // Another sync shouldn't make any difference
  board->Sync();
  BOOST_CHECK_EQUAL(board->GetCount(), (size_t)4);
}

BOOST_AUTO_TEST_CASE(mb_multiple_writer) {
  // Create message board of type "int"
  board_ptr_type board(mb::MessageBoard::create<int>("msg1"));

  // Get writers
  mb::MessageBoard::writer writer1 = board->GetBoardWriter();
  mb::MessageBoard::writer writer2 = board->GetBoardWriter();
  mb::MessageBoard::writer writer3 = board->GetBoardWriter();
  BOOST_CHECK_EQUAL(board->GetCount(), (size_t)0);
  BOOST_CHECK_EQUAL(writer1->GetCount(), (size_t)0);
  BOOST_CHECK_EQUAL(writer2->GetCount(), (size_t)0);
  BOOST_CHECK_EQUAL(writer3->GetCount(), (size_t)0);

  // write to writer2
  writer2->Post<int>(21);

  // write to writer3
  writer3->Post<int>(31);
  writer3->Post<int>(32);
  writer3->Post<int>(33);
  writer3->Post<int>(34);

  BOOST_CHECK_EQUAL(board->GetCount(), (size_t)0);
  BOOST_CHECK_EQUAL(writer1->GetCount(), (size_t)0);
  BOOST_CHECK_EQUAL(writer2->GetCount(), (size_t)1);
  BOOST_CHECK_EQUAL(writer3->GetCount(), (size_t)4);

  // sync
  board->Sync();
  BOOST_CHECK_EQUAL(board->GetCount(), (size_t)5);
  BOOST_CHECK(!writer1->IsConnected());
  BOOST_CHECK(!writer2->IsConnected());
  BOOST_CHECK(!writer3->IsConnected());
  BOOST_CHECK_EQUAL(writer1->GetCount(), (size_t)0);
  BOOST_CHECK_EQUAL(writer2->GetCount(), (size_t)0);
  BOOST_CHECK_EQUAL(writer3->GetCount(), (size_t)0);
}

BOOST_AUTO_TEST_SUITE_END()
