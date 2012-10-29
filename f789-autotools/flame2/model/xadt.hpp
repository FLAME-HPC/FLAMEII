/*!
 * \file flame2/model/xadt.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XADT: holds abstract data type information
 */
#ifndef MODEL__XADT_HPP_
#define MODEL__XADT_HPP_
#include <string>
#include <vector>
#include "xvariable.hpp"

namespace flame { namespace model {

class XADT {
  public:
    XADT();
    ~XADT();
    void print();
    void setName(std::string name);
    std::string getName();
    XVariable * addVariable();
    std::vector<XVariable*> * getVariables();
    void setHoldsDynamicArray(bool b);
    bool holdsDynamicArray();

  private:
    std::string name_;
    std::vector<XVariable*> variables_;
    bool holdsDynamicArray_;
};
}}  // namespace flame::model
#endif  // MODEL__XADT_HPP_
