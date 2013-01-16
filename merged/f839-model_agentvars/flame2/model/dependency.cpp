/*!
 * \file flame2/model/dependency.cpp
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
#include "flame2/config.hpp"
#include "dependency.hpp"

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
    // If communication
    if (dependencyType_ == Dependency::communication)
        graphName.append("Message");
    // If data
    else if (dependencyType_ == Dependency::data ||
            dependencyType_ == Dependency::variable)
        graphName.append("Data");
    // If state
    else if (dependencyType_ == Dependency::state)
        graphName.append("State");
    // If condition
    else if (dependencyType_ == Dependency::condition)
            graphName.append("Condition");
    // If initialisation
    else if (dependencyType_ == Dependency::init)
        graphName.append("");
    // graphName.append(name_);
    return graphName;
}

}}  // namespace flame::model
