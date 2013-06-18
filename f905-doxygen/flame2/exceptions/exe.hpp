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

//! Base class for exceptions specific to EXE module
class flame_exe_exception : public flame_exception {
  public:
    /*! Constructor
     * \param msg Excetion message
     */
    explicit flame_exe_exception(const std::string& msg)
        : flame_exception(msg) {}
};

//! None available
class none_available : public flame_exe_exception {
  public:
    /*! Constructor
     * \param msg Excetion message
     */
    explicit none_available(const std::string& msg)
        : flame_exe_exception(msg) {}
};

//! Runtime exception during task execution
class flame_task_exception : public flame_exe_exception {
  public:
    /*! Constructor
     * \param agent_name Associated agent name
     * \param task_name Task where exection occured
     * \param msg Excetion message
     */
    flame_task_exception(const std::string& agent_name,
                            const std::string& task_name,
                            const std::string& msg)
        : flame_exe_exception(msg),
          agent_(agent_name), task_(task_name), msg_(msg) {}

    ~flame_task_exception() throw() {}

    //! Prints details and location of exception
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
    std::string agent_;  //! agent name
    std::string task_;  //! task name
    std::string msg_;  //! exception message
};

}}  // namespace flame::exceptions
#endif  // EXCEPTIONS__EXE_HPP_
