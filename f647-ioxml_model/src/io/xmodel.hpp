/*!
 * \file src/io/xmodel.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XModel: holds model information
 */
#ifndef IO__XMODEL_HPP_
#define IO__XMODEL_HPP_
#include <string>
#include <vector>
#include "./xmachine.hpp"
#include "./xvariable.hpp"
#include "./xadt.hpp"
#include "./xtimeunit.hpp"
#include "./xmessage.hpp"

namespace flame { namespace io {

class XModel {
  public:
    XModel() {}
    ~XModel();
    int clear();
    void print();
    int check();
    void setName(std::string name) { name_ = name; }
    std::string getName() { return name_; }
    XVariable * addConstant();
    std::vector<XVariable*> * getConstants();
    XADT * addADT();
    XTimeUnit * addTimeUnit();
    void addFunctionFile(std::string file);
    XMachine * addAgent();
    XMessage * addMessage();

  private:
    std::string name_;
    std::vector<XVariable*> constants_;
    std::vector<XADT*> adts_;
    std::vector<XTimeUnit*> timeUnits_;
    std::vector<std::string> functionFiles_;
    std::vector<XMachine*> agents_;
    std::vector<XMessage*> messages_;
};
}}  // namespace flame::io
#endif  // IO__XMODEL_HPP_
