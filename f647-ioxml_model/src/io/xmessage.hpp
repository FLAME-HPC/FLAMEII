/*!
 * \file src/io/xmessage.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XMessage: holds message information
 */
#ifndef IO__XMESSAGE_HPP_
#define IO__XMESSAGE_HPP_
#include <string>
#include <vector>
#include "./xvariable.hpp"

namespace flame { namespace io {

class XMessage {
  public:
    XMessage() {}
    ~XMessage();
    void print();
    void setName(std::string name);
    std::string getName();
    XVariable * addVariable();
    std::vector<XVariable*> * getVariables();

  private:
    std::string name_;
    std::vector<XVariable*> variables_;
};
}}  // namespace flame::io
#endif  // IO__XMESSAGE_HPP_
