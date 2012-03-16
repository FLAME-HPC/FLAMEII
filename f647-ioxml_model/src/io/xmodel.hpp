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

namespace flame { namespace io {

class XModel {
  public:
    XModel() {}
    ~XModel();
    int clear();
    void print();
    void setName(std::string name) { name_ = name; }
    std::string getName() { return name_; }
    XMachine * addAgent();

  private:
    std::string name_;
    std::vector<XMachine*> agents_;
};
}}  // namespace flame::io
#endif  // IO__XMODEL_HPP_
