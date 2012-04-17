/*!
 * \file src/model/xmessage.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XMessage: holds message information
 */
#ifndef MODEL__XMESSAGE_HPP_
#define MODEL__XMESSAGE_HPP_
#include <string>
#include <vector>
#include "./xvariable.hpp"

namespace flame { namespace model {

class XMessage {
  public:
    XMessage() {}
    ~XMessage();
    void print();
    void setName(std::string name);
    std::string getName();
    XVariable * addVariable();
    std::vector<XVariable*> * getVariables();
    bool validateVariableName(std::string name);

  private:
    std::string name_;
    std::vector<XVariable*> variables_;
};
}}  // namespace flame::model
#endif  // MODEL__XMESSAGE_HPP_
