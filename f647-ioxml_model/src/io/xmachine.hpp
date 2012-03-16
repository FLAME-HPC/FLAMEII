/*!
 * \file src/io/xmachine.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XMachine: holds agent information
 */
#ifndef IO__XMACHINE_HPP_
#define IO__XMACHINE_HPP_
#include <string>

namespace flame { namespace io {

class XMachine {
  public:
    XMachine() {}
    void setName(std::string name);
    std::string getName();

  private:
    std::string name_;
};
}}  // namespace flame::io
#endif  // IO__XMACHINE_HPP_
