/*!
 * \file src/exceptions/exe.hpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Exceptions thrown by memory managers
 */
#ifndef EXCEPTIONS__EXE_HPP_
#define EXCEPTIONS__EXE_HPP_
#include "base.hpp"

namespace flame { namespace exceptions {

class flame_exe_exception : public flame_exception {
  public:
    explicit flame_exe_exception(const std::string& msg)
        : flame_exception(msg) {}
};

}}  // namespace flame::exceptions
#endif  // EXCEPTIONS__EXE_HPP_
