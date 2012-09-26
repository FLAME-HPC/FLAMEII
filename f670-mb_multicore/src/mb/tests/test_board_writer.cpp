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
#include "../message.hpp"

BOOST_AUTO_TEST_SUITE(MBModule)

namespace e = flame::exceptions;
namespace mb = flame::mb;
namespace mem = flame::mem;

BOOST_AUTO_TEST_CASE(mb_single_writer) {
  mb::MessageBoard board = mb::MessageBoard("msg1");
  board.RegisterVar<int>("int");
  board.RegisterVar<double>("dbl");

  BOOST_CHECK_EQUAL(board.GetCount(), (size_t)0);

  /* should be a NOOP */
  board.Sync();
  BOOST_CHECK_EQUAL(board.GetCount(), (size_t)0);

  /* get a writer */
  mb::BoardWriterHandle writer = board.GetBoardWriter();
  /* board should not be finalised. no more new vars */
  BOOST_CHECK_THROW(board.RegisterVar<int>("i2"), e::logic_error);

  /* get a message handle so we can post new messages */
  mb::MessageHandle msg = writer->GetMessage();

  /* we should not be able to post yet. Not all vars set */
  BOOST_CHECK_THROW(msg->Post(), e::insufficient_data);

  /* Setting unknown variable */
  BOOST_CHECK_THROW(msg->Set<int>("unknown", 1), e::invalid_variable);

  /* Setting variable with wrong type */
  BOOST_CHECK_THROW(msg->Set<int>("dbl", 1), e::invalid_type);

  /* The following should work */
  msg->Set<int>("int", 1);
  BOOST_CHECK_THROW(msg->Post(), e::insufficient_data);  // still not enough
  msg->Set<double>("dbl", 0.1);

  /* We should not be able to post */
  msg->Post();
  BOOST_CHECK_EQUAL(writer->GetCount(), (size_t)1);

  /* We should be able to post again (duplicate message) */
  msg->Post();
  BOOST_CHECK_EQUAL(writer->GetCount(), (size_t)2);

  /* Reset message. Can no longer post.*/
  msg->Clear();
  BOOST_CHECK_THROW(msg->Post(), e::insufficient_data);

  /* Not yet synched, Board should still be empty */
  BOOST_CHECK_EQUAL(board.GetCount(), (size_t)0);

  /* sync */
  board.Sync();
  BOOST_CHECK_EQUAL(board.GetCount(), (size_t)2);

  /* another sync should not make a difference */
  board.Sync();
  BOOST_CHECK_EQUAL(board.GetCount(), (size_t)2);
}

BOOST_AUTO_TEST_CASE(mb_multiple_writers) {
  mb::MessageBoard board = mb::MessageBoard("msg1");
  board.RegisterVar<int>("int");
  board.RegisterVar<double>("dbl");
  BOOST_CHECK_EQUAL(board.GetCount(), (size_t)0);

  mb::BoardWriterHandle writer;
  mb::MessageHandle msg;

  /* Get a writer, don't write */
  writer = board.GetBoardWriter();
  BOOST_CHECK_EQUAL(writer->GetCount(), (size_t)0);

  /* Get another writer, post one message */
  writer = board.GetBoardWriter();
  msg = writer->GetMessage();
  msg->Set<int>("int", 10);
  msg->Set<double>("dbl", 0.1);
  msg->Post();
  BOOST_CHECK_EQUAL(writer->GetCount(), (size_t)1);

  /* Get another writer, post 4 messages */
  writer = board.GetBoardWriter();
  msg = writer->GetMessage();
  msg->Set<int>("int", 20);
  msg->Set<double>("dbl", 0.2);
  msg->Post();
  msg->Post();
  msg->Set<int>("int", 40); /* change data */
  msg->Post();
  msg->Set<double>("dbl", 0.04); /* change data */
  msg->Post();
  BOOST_CHECK_EQUAL(writer->GetCount(), (size_t)4);

  /* before sync, board should still be empty */
  BOOST_CHECK_EQUAL(board.GetCount(), (size_t)0);

  /* after sync, 5 messages */
  board.Sync();
  BOOST_CHECK_EQUAL(board.GetCount(), (size_t)5);

  /* sync again, no change */
  board.Sync();
  BOOST_CHECK_EQUAL(board.GetCount(), (size_t)5);

  /* get another writer and post 5 more messages */
  writer = board.GetBoardWriter();
  msg = writer->GetMessage();
  msg->Set<int>("int", 10);
  msg->Set<double>("dbl", 0.1);
  msg->Post();
  msg->Post();
  msg->Post();
  msg->Post();
  msg->Post();
  BOOST_CHECK_EQUAL(writer->GetCount(), (size_t)5);

  /* after sync, 10 messages */
  board.Sync();
  BOOST_CHECK_EQUAL(board.GetCount(), (size_t)10);

  /* sync again, no change */
  board.Sync();
  BOOST_CHECK_EQUAL(board.GetCount(), (size_t)10);
}

BOOST_AUTO_TEST_CASE(mb_concurrent_writers) {
  mb::MessageBoard board = mb::MessageBoard("msg1");
  board.RegisterVar<int>("int");
  board.RegisterVar<double>("dbl");
  BOOST_CHECK_EQUAL(board.GetCount(), (size_t)0);

  mb::BoardWriterHandle writer1, writer2;
  mb::MessageHandle msg1, msg2, msg2b;

  writer1 = board.GetBoardWriter();
  writer2 = board.GetBoardWriter();

  msg1 = writer1->GetMessage();
  msg2 = writer2->GetMessage();
  msg2b = writer2->GetMessage();

  msg1->Set<int>("int", 10);
  msg1->Set<double>("dbl", 0.1);

  msg2->Set<int>("int", 20);
  msg2->Set<double>("dbl", 0.2);

  msg2b->Set<int>("int", 30);
  msg2b->Set<double>("dbl", 0.3);

  msg1->Post();
  msg2->Post();
  msg2->Post();
  msg1->Post();
  msg1->Post();
  msg2b->Post();
  msg1->Post();

  BOOST_CHECK_EQUAL(writer1->GetCount(), (size_t)4);
  BOOST_CHECK_EQUAL(writer2->GetCount(), (size_t)3);

  board.Sync();
  BOOST_CHECK_EQUAL(board.GetCount(), (size_t)7);

}

BOOST_AUTO_TEST_SUITE_END()
