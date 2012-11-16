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
// TODO(lsc): Cache message iterators (?)

#ifndef FLAME2__API__AGENT_API_HPP_
#define FLAME2__API__AGENT_API_HPP_
#include <string>
#include <boost/shared_ptr.hpp>
#include "flame2/mem/memory_iterator.hpp"
#include "flame2/mb/client.hpp"
#include "flame2/exceptions/api.hpp"
#include "flame2/exceptions/all.hpp"
#include "message_iterator_wrapper.hpp"

namespace flame { namespace api {
  
class AgentAPI;  // Forward declaration (defined below)
typedef AgentAPI& AgentFuncParamType;
typedef int AgentFuncRetType;

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
    inline T GetMem(const std::string& var_name) {
      try {
        return mem_->Get<T>(var_name);
      } catch(const flame::exceptions::invalid_type& E) {
        throw flame::exceptions::flame_api_invalid_type(
          "GetMem",
          "Invalid type specified. Check that the type used when calling "
          "'.GetMem<DATATYPE>()' matches the type of the agent memory variable."
        );
      } catch(const flame::exceptions::invalid_variable& E) {
        throw flame::exceptions::flame_api_unknown_param(
          "GetMem",
          std::string("Unknown memory variable. Agent does not have memory "
          "variable with name '") + var_name + "'."
        );
      } catch(const flame::exceptions::invalid_operation& E) {
        throw flame::exceptions::flame_api_access_denied(
          "GetMem",
          std::string("No access. This function has not been given read "
          "access to memory variable '") + var_name + "'."
        );
      }
    }

    template <typename T>
    inline void SetMem(const std::string& var_name, T value) {
      try {
        mem_->Set<T>(var_name, value);
      } catch(const flame::exceptions::invalid_type& E) {
        throw flame::exceptions::flame_api_invalid_type(
          "SetMem",
          "Invalid type specified. Check that the type used when calling "
          "'.SetMem<DATATYPE>()' matches the type of the agent memory variable."
        );
      } catch(const flame::exceptions::invalid_variable& E) {
        throw flame::exceptions::flame_api_unknown_param(
          "SetMem",
          std::string("Unknown memory variable. Agent does not have memory "
          "variable with name '") + var_name + "'."
        );
      } catch(const flame::exceptions::invalid_operation& E) {
        throw flame::exceptions::flame_api_access_denied(
          "SetMem",
          std::string("No access. This function has not been given write "
          "access to memory variable '") + var_name + "'."
        );
      }
    }

    template <typename T>
    inline void PostMessage(const std::string& msg_name, T msg) {
      try {
        mb_->GetBoardWriter(msg_name)->Post<T>(msg);
      } catch(const flame::exceptions::invalid_type& E) {
        throw flame::exceptions::flame_api_invalid_type(
          "PostMessage",
          "Invalid type specified. Check that the type used when calling "
          "'.PostMessage<DATATYPE>()' matches the message type."
        );
      } catch(const flame::exceptions::invalid_operation& E) {
        throw flame::exceptions::flame_api_access_denied(
          "PostMessage",
          std::string("No access. This function has not been given write ")
          + "access to message named '" + msg_name + "'."
        );
      } catch(const flame::exceptions::invalid_argument& E) {
        throw flame::exceptions::flame_api_unknown_param(
          "PostMessage",
          std::string("Unknown message. The is no registered message with the ")
          + "name '" + msg_name + "'. Do check your that the name you "
          "specified matches the message name provided in the model definition."
        );
      }
    }

    inline MessageIteratorWrapper GetMessageIterator(
          const std::string& msg_name) {
      try {
        return MessageIteratorWrapper(mb_->GetMessages(msg_name));
      } catch(const flame::exceptions::invalid_operation& E) {
        throw flame::exceptions::flame_api_access_denied(
          "GetMessageIterator",
          std::string("No access. This function has not been given read ")
          + "access to message named '" + msg_name + "'."
        );
      } catch(const flame::exceptions::invalid_argument& E) {
        throw flame::exceptions::flame_api_unknown_param(
          "GetMessageIterator",
          std::string("Unknown message. The is no registered message with the ")
          + "name '" + msg_name + "'. Do check your that the name you "
          "specified matches the message name provided in the model definition."
        );
      }
    }
    
  private:
    MemIterPtr mem_;
    MBClient mb_;
};


}}  // namespace flame2::api

#endif  // FLAME2__API__AGENT_API_HPP_
