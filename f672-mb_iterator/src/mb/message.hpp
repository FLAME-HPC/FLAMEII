/*!
 * \file src/mb/message.hpp
 * \author Shawn Chin
 * \date September 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Declaration of Message
 */
#ifndef MB__MESSAGE_HPP
#define MB__MESSAGE_HPP
#include <map>
#include <string>
#include <typeinfo>
#include "boost/any.hpp"
#include "board_writer.hpp"

namespace flame { namespace mb {

/*!
 * \brief A message instance
 *
 * Used as a proxy to read and post messages.
 *
 * An instance is only usable when linked with a BoardWriter (for posting)
 * or a MessageIterator (for reading). Therefore, it should not be manually
 * instantiated (protected constructor) and only generated from friend
 * classes.
 */
class Message {
  friend class BoardWriter;
  friend class MessageIteratorBackend;

  public:
    //! Clears all message variables that are set
    void Clear(void);

    //! Post message content to the associated BoardWriter
    void Post(void);

    /*!
     * \brief Sets the value of a message variable
     * \param[in] var_name Variable name
     * \param[in] value Value to set
     *
     * Exceptions thrown:
     * - flame::exceptions::invalid_operation Message is read-only
     * - flame::exceptions::invalid_type Variable assigned with wrong type
     * - flame::exceptions::invalid_variable Unknown variable name
     */
    template <typename T>
    void Set(std::string var_name, T value) {
#ifndef DISABLE_RUNTIME_TYPE_CHECKING
      if (!validator_->ValidateType(var_name, &typeid(T))) {
        throw flame::exceptions::invalid_type("Mismatching type");
      }
#endif
      if (readonly_) {
        throw flame::exceptions::invalid_operation("Message is read-only");
      }
      data_[var_name] = value;
    }

    /*!
     * \brief Returns the value of a message variable
     * \param[in] var_name Variable name
     * \return Value assigned to that variable
     *
     * Exceptions thrown:
     * - flame::exceptions::invalid_type Variable requested with wrong type
     * - flame::exceptions::invalid_variable Variable unknown or not set
     */
    template <typename T>
    T Get(std::string var_name) {
#ifndef DISABLE_RUNTIME_TYPE_CHECKING
      if (!validator_->ValidateType(var_name, &typeid(T))) {
        throw flame::exceptions::invalid_type("Mismatching type");
      }
#endif
      return boost::any_cast<T>(data_[var_name]);
    }

  protected:
    /*!
     * \brief Map type used to store message data
     *
     * boost::any is used as value type so we can store heterogenous data
     * within the same map. The actual datatype is check in the Set()
     * method before it is entered into the map.
     */
    typedef std::map<std::string, boost::any> DataMap;

    /*!
     * \brief Map of data stored in this message
     *
     * Defined as protected so BoardWriter can have direct access to data
     */
    DataMap data_;

    /*!
     * \brief Constructor
     * \param[in] tv Pointer to TypeValidator instance
     *
     * The TypeValidator pointer is stored as validator_ and will be used
     * to quickly validate input data.
     *
     * readonly_ is initialised to true.
     */
    explicit Message(TypeValidator* tv)
      : readonly_(true), validator_(tv) {}

    //! Assigns callback function and sets readonly_ to false
    void AssignPostCallback(MessagePostCallback func);

    //! Protected version which assign values using boost::any
    void Set(std::string var_name, boost::any value);

  private:
    bool readonly_;  //! Flag indicating read-only status
    MessagePostCallback callback_;  //! Callback function to perform Post()
    TypeValidator* validator_;  //! Obj pointer used to validate input data
};

}}  // namespace flame::mb
#endif  // MB__MESSAGE_HPP

