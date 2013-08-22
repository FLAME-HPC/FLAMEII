/*!
 * \file tests/mb/test_board_manager.cpp
 * \author Shawn Chin
 * \date November 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Test suite for message boards
 */
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "flame2/mb/message_board_manager.hpp"
#include "flame2/mb/message_board.hpp"

BOOST_AUTO_TEST_SUITE(MBModule)

namespace e = flame::exceptions;
namespace mb = flame::mb;

BOOST_AUTO_TEST_CASE(mb_initialise_and_test_singleton) {
  mb::MessageBoardManager& mgr1 = mb::MessageBoardManager::GetInstance();
  mgr1.RegisterMessage<int>("TestSingleton");

  // get another instance
  mb::MessageBoardManager& mgr = mb::MessageBoardManager::GetInstance();
  BOOST_CHECK_THROW(mgr.RegisterMessage<int>("TestSingleton"), e::logic_error);
}

BOOST_AUTO_TEST_CASE(mb_manager_lifecycle) {
  mb::MessageBoardManager& mgr = mb::MessageBoardManager::GetInstance();
  mgr.RegisterMessage<double>("location");

  mb::MessageBoard::writer writer = mgr.GetBoardWriter("location");
  writer->Post<double>(1.0);
  writer->Post<double>(2.0);
  writer->Post<double>(3.0);

  BOOST_CHECK_EQUAL(mgr.GetCount("location"), (size_t)0);
  mgr.Sync("location");
  BOOST_CHECK_EQUAL(mgr.GetCount("location"), (size_t)3);

  // access using a different manager instance
  mb::MessageBoardManager& mgr2 = mb::MessageBoardManager::GetInstance();
  mb::MessageBoard::iterator iter = mgr2.GetMessages("location");
  BOOST_CHECK_EQUAL(iter->GetCount(), (size_t)3);

  for (int i = 1; !iter->AtEnd(); iter->Next(), ++i) {
    BOOST_CHECK_EQUAL(iter->Get<double>(), 1.0*i);
  }

  // TESTBUILD only routine to reset list of registered boards
  mgr.Reset();
  BOOST_CHECK_NO_THROW(mgr.RegisterMessage<double>("location"));
  BOOST_CHECK_THROW(mgr.RegisterMessage<double>("location"), e::logic_error);
  mgr.Reset();
}

BOOST_AUTO_TEST_SUITE_END()
