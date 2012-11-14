/*!
 * \file flame2/api/cxx/agent_api.hpp
 * \author Shawn Chin
 * \date November 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Declaration of AgentAPI 
 */
// TODO(lsc): trap exceptions and print sensible error messages

#ifndef FLAME2__API__CXX__AGENT_API_HPP_
#define FLAME2__API__CXX__AGENT_API_HPP_
#include <string>
#include <boost/shared_ptr.hpp>
#include "flame2/mb/message_iterator.hpp"
#include "flame2/mem/memory_iterator.hpp"
#include "flame2/mb/client.hpp"
namespace flame { namespace api {

typedef boost::shared_ptr<flame::mb::MessageIterator>  MessageIterator;
typedef boost::shared_ptr<flame::mb::Client> MBClient;
typedef boost::shared_ptr<flame::mem::MemoryIterator> MemIterPtr;
/*!
 * \brief Proxy object through which agent functions make API calls
 *
 * An instance of this class is passed as an argument to all agent transition
 * functions
 */
class AgentAPI {
  public:
    AgentAPI(MemIterPtr mem, MBClient mb) : mem_(mem), mb_(mb) {}

    template <typename T>
    inline T MemoryGet(const std::string& var_name) {
      return mem_->Get<T>(var_name);
    }

    template <typename T>
    inline void MemorySet(const std::string& var_name, T value) {
      mem_->Set<T>(var_name, value);
    }

    template <typename T>
    inline void PostMessage(const std::string& msg_name, T msg) {
      mb_->GetBoardWriter(msg_name)->Post<T>(msg);
    }

    inline MessageIterator GetMessages(const std::string& msg_name) {
      return mb_->GetMessages(msg_name);
    }
    
  private:
    MemIterPtr mem_;
    MBClient mb_;
};


}}  // namespace flame2::api

#endif  // FLAME2__API__CXX__AGENT_API_HPP_
