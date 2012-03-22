/*!
 * \file src/io/xioput.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XIOput: holds input/output information
 */
#ifndef IO__XIOPUT_HPP_
#define IO__XIOPUT_HPP_
#include <string>
#include "./xcondition.hpp"

namespace flame { namespace io {

class XIOput {
  public:
    XIOput();
    ~XIOput();
    void print();
    void setMessageName(std::string name);
    std::string getMessageName();
    void setRandom(bool random);
    bool getRandom();
    void setSort(bool sort);
    bool getSort();
    void setSortKey(std::string key);
    std::string getSortKey();
    void setSortOrder(std::string order);
    std::string getSortOrder();
    XCondition * addFilter();
    XCondition * getFilter();

  private:
    std::string messageName_;
    bool random_;
    bool sort_;
    std::string sortKey_;
    std::string sortOrder_;
    XCondition * filter_;
};
}}  // namespace flame::io
#endif  // IO__XIOPUT_HPP_
