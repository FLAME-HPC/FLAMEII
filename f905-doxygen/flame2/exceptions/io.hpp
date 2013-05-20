/*!
 * \file flame2/exceptions/io.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Exceptions thrown by io managers
 */
#ifndef EXCEPTIONS__IO_HPP_
#define EXCEPTIONS__IO_HPP_
#include <string>
#include "base.hpp"

namespace flame { namespace exceptions {

/*!
 * \brief FLAME IO exception
 */
class flame_io_exception : public flame_exception {
  public:
    /*!
     * \brief Constructor
     * \param[in] msg Exception message
     */
    explicit flame_io_exception(const std::string& msg)
        : flame_exception(msg) {}
};

/*!
 * \brief FLAME IO invalid file type exception
 */
class invalid_file_type : public flame_io_exception {
  public:
    /*!
     * \brief Constructor
     * \param[in] msg Exception message
     */
    explicit invalid_file_type(const std::string& msg)
        : flame_io_exception(msg) {}
};

/*!
 * \brief FLAME IO inaccessible file exception
 */
class inaccessible_file : public flame_io_exception {
  public:
    /*!
     * \brief Constructor
     * \param[in] msg Exception message
     */
    explicit inaccessible_file(const std::string& msg)
        : flame_io_exception(msg) {}
};

/*!
 * \brief FLAME IO unparseable file exception
 */
class unparseable_file : public flame_io_exception {
  public:
    /*!
     * \brief Constructor
     * \param[in] msg Exception message
     */
    explicit unparseable_file(const std::string& msg)
        : flame_io_exception(msg) {}
};

/*!
 * \brief FLAME IO invalid model file exception
 */
class invalid_model_file : public flame_io_exception {
  public:
    /*!
     * \brief Constructor
     * \param[in] msg Exception message
     */
    explicit invalid_model_file(const std::string& msg)
        : flame_io_exception(msg) {}
};

/*!
 * \brief FLAME IO invalid pop file exception
 */
class invalid_pop_file : public flame_io_exception {
  public:
    /*!
     * \brief Constructor
     * \param[in] msg Exception message
     */
    explicit invalid_pop_file(const std::string& msg)
        : flame_io_exception(msg) {}
};

}}  // namespace flame::exceptions
#endif  // EXCEPTIONS__IO_HPP_
