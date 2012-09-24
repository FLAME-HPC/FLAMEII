#ifndef MB__MESSAGE_HPP
#define MB__MESSAGE_HPP
#include <string>

namespace flame { namespace mb {


class Message {
  friend class BoardWriter;

  public:

  protected:
    explicit Message(const std::string message_name)
      : msg_name_(message_name) {}

  private:
    std::string msg_name_;
};

}}  // namespace flame::mb
#endif // MB__MESSAGE_HPP
