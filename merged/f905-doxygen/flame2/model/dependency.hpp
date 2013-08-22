/*!
 * \file flame2/model/dependency.hpp
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

/*!
 * \brief Class to hold a graph dependency
 */
class Dependency {
  public:
    /*!
     * \brief Enumeration of dependency types
     */
    enum DependencyType { state = 0, communication, data, init,
        condition, variable, blank };
    /*!
     * \brief Constructor
     * \param[in] name Dependency name
     * \param[in] type Dependency type
     */
    Dependency(std::string name, DependencyType type);
    /*!
     * \brief Set dependency name
     * \param[in] name The name
     */
    void setName(std::string name);
    /*!
     * \brief Get dependency name
     * \return The name
     */
    std::string getName();
    /*!
     * \brief Set dependency type
     * \param[in] type The type
     */
    void setDependencyType(DependencyType type);
    /*!
     * \brief Get dependency type
     * \return The dependency type
     */
    DependencyType getDependencyType();
    /*!
     * \brief Get a name from the dependency type
     * \return Dependency type name description
     */
    std::string getGraphName();

  private:
    //! \brief Dependency name
    std::string name_;
    //! \brief Dependency name
    DependencyType dependencyType_;
};
}}  // namespace flame::model
#endif  // MODEL__DEPENDENCY_HPP_
