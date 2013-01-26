/*!
 * \file flame2/exceptions/exe.hpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Exceptions thrown by memory managers
 */
#ifndef EXCEPTIONS__EXE_HPP_
#define EXCEPTIONS__EXE_HPP_
#include <string>
#include <sstream>
#include "base.hpp"

namespace flame { namespace exceptions {

class flame_exe_exception : public flame_exception {
  public:
    explicit flame_exe_exception(const std::string& msg)
        : flame_exception(msg) {}
};

class none_available : public flame_exe_exception {
  public:
    explicit none_available(const std::string& msg)
        : flame_exe_exception(msg) {}
};

class flame_task_exception : public flame_exe_exception {
  public:
    flame_task_exception(const std::string& agent_name,
                            const std::string& task_name,
                            const std::string& msg)
        : flame_exe_exception(msg),
          agent_(agent_name), task_(task_name), msg_(msg) {}

    ~flame_task_exception() throw() {}

    const char* what() const throw() {
      std::ostringstream out;
      out << std::endl;
      out << "Runtime Error Detected" << std::endl;
      out << "======================" << std::endl;
      out << "Agent: " << agent_ << std::endl;
      out << "Function: " << task_ << std::endl;
      out << msg_ << std::endl;
      return out.str().c_str();
    }

  private:
    std::string agent_;
    std::string task_;
    std::string msg_;
};

}}  // namespace flame::exceptions
#endif  // EXCEPTIONS__EXE_HPP_
