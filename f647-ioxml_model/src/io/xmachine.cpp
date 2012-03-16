/*!
 * \file src/io/xmachine.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XMachine: holds agent information
 */
#include <iostream>
#include "./xmodel.hpp"

namespace flame { namespace io {

void XMachine::setName(std::string name) {
    name_ = name;
}

std::string XMachine::getName() {
    return name_;
}

}}  // namespace flame::io
