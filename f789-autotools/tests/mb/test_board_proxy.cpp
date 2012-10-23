/*!
 * \file src/mb/tests/test_board_proxy.cpp
 * \author Shawn Chin
 * \date October 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Test suite for message boards
 */
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "mb/message_board_manager.hpp"
#include "mb/message_board.hpp"
#include "mb/board_writer.hpp"
#include "mb/message_iterator.hpp"
#include "mb/message.hpp"
#include "mb/proxy.hpp"
#include "mb/client.hpp"

BOOST_AUTO_TEST_SUITE(MBModule)

namespace e = flame::exceptions;
namespace mb = flame::mb;

BOOST_AUTO_TEST_CASE(mb_proxy_acl) {
  // Set up board manager
  mb::MessageBoardManager& mgr = mb::MessageBoardManager::GetInstance();
  mgr.RegisterMessage("msg1");
  mgr.RegisterMessageVar<double>("msg1", "x");
  mgr.RegisterMessage("msg2");
  mgr.RegisterMessageVar<double>("msg2", "x");

  // Test proxy with no read/write access
  mb::Proxy p;
  mb::ClientHandle c = p.GetClient();
  BOOST_CHECK_THROW(c->GetMessages("msg1"), e::invalid_operation);
  BOOST_CHECK_THROW(c->GetWriter("msg1"), e::invalid_operation);

  // Test invalid msgs
  BOOST_CHECK_THROW(p.AllowRead("msg0"), e::invalid_argument);
  BOOST_CHECK_THROW(p.AllowRead("msg3"), e::invalid_argument);
  BOOST_CHECK_THROW(p.AllowPost("msg0"), e::invalid_argument);
  BOOST_CHECK_THROW(p.AllowPost("msg3"), e::invalid_argument);

  // Test Proxy with read but no write
  p.AllowRead("msg1");
  c = p.GetClient();
  BOOST_CHECK_NO_THROW(c->GetMessages("msg1"));
  BOOST_CHECK_THROW(c->GetMessages("msg0"), e::invalid_operation);
  BOOST_CHECK_THROW(c->GetMessages("msg2"), e::invalid_operation);
  BOOST_CHECK_THROW(c->GetMessages("msg3"), e::invalid_operation);

  p = mb::Proxy();
  p.AllowRead("msg2");
  c = p.GetClient();
  BOOST_CHECK_NO_THROW(c->GetMessages("msg2"));
  BOOST_CHECK_THROW(c->GetMessages("msg0"), e::invalid_operation);
  BOOST_CHECK_THROW(c->GetMessages("msg1"), e::invalid_operation);
  BOOST_CHECK_THROW(c->GetMessages("msg3"), e::invalid_operation);

  // Test proxy with post but not read
  p = mb::Proxy();
  p.AllowPost("msg1");
  c = p.GetClient();
  BOOST_CHECK_NO_THROW(c->GetWriter("msg1"));
  BOOST_CHECK_NO_THROW(c->NewMessage("msg1"));
  BOOST_CHECK_THROW(c->NewMessage("msg0"), e::invalid_operation);
  BOOST_CHECK_THROW(c->GetWriter("msg0"), e::invalid_operation);
  BOOST_CHECK_THROW(c->GetWriter("msg2"), e::invalid_operation);
  BOOST_CHECK_THROW(c->GetWriter("msg3"), e::invalid_operation);

  p = mb::Proxy();
  p.AllowPost("msg2");
  c = p.GetClient();
  BOOST_CHECK_NO_THROW(c->GetWriter("msg2"));
  BOOST_CHECK_NO_THROW(c->NewMessage("msg2"));
  BOOST_CHECK_THROW(c->NewMessage("msg0"), e::invalid_operation);
  BOOST_CHECK_THROW(c->GetWriter("msg0"), e::invalid_operation);
  BOOST_CHECK_THROW(c->GetWriter("msg1"), e::invalid_operation);
  BOOST_CHECK_THROW(c->GetWriter("msg3"), e::invalid_operation);

  // Test proxy with readwrite
  p = mb::Proxy();
  p.AllowRead("msg1");
  p.AllowPost("msg2");
  c = p.GetClient();
  BOOST_CHECK_NO_THROW(c->GetMessages("msg1"));
  BOOST_CHECK_NO_THROW(c->GetWriter("msg2"));
  BOOST_CHECK_NO_THROW(c->NewMessage("msg2"));
  BOOST_CHECK_THROW(c->NewMessage("msg0"), e::invalid_operation);
  BOOST_CHECK_THROW(c->GetMessages("msg0"), e::invalid_operation);
  BOOST_CHECK_THROW(c->GetMessages("msg2"), e::invalid_operation);
  BOOST_CHECK_THROW(c->GetMessages("msg3"), e::invalid_operation);
  BOOST_CHECK_THROW(c->GetWriter("msg0"), e::invalid_operation);
  BOOST_CHECK_THROW(c->GetWriter("msg1"), e::invalid_operation);
  BOOST_CHECK_THROW(c->GetWriter("msg3"), e::invalid_operation);

  // Test conflict checks
  p = mb::Proxy();
  p.AllowRead("msg1");
  p.AllowPost("msg2");
  BOOST_CHECK_THROW(p.AllowPost("msg1"), e::invalid_operation);
  BOOST_CHECK_THROW(p.AllowRead("msg2"), e::invalid_operation);

  // TESTBUILD only routine to reset list of registered boards
  mgr.Reset();
}

