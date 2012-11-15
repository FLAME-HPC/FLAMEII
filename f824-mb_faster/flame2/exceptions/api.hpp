/*!
 * \file flame2/exceptions/api.hpp
 * \author Shawn Chin
 * \date November 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Exceptions thrown by User API calls
 */
#ifndef EXCEPTIONS__API_HPP_
#define EXCEPTIONS__API_HPP_
#include <string>
#include <sstream>
#include "base.hpp"

namespace flame { namespace exceptions {

class flame_api_exception : public flame_exception {
  public:
    flame_api_exception(const std::string& func, const std::string& msg)
        : flame_exception(msg), f_(func), m_(msg) {}
        
    ~flame_api_exception () throw() {}
    
    const char* what() const throw () {
      std::ostringstream out;
      out << "Usage Error: [" << f_ << "] " << m_ << std::endl;
      return out.str().c_str();
    }

  private:
    std::string f_;
    std::string m_;

};
  
}}  // namespace flame::exceptions
#endif  // EXCEPTIONS__API_HPP_
