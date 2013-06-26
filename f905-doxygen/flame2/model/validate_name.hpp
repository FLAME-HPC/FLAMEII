/*!
 * \file flame2/model/validate_name.hpp
 * \author Simon Coakley
 * \date 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief ValidateName: validates names
 */
#ifndef MODEL__VALIDATE_NAME_HPP_
#define MODEL__VALIDATE_NAME_HPP_
#include <string>

namespace flame { namespace model {

class ValidateName {
  public:
    /*!
     * \brief Validates a name string
     * \param[in] name The string to validate
     * \return Boolean result
     * The iterator tries to find the first element that satisfies the predicate.
     * If no disallowed character is found then you end with name.end() which is
     * taken to be success and true is returned.
     */
    static bool name_is_allowed(std::string name);
};

}}  // namespace flame::model
#endif  // MODEL__VALIDATE_NAME_HPP_
