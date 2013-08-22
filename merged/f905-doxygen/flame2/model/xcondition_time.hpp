/*!
 * \file flame2/model/xcondition_time.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XConditionTime: holds condition time information
 */
#ifndef MODEL__XCONDITION_TIME_HPP_
#define MODEL__XCONDITION_TIME_HPP_
#include <string>
#include <set>

namespace flame { namespace model {

class XVariable;
class XMachine;
class XMessage;
class XModel;
class XCondition;

/*!
 * \brief Class to hold condition time information
 */
class XConditionTime {
  public:
    XConditionTime();
    /*!
     * \brief Process symbols in time conditions
     * \return Number of errors
     */
    int processSymbolsTime();
    /*!
     * \brief Get time period string
     * \return Time period string
     */
    std::string timePeriod();
    /*!
     * \brief Set time period string
     * \param[in] s Time period string
     */
    void setTimePeriod(std::string s);
    /*!
     * \brief Get time phase variable
     * \return Time phase variable
     */
    std::string timePhaseVariable();
    /*!
     * \brief Set time phase variable
     * \param[in] s Time phase variable
     */
    void setTimePhaseVariable(std::string s);
    /*!
     * \brief Get time duration string
     * \return Time duration string
     */
    std::string timeDurationString();
    /*!
     * \brief Set time duration string
     * \param[in] s Time duration string
     */
    void setTimeDurationString(std::string s);
    /*!
     * \brief Get time phase value
     * \return Time phase value
     */
    int timePhaseValue();
    /*!
     * \brief Set time phase value
     * \param[in] i Time phase value
     */
    void setTimePhaseValue(int i);
    /*!
     * \brief If time phase is a variable
     * * \return Boolean result
     */
    bool timePhaseIsVariable();
    /*!
     * \brief Set if time phase is a variable
     * \param[in] b Boolean value
     */
    void setTimePhaseIsVariable(bool b);
    /*!
     * \brief Get time duration
     * \return Time duration
     */
    int timeDuration();
    /*!
     * \brief Set time duration
     * \param[in] i Time duration
     */
    void setTimeDuration(int i);
    /*!
     * \brief If found time duration
     * * \return Boolean result
     */
    bool foundTimeDuration();
    /*!
     * \brief Set if found time duration
     * \param[in] b Boolean value
     */
    void setFoundTimeDuration(bool b);
    /*!
     * \brief Print time condition
     */
    void printTime();

  private:
    //! \brief Time period string
    std::string timePeriod_;
    //! \brief Time phase variable string
    std::string timePhaseVariable_;
    //! \brief Time duration string
    std::string timeDurationString_;
    //! \brief Time phase integer value
    int timePhaseValue_;
    //! \brief If time phase is a variable
    bool timePhaseIsVariable_;
    //! \brief Time duration integer value
    int timeDuration_;
    //! \brief If found time duration
    bool foundTimeDuration_;
};
}}  // namespace flame::model
#endif  // MODEL__XCONDITION_TIME_HPP_