BOOST_AUTO_TEST_CASE(mb_proxy_access) {
  // Set up board manager
  mb::MessageBoardManager& mgr = mb::MessageBoardManager::GetInstance();
  mgr.RegisterMessage("msg1");
  mgr.RegisterMessageVar<double>("msg1", "x");

  mb::MessageBoard::Iterator iter;
  mb::MessageBoard::Writer writer, writer2;
  mb::MessageBoard::Message message;

  // Test post
  mb::Proxy p1, p2;
  p1.AllowPost("msg1");
  p2.AllowRead("msg1");
  mb::ClientHandle c1, c2;
  // Post by retrieving a single message
  c1 = p1.GetClient();
  message = c1->NewMessage("msg1");
  message->Set<double>("x", 1.0);
  message->Post();
  // Post by retrieving a message writer
  writer = c1->GetWriter("msg1");
  message = writer->NewMessage();
  message->Set<double>("x", 2.0);
  message->Post();

  // Test read (before sync)
  c2 = p2.GetClient();
  iter = c2->GetMessages("msg1");
  BOOST_CHECK_EQUAL(iter->GetCount(), (size_t)0);

  // Test read (after sync)
  mgr.Sync("msg1");
  iter = c2->GetMessages("msg1");
  BOOST_CHECK_EQUAL(iter->GetCount(), (size_t)2);
  BOOST_CHECK_EQUAL(iter->GetMessage()->Get<double>("x"), 1.0);
  BOOST_CHECK_EQUAL(iter->Next(), true);
  BOOST_CHECK_EQUAL(iter->Get<double>("x"), 2.0);
  BOOST_CHECK_EQUAL(iter->Next(), false);

  // test writer expiry after sync
  BOOST_CHECK(!writer->IsConnected());
  message = writer->NewMessage();
  message->Set<double>("x", 2.0);
  BOOST_CHECK_THROW(message->Post(), e::invalid_operation);

  // Test writer cache refresh when board is synched
  writer = c1->GetWriter("msg1");  // new writer
  BOOST_CHECK(writer->IsConnected());

  // Test writer cache reuse
  writer2 = c1->GetWriter("msg1");
  BOOST_CHECK_EQUAL(writer.get(), writer2.get());  // compare raw ptrs

  // Write from using both writers
  message = writer->NewMessage();
  message->Set<double>("x", 3.0);
  message->Post();
  message = writer2->NewMessage();  // another writer
  message->Set<double>("x", 4.0);
  message->Post();

  mgr.Sync("msg1");
  BOOST_CHECK_EQUAL(c2->GetMessages("msg1")->GetCount(), (size_t)4);

  // TESTBUILD only routine to reset list of registered boards
  mgr.Reset();
}

BOOST_AUTO_TEST_SUITE_END()
