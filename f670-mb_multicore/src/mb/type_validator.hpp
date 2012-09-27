/*!
 * \file src/mb/type_validator.hpp
 * \author Shawn Chin
 * \date September 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Declaration and implementation of the TypeValidator interface class
 */
#ifndef MEM__TYPE_VALIDATOR_HPP_
#define MEM__TYPE_VALIDATOR_HPP_
#include <map>
#include <string>
#include <typeinfo>
#include "exceptions/all.hpp"

namespace flame { namespace mb {

/*!
 * \brief TypeValidator interface
 *
 * Provides the routines and datastructure to quickly validate the existence
 * and datatype of a message variable.
 *
 * Classes that subclass this interface can cache var names and types using
 * RegisterType(). This allows associated messages to quickly validate input
 * using the ValidateType() when performing set/get operations.
 *
 * \note Type information can be extracted from VectorWrappers using
 * VectorWrapperBase::GetDataType().
 */
class TypeValidator {
  public:

    //! Virtual destructor
    virtual ~TypeValidator() {}

    //! Registers an new variable and its type
    virtual void RegisterType(const std::string var_name,
                              const std::type_info* type) {
      type_map_[var_name] = type;
    }

    /*!
     * \brief Validate a name and type against the cached info
     * \return true/false depending on whether the type matches
     *
     * Throws flame::exceptions::invalid_variable if the variable name
     * has not been registered (unknown variable).
     */
    virtual bool ValidateType(const std::string var_name,
                              const std::type_info* type) {
      try {
        return (type_map_.at(var_name) == type);
      } catch(const std::out_of_range& E) {
        throw flame::exceptions::invalid_variable("Invalid variable name");
      }
    }

  protected:
    //! Map which associates type to each var name
    std::map<std::string, const std::type_info*> type_map_;
};
}}  // namespace flame::mb
#endif  // MEM__TYPE_VALIDATOR_HPP_
