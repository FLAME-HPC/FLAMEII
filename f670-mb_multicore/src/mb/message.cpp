#include "exceptions/all.hpp"
#include "message.hpp"

namespace flame { namespace mb {

void Message::AssignPostCallback(MessagePostCallback func) {
  callback_ = func;
  readonly_ = false;
}

void Message::Post(void) {
  if (readonly_ || !callback_) {
    throw flame::exceptions::invalid_operation("This message is read-only");
  }
  callback_(this);
}

void Message::Set(std::string var_name, boost::any value) {
#ifndef DISABLE_RUNTIME_TYPE_CHECKING
  if (validator_->ValidateType(var_name, &value.type())) {
    throw flame::exceptions::invalid_type("Mismatching type");
  }
#endif
  data_[var_name] = value;
}

}}  // namespace::mb
