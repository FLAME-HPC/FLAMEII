/*!
 * \file src/model/dependency.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Dependency: holds task dependency information
 */
#include <cstdio>
#include <string>
#include <vector>
#include "./dependency.hpp"

namespace flame { namespace model {

/*!
 * \brief Initialises Dependency
 *
 * Initialises Dependency.
 */
Dependency::Dependency() {
    name_ = "";
}

void Dependency::setName(std::string name) {
    name_ = name;
}

std::string Dependency::getName() {
    return name_;
}

void Dependency::setDependencyType(DependencyType type) {
    dependencyType_ = type;
}

Dependency::DependencyType Dependency::getDependencyType() {
    return dependencyType_;
}

}}  // namespace flame::model
