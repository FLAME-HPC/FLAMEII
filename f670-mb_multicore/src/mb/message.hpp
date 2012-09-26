#ifndef MB__MESSAGE_HPP
#define MB__MESSAGE_HPP
#include <string>
#include <typeinfo>
#include "boost/any.hpp"
#include "boost/shared_ptr.hpp"
#include "board_writer.hpp"

// TODO: In debug mode, check that all message values have been set before
// posing. Flags should be reset upon Post() so Message can be reused.
namespace flame { namespace mb {

typedef boost::shared_ptr<Message> MessageHandle;

class TypeValidator;  // forward declaration

class Message {
  friend class BoardWriter;
  // friend class MessageIterator;

  public:
    typedef std::map<std::string, boost::any> DataMap;

    void Clear(void);

    void Post(void);

    template <typename T>
    void Set(std::string var_name, T value) {
#ifndef DISABLE_RUNTIME_TYPE_CHECKING
      if (!validator_->ValidateType(var_name, &typeid(T))) {
        throw flame::exceptions::invalid_type("Mismatching type");
      }
#endif
      data_[var_name] = value;
    }

    template <typename T>
    void Get(std::string var_name) {
#ifndef DISABLE_RUNTIME_TYPE_CHECKING
      if (!validator_->ValidateType(var_name, &typeid(T))) {
        throw flame::exceptions::invalid_type("Mismatching type");
      }
#endif
      return boost::any_cast<T>(data_[var_name]);
    }

  protected:
    // Limit constructor to BoardWrite and MessageIterators
    explicit Message(TypeValidator* tv)
      : readonly_(true), validator_(tv) {}

    // Assigns callback function and set readonly_ = false
    void AssignPostCallback(MessagePostCallback func);

    // Allow BoardWrite and MessageIterators to assign values using boost::any
    void Set(std::string var_name, boost::any value);

    // BoardWriter should have access to this map
    DataMap data_;

  private:
    bool readonly_;
    MessagePostCallback callback_;
    TypeValidator* validator_;

};

}}  // namespace flame::mb
#endif // MB__MESSAGE_HPP
