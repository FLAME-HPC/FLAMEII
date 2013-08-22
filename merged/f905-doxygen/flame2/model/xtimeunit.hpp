/*!
 * \file flame2/model/xtimeunit.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XTimeUnit: holds time unit information
 */
#ifndef MODEL__XTIMEUNIT_HPP_
#define MODEL__XTIMEUNIT_HPP_
#include <boost/cstdint.hpp>
#include <string>

namespace flame { namespace model {

/*!
 * \brief This class holds model time unit information
 */
class XTimeUnit {
  public:
    /*!
     * \brief Constructor
     *
     * Initialises period and id to be zero
     */
    XTimeUnit();
    /*!
     * \brief Prints XTimeUnit
     *
     * Prints XTimeUnit to standard out including
     * name,
     * unit and
     * period.
     */
    void print();
    /*!
     * \brief Set the name
     * \param[in] name The name
     */
    void setName(std::string name);
    /*!
     * \brief Get the name
     * \return The name
     */
    std::string getName();
    /*!
     * \brief Set the unit
     * \param[in] unit The unit name
     */
    void setUnit(std::string unit);
    /*!
     * \brief Get the unit name
     * \return The unit name
     */
    std::string getUnit();
    /*!
     * \brief Set the period using a string
     * \param[in] period The period as a string
     */
    void setPeriodString(std::string period);
    /*!
     * \brief Get the period as a string
     * \return The period as a string
     */
    std::string getPeriodString();
    /*!
     * \brief Set the period as an integer
     * \param[in] period The period integer
     */
    void setPeriod(int period);
    /*!
     * \brief Get the period as an integer
     * \return The period integer
     */
    int getPeriod();
    /*!
     * \brief Set the ID
     * \param[in] id The ID
     */
    void setID(int id);
    /*!
     * \brief Get the ID
     * \return The ID
     */
    int getID();

  private:
    //! The name
    std::string name_;
    //! The unit name
    std::string unit_;
    //! The period as a string
    std::string periodString_;
    //! The period as an integer
    int period_;
    //! \todo What is the id_ used for?
    int id_;
};
}}  // namespace flame::model
#endif  // MODEL__XTIMEUNIT_HPP_
