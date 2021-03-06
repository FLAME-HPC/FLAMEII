/*!
 * \file flame2/exceptions/runtime.hpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Parent class for all exceptions thrown by FLAME
 */
#ifndef EXCEPTIONS__BASE_HPP_
#define EXCEPTIONS__BASE_HPP_
#include <stdexcept>
#include <string>

namespace flame { namespace exceptions {

class flame_exception : public std::runtime_error {
  public:
    explicit flame_exception(const std::string& msg)
        : std::runtime_error(msg) {}
};

class logic_error : public flame_exception {
  public:
    explicit logic_error(const std::string& msg)
        : flame_exception(msg) {}
};

class invalid_argument : public flame_exception {
  public:
    explicit invalid_argument(const std::string& msg)
        : flame_exception(msg) {}
};

class invalid_operation : public flame_exception {
  public:
    explicit invalid_operation(const std::string& msg)
        : flame_exception(msg) {}
};

class out_of_range : public flame_exception {
  public:
    explicit out_of_range(const std::string& msg)
        : flame_exception(msg) {}
};

class insufficient_data : public flame_exception {
  public:
    explicit insufficient_data(const std::string& msg)
        : flame_exception(msg) {}
};

class invalid_type : public flame_exception {
  public:
    explicit invalid_type(const std::string& msg)
        : flame_exception(msg) {}
};

class invalid_variable : public flame_exception {
  public:
    explicit invalid_variable(const std::string& msg)
        : flame_exception(msg) {}
};

class not_implemented : public flame_exception {
  public:
    explicit not_implemented(const std::string& msg)
        : flame_exception(msg) {}
};

class file_not_found : public flame_exception {
  public:
    explicit file_not_found(const std::string& msg)
        : flame_exception(msg) {}
};

}}  // namespace flame::exceptions
#endif  // EXCEPTIONS__BASE_HPP_
