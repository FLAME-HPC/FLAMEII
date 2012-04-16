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
#include <vector>
#include "./xvariable.hpp"
#include "./xfunction.hpp"

namespace flame { namespace io {

class XMachine {
  public:
    XMachine() {}
    ~XMachine();
    void print();
    void setName(std::string name);
    const std::string getName();
    XVariable * addVariable();
    std::vector<XVariable*> * getVariables();
    XVariable * getVariable(std::string name);
    XFunction * addFunction();
    std::vector<XFunction*> * getFunctions();
    bool validateVariableName(std::string name);

  private:
    std::string name_;
    std::vector<XVariable*> variables_;
    std::vector<XFunction*> functions_;
};
}}  // namespace flame::io
#endif  // IO__XMACHINE_HPP_
