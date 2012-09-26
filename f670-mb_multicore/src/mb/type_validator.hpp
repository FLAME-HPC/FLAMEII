/*!
 * \file src/mb/type_validator.hpp
 * \author Shawn Chin
 * \date September 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief TypeValidator interface class.
 */
#ifndef MEM__TYPE_VALIDATOR_HPP_
#define MEM__TYPE_VALIDATOR_HPP_
#include <map>
#include <string>
#include <typeinfo>
#include "exceptions/all.hpp"
namespace flame { namespace mb {
class TypeValidator {
  public:
    virtual ~TypeValidator() {}

    virtual void RegisterType(const std::string var_name,
                              const std::type_info* type) {
      type_map_[var_name] = type;
    }

    virtual bool ValidateType(const std::string var_name,
                              const std::type_info* type) {
      try {
        return (type_map_.at(var_name) == type);
      } catch(const std::out_of_range& E) {
        throw flame::exceptions::invalid_variable("Invalid variable name");
      }
    }

  protected:
    std::map<std::string, const std::type_info*> type_map_;
};
}}  // namespace flame::mb
#endif  // MEM__TYPE_VALIDATOR_HPP_
