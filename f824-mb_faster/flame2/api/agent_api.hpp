/*!
 * \file flame2/api/cxx/agent_api.hpp
 * \author Shawn Chin
 * \date November 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Declaration of AgentAPI 
 */
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

class AgentAPI;  // Forward declaration (class defined below)

//! Datatype for argument of all agent transition functions
typedef AgentAPI& FLAME_AgentFunctionParamType;
//! Datatype for return value for all agent transition functions
typedef int FLAME_AgentFunctionReturnType;

//! Shared pointer to message board client
typedef boost::shared_ptr<flame::mb::Client> MBClient;
//! Shared pointer to agent memory iterator
typedef boost::shared_ptr<flame::mem::MemoryIterator> MemIterPtr;

/*!
 * \brief Proxy object through which agent functions make API calls
 *
 * An instance of this class is passed as an argument to all agent transition
 * functions. It should be used to make all FLAME API calls. This allows us
 * to map each call to the corrent agent memory and access priviledges without
 * resorting to nasty global values.
 *
 * All methods are inlined to reduce the impact of the additional level of
 * indirection.
 *
 * All erroneous calls should result in an subclass of
 * flame::exceptions::flame_api_exception being thrown with an appropriate
 * error message. Any other exception thrown as a result of a method call
 * woulb be considered a framework bug.
 */
class AgentAPI {
  public:
    /*!
     * \brief Constructor
     * \param mem Shared pointer to agent memory iterator
     * \param mb Shared pointer to message board client
     *
     * We expect \c mem to be derived from
     * flame::mem::AgentShadow::GetMemoryIterator() and mb from the
     * flame::exe::TaskInterface::GetMessageBoardClient(). These objects should
     * thus contain access control information to manage access from a
     * specific agent function to memory and message boards.
     */
    AgentAPI(MemIterPtr mem, MBClient mb) : mem_(mem), mb_(mb) {}

    /*!
     * \brief Returns an agent memory value
     * \param var_name Name of memory variable to retrieve
     *
     * Possible exceptions:
     *  - flame::exceptions::flame_api_invalid_type (Invalid type specified)
     *  - flame::exceptions::flame_api_unknown_param (Unknown variable name)
     *  - flame::exceptions::flame_api_access_denied (No read access to var)
     */
    template <typename T>
    inline T GetMem(const std::string& var_name) {
      try {
        return mem_->Get<T>(var_name);
      } catch(const flame::exceptions::invalid_type& E) {
        throw flame::exceptions::flame_api_invalid_type(
          "GetMem",
          "Invalid type specified. Check that the type used when calling "
          "'.GetMem<DATATYPE>()' matches the type of the agent memory "
          "variable.");
      } catch(const flame::exceptions::invalid_variable& E) {
        throw flame::exceptions::flame_api_unknown_param(
          "GetMem",
          std::string("Unknown memory variable. Agent does not have memory "
          "variable with name '") + var_name + "'.");
      } catch(const flame::exceptions::invalid_operation& E) {
        throw flame::exceptions::flame_api_access_denied(
          "GetMem",
          std::string("No access. This function has not been given read "
          "access to memory variable '") + var_name + "'.");
      }
    }

    /*!
     * \brief Sets an agent memory value
     * \param var_name Name of memory variable to set
     * \param value Value to set it to
     *
     * Possible exceptions:
     *  - flame::exceptions::flame_api_invalid_type (Invalid type specified)
     *  - flame::exceptions::flame_api_unknown_param (Unknown variable name)
     *  - flame::exceptions::flame_api_access_denied (No write access to var)
     */
    template <typename T>
    inline void SetMem(const std::string& var_name, T value) {
      try {
        mem_->Set<T>(var_name, value);
      } catch(const flame::exceptions::invalid_type& E) {
        throw flame::exceptions::flame_api_invalid_type(
          "SetMem",
          "Invalid type specified. Check that the type used when calling "
          "'.SetMem<DATATYPE>()' matches the type of the agent memory "
          "variable.");
      } catch(const flame::exceptions::invalid_variable& E) {
        throw flame::exceptions::flame_api_unknown_param(
          "SetMem",
          std::string("Unknown memory variable. Agent does not have memory "
          "variable with name '") + var_name + "'.");
      } catch(const flame::exceptions::invalid_operation& E) {
        throw flame::exceptions::flame_api_access_denied(
          "SetMem",
          std::string("No access. This function has not been given write "
          "access to memory variable '") + var_name + "'.");
      }
    }

    /*!
     * \brief Post a message
     * \param msg_name Name of message to post
     * \param msg Value of the message
     *
     * Possible exceptions:
     *  - flame::exceptions::flame_api_invalid_type (Mismatcing type specified)
     *  - flame::exceptions::flame_api_unknown_param (Unknown message name)
     *  - flame::exceptions::flame_api_access_denied (No write access to board)
     */
    template <typename T>
    inline void PostMessage(const std::string& msg_name, T msg) {
      try {
        mb_->GetBoardWriter(msg_name)->Post<T>(msg);
      } catch(const flame::exceptions::invalid_type& E) {
        throw flame::exceptions::flame_api_invalid_type(
          "PostMessage",
          "Invalid type specified. Check that the type used when calling "
          "'.PostMessage<DATATYPE>()' matches the message type.");
      } catch(const flame::exceptions::invalid_operation& E) {
        throw flame::exceptions::flame_api_access_denied(
          "PostMessage",
          std::string("No access. This function has not been given write ")
          + "access to message named '" + msg_name + "'.");
      } catch(const flame::exceptions::invalid_argument& E) {
        throw flame::exceptions::flame_api_unknown_param(
          "PostMessage",
          std::string("Unknown message. The is no registered message with the ")
          + "name '" + msg_name + "'. Do check your that the name you "
          "specified matches the message name provided in the model "
          "definition.");
      }
    }

    /*!
     * \brief Returns a message iterator
     * \param msg_name Name of message to post
     *
     * Possible exceptions:
     *  - flame::exceptions::flame_api_unknown_param (Unknown message name)
     *  - flame::exceptions::flame_api_access_denied (No read access to board)
     */
    inline MessageIteratorWrapper GetMessageIterator(
          const std::string& msg_name) {
      try {
        return MessageIteratorWrapper(mb_->GetMessages(msg_name));
      } catch(const flame::exceptions::invalid_operation& E) {
        throw flame::exceptions::flame_api_access_denied(
          "GetMessageIterator",
          std::string("No access. This function has not been given read ")
          + "access to message named '" + msg_name + "'.");
      } catch(const flame::exceptions::invalid_argument& E) {
        throw flame::exceptions::flame_api_unknown_param(
          "GetMessageIterator",
          std::string("Unknown message. The is no registered message with the ")
          + "name '" + msg_name + "'. Do check your that the name you "
          "specified matches the message name provided in the model "
          "definition.");
      }
    }

  private:
    MemIterPtr mem_;  //! Store shared pointer to agent memory iterator
    MBClient mb_;  //! Store shared pointer to message board client
};
}}  // namespace flame2::api

#endif  // FLAME2__API__AGENT_API_HPP_
