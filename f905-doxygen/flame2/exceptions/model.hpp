/*!
 * \file flame2/exceptions/model.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Exceptions thrown by model classes
 */
#ifndef EXCEPTIONS__MODEL_HPP_
#define EXCEPTIONS__MODEL_HPP_
#include <string>
#include "base.hpp"

namespace flame { namespace exceptions {

/*!
 * \brief FLAME MODEL exception
 */
class flame_model_exception : public flame_exception {
  public:
    /*!
     * \brief Constructor
     * \param[in] msg Exception message
     */
    explicit flame_model_exception(const std::string& msg)
        : flame_exception(msg) {}
};

}}  // namespace flame::exceptions
#endif  // EXCEPTIONS__MODEL_HPP_
