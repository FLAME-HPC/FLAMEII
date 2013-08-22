/*!
 * \file flame2/model/xioput.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XIOput: holds input/output information
 */
#ifndef MODEL__XIOPUT_HPP_
#define MODEL__XIOPUT_HPP_
#include <string>
#include "xcondition.hpp"

namespace flame { namespace model {

/*!
 * \brief Class to hold input or output information
 */
class XIOput {
  public:
    /*!
     * \brief Constructor
     *
     * Initialises XIOput with no random, no filter and no sort.
     */
    XIOput();
    /*!
     * \brief Destructor
     *
     * Cleans up XIOput by deleting any filter.
     */
    ~XIOput();
    /*!
     * \brief Prints XIOput
     *
     * Prints XIOput to standard out.
     */
    void print();
    /*!
     * \brief Set the message name
     * \param[in] name The message name
     */
    void setMessageName(std::string name);
    /*!
     * \brief Get the message name
     * \return The message name
     */
    std::string getMessageName();
    /*!
     * \brief Set the random string from a random tag
     * \param[in] random The random tag string
     */
    void setRandomString(std::string random);
    /*!
     * \brief Get the random tag string
     * \return The random tag string
     */
    std::string getRandomString();
    /*!
     * \brief Set that the random tag string has been set
     * \param[in] random The boolean to be set
     */
    void setRandomSet(bool random);
    /*!
     * \brief Is the random tag string set
     * \return The boolean result
     */
    bool isRandomSet();
    /*!
     * \brief Set the input to be randomised
     * \param[in] random The boolean
     */
    void setRandom(bool random);
    /*!
     * \brief Is the input to be randomised
     * \return The boolean result
     */
    bool isRandom();
    /*!
     * \brief Set if the input is to be sorted
     * \param sort The boolean
     */
    void setSort(bool sort);
    /*!
     * \brief Is the input to be sorted
     * \return The boolean result
     */
    bool isSort();
    /*!
     * \brief Set the sort key
     * \param[in] key The key string
     */
    void setSortKey(std::string key);
    /*!
     * \brief Get the sort key
     * \return The sort key string
     */
    std::string getSortKey();
    /*!
     * \brief Set the sort order
     * \param[in] order The sort order string
     */
    void setSortOrder(std::string order);
    /*!
     * \brief Get the sort order
     * \return The sort order string
     */
    std::string getSortOrder();
    /*!
     * \brief Add an input filter
     * \return Pointer to the created filter XCondition
     */
    XCondition * addFilter();
    /*!
     * \brief Get the input filter
     * \return Pointer to the filter XCondition
     */
    XCondition * getFilter();

  private:
    //! \brief The message name
    std::string messageName_;
    //! \brief The string read in from a randon tag
    std::string randomString_;
    //! \brief If random tag has been read in
    bool randomSet_;
    //! \brief If the input is to be randomised
    bool random_;
    //! \brief If the input is to be sorted
    bool sort_;
    //! \brief The sorting key
    std::string sortKey_;
    //! \brief The sorting order
    std::string sortOrder_;
    //! \brief The input filter if used
    XCondition * filter_;
    //! \brief This class has pointer members so disable copy constructor
    XIOput(const XIOput&);
    //! \brief This class has pointer members so disable assignment operation
    void operator=(const XIOput&);
};
}}  // namespace flame::model
#endif  // MODEL__XIOPUT_HPP_
