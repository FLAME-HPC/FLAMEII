/*!
 * \file src/mb/tests/test_board_manager.cpp
 * \author Shawn Chin
 * \date October 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Test suite for message boards
 */
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "../message_board_manager.hpp"
#include "../board_writer.hpp"
#include "../message_iterator.hpp"

BOOST_AUTO_TEST_SUITE(MBModule)

namespace e = flame::exceptions;
namespace mb = flame::mb;

BOOST_AUTO_TEST_CASE(mb_initialise_and_test_singleton) {
  mb::MessageBoardManager& mgr1 = mb::MessageBoardManager::GetInstance();
  mgr1.RegisterMessage("TestSingleton");

  // get another instance
  mb::MessageBoardManager& mgr = mb::MessageBoardManager::GetInstance();
  BOOST_CHECK_THROW(mgr.RegisterMessage("TestSingleton"), e::logic_error);
}

BOOST_AUTO_TEST_CASE(mb_manager_lifecycle) {
  mb::MessageBoardManager& mgr = mb::MessageBoardManager::GetInstance();
  mgr.RegisterMessage("location");
  mgr.RegisterMessageVar<double>("location", "x");
  mgr.RegisterMessageVar<double>("location", "y");
  mgr.RegisterMessageVar<int>("location", "age");

  mb::MessageBoard::Writer writer = mgr.GetBoardWriter("location");
  // board should be finalised
  BOOST_CHECK_THROW(mgr.RegisterMessageVar<int>("location", "age"),
                    e::logic_error);

  mb::MessageBoard::Message msg = writer->NewMessage();
  msg->Set<double>("x", 1.0);
  msg->Set<double>("y", 10.0);
  msg->Set<int>("age", 100);
  msg->Post();

  msg->Set<double>("x", 2.0);
  msg->Set<double>("y", 20.0);
  msg->Set<int>("age", 200);
  msg->Post();

  msg->Set<double>("x", 3.0);
  msg->Set<double>("y", 30.0);
  msg->Set<int>("age", 300);
  msg->Post();

  BOOST_CHECK_EQUAL(mgr.GetCount("location"), (size_t)0);
  mgr.Sync("location");
  BOOST_CHECK_EQUAL(mgr.GetCount("location"), (size_t)3);

  // access using a different manager instance
  mb::MessageBoardManager& mgr2 = mb::MessageBoardManager::GetInstance();
  mb::MessageBoard::Iterator iter = mgr2.GetMessageIterator("location");
  BOOST_CHECK_EQUAL(iter->GetCount(), (size_t)3);

  for (int i = 1; !iter->AtEnd(); iter->Next(), ++i) {
    msg = iter->GetMessage();
    BOOST_CHECK_EQUAL(msg->Get<double>("x"), 1.0*i);
    BOOST_CHECK_EQUAL(msg->Get<double>("y"), 10.0*i);
    BOOST_CHECK_EQUAL(msg->Get<int>("age"), 100*i);
  }

  // TESTBUILD only routine to reset list of registered boards
  mgr.Reset();
  BOOST_CHECK_NO_THROW(mgr.RegisterMessage("location"));
  BOOST_CHECK_THROW(mgr.RegisterMessage("location"), e::logic_error);
  mgr.Reset();
}

BOOST_AUTO_TEST_SUITE_END()
