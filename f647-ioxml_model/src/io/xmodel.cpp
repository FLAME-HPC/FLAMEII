/*!
 * \file src/io/xmodel.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XModel: holds model information
 */
#include <cstdio>
#include "./xmodel.hpp"

namespace flame { namespace io {

XModel::~XModel() {
    clear();
}

int XModel::clear() {
    name_ = "";
    /* Clear agents vector */
    XMachine * xm;
    while (!agents_.empty()) {
        xm = agents_.back();
        delete xm;
        agents_.pop_back();
    }

    return 0;
}

void XModel::print() {
    std::fprintf(stdout, "Model Name: %s\n", name_.c_str());
    unsigned int ii;
    for (ii = 0; ii < agents_.size(); ii++) {
        agents_[ii]->print();
    }
}

XMachine * XModel::addAgent() {
    XMachine * xmachine = new XMachine;
    agents_.push_back(xmachine);
    return xmachine;
}

}}  // namespace flame::io
