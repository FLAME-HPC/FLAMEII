/*!
 * \file src/model/dependency.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Dependency: holds task dependency information
 */
#ifndef MODEL__DEPENDENCY_HPP_
#define MODEL__DEPENDENCY_HPP_
#include <string>
#include <vector>

namespace flame { namespace model {

class Task;

class Dependency {
  public:
    enum DependencyType { state = 0, communication, data, init, condition };
    Dependency();
    ~Dependency() {}
    void setName(std::string name);
    std::string getName();
    void setParentName(std::string name);
    std::string getParentName();
    void setDependencyType(DependencyType type);
    DependencyType getDependencyType();
    std::string getGraphName();

  private:
    /* State name/Message name/Memory variable name */
    std::string name_;
    /* Agent/Message name */
    std::string parentName_;
    DependencyType dependencyType_;
};
}}  // namespace flame::model
#endif  // MODEL__DEPENDENCY_HPP_
