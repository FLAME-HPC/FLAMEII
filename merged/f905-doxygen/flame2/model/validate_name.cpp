/*!
 * \file flame2/model/validate_name.cpp
 * \author Simon Coakley
 * \date 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief ValidateName: validates names
 */
#include <cctype>
#include <string>
#include <algorithm>
#include "flame2/config.hpp"
#include "validate_name.hpp"

namespace flame { namespace model {

/*!
 * \brief Check if a character is disallowed in a name
 * \param[in] c The character
 * \return Boolean result
 */
bool char_is_disallowed(char c) {
  return !(isalnum(c) || c == '_' || c == '-');
}

bool ValidateName::name_is_allowed(std::string name) {
  return (std::find_if(name.begin(), name.end(),
      char_is_disallowed) == name.end());
}

}}  // namespace flame::model
