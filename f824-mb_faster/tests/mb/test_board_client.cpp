/*!
 * \file tests/mb/test_board_client.cpp
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
#include "flame2/mb/proxy.hpp"

BOOST_AUTO_TEST_SUITE(MBModule)

namespace e = flame::exceptions;
namespace mb = flame::mb;

BOOST_AUTO_TEST_CASE(mb2_proxy_acl) {
  // Set up board manager
  mb::MessageBoardManager& mgr = mb::MessageBoardManager::GetInstance();
  mgr.RegisterMessage<int>("m_int");
  mgr.RegisterMessage<double>("m_double");

  // Test proxy with no read/write access
  mb::Proxy p;
  mb::Proxy::client c = p.GetClient();
  BOOST_CHECK_THROW(c->GetMessages("m_int"), e::invalid_operation);
  BOOST_CHECK_THROW(c->GetBoardWriter("m_int"), e::invalid_operation);

  // Test invalid messages
  BOOST_CHECK_THROW(p.AllowRead("a_msg"), e::invalid_argument);
  BOOST_CHECK_THROW(p.AllowRead("z_msg"), e::invalid_argument);
  BOOST_CHECK_THROW(p.AllowPost("a_msg"), e::invalid_argument);
  BOOST_CHECK_THROW(p.AllowPost("z_msg"), e::invalid_argument);

  // Test read perms
  p.AllowRead("m_int");
  c = p.GetClient();
  BOOST_CHECK_NO_THROW(c->GetMessages("m_int"));
  BOOST_CHECK_THROW(c->GetMessages("m_double"), e::invalid_operation);

  // Test a different proxy to make sure they are independent
  p = mb::Proxy();
  p.AllowRead("m_double");
  c = p.GetClient();
  BOOST_CHECK_NO_THROW(c->GetMessages("m_double"));
  BOOST_CHECK_THROW(c->GetMessages("m_int"), e::invalid_operation);

  // Test post perms
  p = mb::Proxy();
  p.AllowPost("m_int");
  c = p.GetClient();
  BOOST_CHECK_NO_THROW(c->GetBoardWriter("m_int"));
  BOOST_CHECK_THROW(c->GetBoardWriter("m_double"), e::invalid_operation);

  // Test proxy with read and write (to diff messages)
  p = mb::Proxy();
  p.AllowPost("m_int");
  p.AllowRead("m_double");
  c = p.GetClient();
  BOOST_CHECK_NO_THROW(c->GetBoardWriter("m_int"));
  BOOST_CHECK_NO_THROW(c->GetMessages("m_double"));
  BOOST_CHECK_THROW(c->GetMessages("m_int"), e::invalid_operation);
  BOOST_CHECK_THROW(c->GetBoardWriter("m_double"), e::invalid_operation);

  // Test conflict check
  BOOST_CHECK_THROW(p.AllowPost("m_double"), e::invalid_operation);
  BOOST_CHECK_THROW(p.AllowRead("m_int"), e::invalid_operation);

  // Test with unknown msg
  BOOST_CHECK_THROW(c->GetMessages("unknown"), e::invalid_argument);
  BOOST_CHECK_THROW(c->GetBoardWriter("unknown"), e::invalid_argument);
  
  // TESTBUILD only routine to reset list of registered boards
  mgr.Reset();
}

BOOST_AUTO_TEST_SUITE_END()
