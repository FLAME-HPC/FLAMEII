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
#include <boost/ptr_container/ptr_vector.hpp>
#include <string>
#include <vector>
#include "xvariable.hpp"

namespace flame { namespace model {

class XADT {
  public:
    XADT();
    void print();
    void setName(std::string name);
    std::string getName();
    XVariable * addVariable();
    boost::ptr_vector<XVariable> * getVariables();
    void setHoldsDynamicArray(bool b);
    bool holdsDynamicArray();

  private:
    std::string name_;
    boost::ptr_vector<XVariable> variables_;
    bool holdsDynamicArray_;
};
}}  // namespace flame::model
#endif  // MODEL__XADT_HPP_
