/*!
 * \file tests/api/test_flame_api.cpp
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
#include "flame2/mb/message_board_manager.hpp"
#include "flame2/mb/proxy.hpp"
#include "flame2/mem/memory_manager.hpp"
#include "flame2/api/agent_api.hpp"

BOOST_AUTO_TEST_SUITE(APIModule)

namespace mb = flame::mb;
namespace mem = flame::mem;
namespace e = flame::exceptions;

BOOST_AUTO_TEST_CASE(api_test_basic) {

  // Create and populate agent memory
  mem::MemoryManager& mem_mgr = mem::MemoryManager::GetInstance();
  mem_mgr.RegisterAgent("Circle");
  mem_mgr.RegisterAgentVar<int>("Circle", "ro");
  mem_mgr.RegisterAgentVar<int>("Circle", "rw");
  mem_mgr.RegisterAgentVar<int>("Circle", "no_access");
  mem_mgr.HintPopulationSize("Circle", (size_t)10);
  std::vector<int>* ro_ptr = mem_mgr.GetVector<int>("Circle", "ro");
  std::vector<int>* rw_ptr = mem_mgr.GetVector<int>("Circle", "rw");
  std::vector<int>* na_ptr = mem_mgr.GetVector<int>("Circle", "no_access");
  for (int i = 0; i < 10; ++i) {
    ro_ptr->push_back(i);
    rw_ptr->push_back(i + 10);
    na_ptr->push_back(i + 100);
  }
  
  // Create message board
  mb::MessageBoardManager& mb_mgr = mb::MessageBoardManager::GetInstance();
  mb_mgr.RegisterMessage<int>("msg_read");
  mb_mgr.RegisterMessage<int>("msg_write");
  mb_mgr.RegisterMessage<int>("msg_no_access");
  
  
  // Prepare access to memory
  mem::AgentShadowPtr shadow = mem_mgr.GetAgentShadow("Circle");
  shadow->AllowAccess("ro");  // read-only
  shadow->AllowAccess("rw", true);  // read-write
  // No access given to var "no_access"
  
  // Prepare access to message board
  mb::Proxy proxy;
  proxy.AllowRead("msg_read");
  proxy.AllowPost("msg_write");

  // Create API object
  mem::MemoryIteratorPtr mem_iter = shadow->GetMemoryIterator();
  flame::api::AgentAPI FLAME(mem_iter, proxy.GetClient());

  // test read access
  BOOST_CHECK_EQUAL(FLAME.GetMem<int>("ro"), 0);
  BOOST_CHECK_EQUAL(FLAME.GetMem<int>("rw"), 10);
  BOOST_CHECK_THROW(FLAME.GetMem<int>("no_access"), e::flame_api_access_denied);
  BOOST_CHECK_THROW(FLAME.GetMem<int>("unknown"), e::flame_api_unknown_param);
  BOOST_CHECK_THROW(FLAME.GetMem<double>("rw"), e::flame_api_invalid_type);

  // test write access
  BOOST_CHECK_NO_THROW(FLAME.SetMem<int>("rw", 10));
  BOOST_CHECK_THROW(FLAME.SetMem<int>("ro", 10), e::flame_api_access_denied);
  BOOST_CHECK_THROW(FLAME.SetMem<int>("no_access", 10),
                      e::flame_api_access_denied);
  BOOST_CHECK_THROW(FLAME.SetMem<int>("unknown", 10),
                      e::flame_api_unknown_param);
  BOOST_CHECK_THROW(FLAME.SetMem<double>("rw", 10.0),
                      e::flame_api_invalid_type);

  // test post message
  BOOST_CHECK_NO_THROW(FLAME.PostMessage<int>("msg_write", 99));
  BOOST_CHECK_THROW(FLAME.PostMessage<double>("msg_write", 0.00),
                      e::flame_api_invalid_type);
  BOOST_CHECK_THROW(FLAME.PostMessage<int>("msg_read", 1.00),
                      e::flame_api_access_denied);
  BOOST_CHECK_THROW(FLAME.PostMessage<int>("unknown", 1.00),
                      e::flame_api_unknown_param);

  // test get messages
  BOOST_CHECK_NO_THROW(FLAME.GetMessageIterator("msg_read"));
  BOOST_CHECK_THROW(FLAME.GetMessageIterator("msg_write"),
                      e::flame_api_access_denied);
  BOOST_CHECK_THROW(FLAME.GetMessageIterator("unknown"),
                      e::flame_api_unknown_param);


  // test iterating through agents
  for(int i = 0; i < 10; ++i) {
    BOOST_CHECK_EQUAL(FLAME.GetMem<int>("ro"), i);
    BOOST_CHECK_EQUAL(FLAME.GetMem<int>("rw"), i + 10);
    BOOST_CHECK(!mem_iter->AtEnd());
    mem_iter->Step();
  }
  BOOST_CHECK(mem_iter->AtEnd());
  
  // reset managers so as not to affect next test suite
  mem_mgr.Reset();
  mb_mgr.Reset(); 
}

BOOST_AUTO_TEST_SUITE_END()
