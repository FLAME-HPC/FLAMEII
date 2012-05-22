/*!
 * \file src/model/xvariable.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XVariable: holds agent variable information
 */
#ifndef MODEL__XVARIABLE_HPP_
#define MODEL__XVARIABLE_HPP_
#include <string>

namespace flame { namespace model {

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
    void setStaticArraySize(int size);
    int getStaticArraySize();
    void setHasADTType(bool b);
    bool hasADTType();
    void setHoldsDynamicArray(bool b);
    bool holdsDynamicArray();
    void setConstantString(std::string name);
    std::string getConstantString();
    void setConstantSet(bool set);
    bool isConstantSet();
    void setConstant(bool constant);
    bool isConstant();

  private:
    std::string type_;
    std::string name_;
    bool isDynamicArray_;
    bool isStaticArray_;
    int staticArraySize_;
    bool hasADTType_;
    bool holdsDynamicArray_;
    std::string constantString_;
    bool constantSet_;
    bool constant_;
};
}}  // namespace flame::model
#endif  // MODEL__XVARIABLE_HPP_
