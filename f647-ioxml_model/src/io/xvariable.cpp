/*!
 * \file src/io/xvariable.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XVariable: holds agent variable information
 */
#include <string>
#include <cstdio>
#include "./xvariable.hpp"

namespace flame { namespace io {

void XVariable::print() {
    std::fprintf(stdout, "\t%s %s\n",
            getType().c_str(), getName().c_str());
}

void XVariable::setName(std::string name) {
    name_ = name;
}

std::string XVariable::getName() {
    return name_;
}

void XVariable::setType(std::string type) {
    type_ = type;
}

std::string XVariable::getType() {
    return type_;
}

}}  // namespace flame::io
