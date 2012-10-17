/*!
 * \file src/compat/C/message_board_wrapper.hpp
 * \author Shawn Chin
 * \date October 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Wrapper for message board to allow type-free access
 */
#ifndef COMPAT__C__MESSAGE_BOARD_WRAPPER_HPP_
#define COMPAT__C__MESSAGE_BOARD_WRAPPER_HPP_
#include <string>
#include "mb/client.hpp"
#include "message_iterator_wrapper.hpp"
#include "flame_compat_c.hpp"

namespace flame { namespace compat { namespace c {

class MessageBoardWrapper {
  public:
    explicit MessageBoardWrapper(std::string name) : name_(name) {}
    virtual ~MessageBoardWrapper() {}
    virtual void PostMessage(flame::mb::Client* mb, void* msg_ptr) = 0;
    virtual MessageIteratorWrapper* GetMessages(flame::mb::Client* mb) = 0;

  protected:
    std::string name_;
};

template <typename T>
class MessageBoardWrapperImpl : public MessageBoardWrapper {
  public:
    explicit MessageBoardWrapperImpl(std::string name)
        : MessageBoardWrapper(name) {}

    //! Posts a message on behalf of the caller
    void PostMessage(flame::mb::Client* mb, void* msg_ptr) {
      T* typed_ptr = static_cast<T*>(msg_ptr);
      flame::mb::MessageHandle msg = mb->NewMessage(name_);
      msg->Set<T>(Constants::MESSAGE_VARNAME, *typed_ptr);
      msg->Post();
    }

    //! Returns a message iterator for all messages of a specific type
    MessageIteratorWrapper* GetMessages(flame::mb::Client* mb) {
      return new MessageIteratorWrapperImpl<T>(mb->GetMessages(name_));
    }
};

}}}  // namespace flame::compat::c
#endif  // COMPAT__C__MESSAGE_BOARD_WRAPPER_HPP_
