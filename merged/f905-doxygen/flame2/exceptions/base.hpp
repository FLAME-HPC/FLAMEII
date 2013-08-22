/*!
 * \file flame2/exceptions/base.hpp
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

//! Base class for all FLAME exceptions
class flame_exception : public std::runtime_error {
  public:
    /*! Constructor
     * \param msg Excetion message
     */
    explicit flame_exception(const std::string& msg)
        : std::runtime_error(msg) {}
};

//! Logic error. Probably a bug.
class logic_error : public flame_exception {
  public:
    /*! Constructor
     * \param msg Excetion message
     */
    explicit logic_error(const std::string& msg)
        : flame_exception(msg) {}
};

//! Invalid argument specified
class invalid_argument : public flame_exception {
  public:
    /*! Constructor
     * \param msg Excetion message
     */
    explicit invalid_argument(const std::string& msg)
        : flame_exception(msg) {}
};

//! Invalid operation
class invalid_operation : public flame_exception {
  public:
    /*! Constructor
     * \param msg Excetion message
     */
    explicit invalid_operation(const std::string& msg)
        : flame_exception(msg) {}
};

//! Out-of-range error
class out_of_range : public flame_exception {
  public:
    /*! Constructor
     * \param msg Excetion message
     */
    explicit out_of_range(const std::string& msg)
        : flame_exception(msg) {}
};

//! Insufficient data provided
class insufficient_data : public flame_exception {
  public:
    /*! Constructor
     * \param msg Excetion message
     */
    explicit insufficient_data(const std::string& msg)
        : flame_exception(msg) {}
};

//! Mismatching or unexpected type
class invalid_type : public flame_exception {
  public:
    /*! Constructor
     * \param msg Excetion message
     */
    explicit invalid_type(const std::string& msg)
        : flame_exception(msg) {}
};

//! Invalid variable specified
class invalid_variable : public flame_exception {
  public:
    /*! Constructor
     * \param msg Excetion message
     */
    explicit invalid_variable(const std::string& msg)
        : flame_exception(msg) {}
};

//! Not implemented
class not_implemented : public flame_exception {
  public:
    /*! Constructor
     * \param msg Excetion message
     */
    explicit not_implemented(const std::string& msg)
        : flame_exception(msg) {}
};

//! File not found
class file_not_found : public flame_exception {
  public:
    /*! Constructor
     * \param msg Excetion message
     */
    explicit file_not_found(const std::string& msg)
        : flame_exception(msg) {}
};

}}  // namespace flame::exceptions
#endif  // EXCEPTIONS__BASE_HPP_
