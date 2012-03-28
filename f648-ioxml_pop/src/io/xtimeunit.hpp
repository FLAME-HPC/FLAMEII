/*!
 * \file src/io/xtimeunit.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XTimeUnit: holds time unit information
 */
#ifndef IO__XTIMEUNIT_HPP_
#define IO__XTIMEUNIT_HPP_
#include <boost/cstdint.hpp>
#include <string>

namespace flame { namespace io {

class XTimeUnit {
  public:
    XTimeUnit() {}
    ~XTimeUnit();
    void print();
    void setName(std::string name);
    std::string getName();
    void setUnit(std::string unit);
    std::string getUnit();
    void setPeriodString(std::string period);
    std::string getPeriodString();
    void setPeriod(int period);
    int getPeriod();

  private:
    std::string name_;
    std::string unit_;
    std::string periodString_;
    int period_;
};
}}  // namespace flame::io
#endif  // IO__XTIMEUNIT_HPP_
