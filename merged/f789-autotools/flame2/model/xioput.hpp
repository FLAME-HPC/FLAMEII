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

class XIOput {
  public:
    XIOput();
    ~XIOput();
    void print();
    void setMessageName(std::string name);
    std::string getMessageName();
    void setRandomString(std::string random);
    std::string getRandomString();
    void setRandomSet(bool random);
    bool isRandomSet();
    void setRandom(bool random);
    bool isRandom();
    void setSort(bool sort);
    bool isSort();
    void setSortKey(std::string key);
    std::string getSortKey();
    void setSortOrder(std::string order);
    std::string getSortOrder();
    XCondition * addFilter();
    XCondition * getFilter();

  private:
    std::string messageName_;
    std::string randomString_;
    /*! \brief if random has been read in */
    bool randomSet_;
    bool random_;
    bool sort_;
    std::string sortKey_;
    std::string sortOrder_;
    XCondition * filter_;
};
}}  // namespace flame::model
#endif  // MODEL__XIOPUT_HPP_
