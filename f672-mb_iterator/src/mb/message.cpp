/*!
 * \file src/mb/message.cpp
 * \author Shawn Chin
 * \date September 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Implementation of class Message
 */
#include <string>
#include "exceptions/all.hpp"
#include "message.hpp"

namespace flame { namespace mb {

/*!
 * \brief Assigns callback function used to perform Post()
 * \param[in] func Callback function
 *
 * This also sets readonly_ to flase which enables Set().
 */
void Message::AssignPostCallback(MessagePostCallback func) {
  callback_ = func;
  readonly_ = false;
}

/*!
 * \brief Posts this message to parent BoardWriter
 *
 * Message instances are reusable, so repeated calls to Post() will
 * submit duplicate data to the board. Users are free to change values
 * before reposting using Set().
 *
 * However, users are encouraged to use Clear() to remove all set values
 * before reposting to avoid unintentionally reusing values that have not
 * been reset.
 *
 * Exceptions thrown:
 * - flame::exceptions::invalid_operation Message is read-only or the
 *    callback function is \c null
 * - any exceptions thrown by the callback function. Possibly
 *   flame::exceptions::insufficient_data (not all variables set).
 */
void Message::Post(void) {
  if (readonly_ || !callback_) {
    throw flame::exceptions::invalid_operation("This message is read-only");
  }
  callback_(this);
}

/*!
 * \brief Clear all message values
 *
 * All values that have been set using Set() will be removed and users will
 * have to set new values before reposting.
 *
 * Raises flame::exceptions::invalid_operation if message is read-only.
 */
void Message::Clear(void) {
  if (readonly_) {
    throw flame::exceptions::invalid_operation("This message is read-only");
  }
  data_.clear();
}

/*!
 * \brief Internal version of Set()
 *
 * This is a protected version of Set() to be used only by friend classes.
 * It allows variables to be assigned using a type-independent function
 * signature. Values are passed in using boost::any.
 *
 * This method can be used to set and update read-only messages.
 * 
 * When DISABLE_RUNTIME_TYPE_CHECKING is defined, the boost::any
 * container is inspected to ensure that the underlying data is of the
 * correct type. Otherwise, the value is store as-is and it is the caller's
 * responsibility to ensure that the datatype matches.
 */
void Message::Set(std::string var_name, boost::any value) {
#ifndef DISABLE_RUNTIME_TYPE_CHECKING
  if (validator_->ValidateType(var_name, &value.type())) {
    throw flame::exceptions::invalid_type("Mismatching type");
  }
#endif
  data_[var_name] = value;
}

}}  // namespace::mb
