/*!
 * \file src/io/xvariable.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XVariable: holds agent variable information
 */
#ifndef IO__XVARIABLE_HPP_
#define IO__XVARIABLE_HPP_
#include <string>

namespace flame { namespace io {

class XVariable {
  public:
    XVariable();
    void print();
    void setName(std::string name);
    std::string getName();
    void setType(std::string type);
    std::string getType();
    void setIsDynamicArray(bool b);
    bool isDynamicArray();
    void setIsStaticArray(bool b);
    bool isStaticArray();

  private:
    std::string type_;
    std::string name_;
    bool isDynamicArray_;
    bool isStaticArray_;
    int staticArraySize;
};
}}  // namespace flame::io
#endif  // IO__XVARIABLE_HPP_
