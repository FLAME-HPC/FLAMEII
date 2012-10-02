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
Dependency::Dependency(std::string name, DependencyType type)
    : name_(name), dependencyType_(type)
{}

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

std::string Dependency::getGraphName() {
    std::string graphName;
    if (dependencyType_ == Dependency::communication)
        graphName.append("Message: ");
    else if (dependencyType_ == Dependency::data)
        graphName.append("Memory: ");
    else if (dependencyType_ == Dependency::state)
        graphName.append("State: ");
    else if (dependencyType_ == Dependency::init)
        graphName.append("Initialise: ");
    graphName.append(name_);
    return graphName;
}

}}  // namespace flame::model
