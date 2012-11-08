/*!
 * \file flame2/model/xtimeunit.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XTimeUnit: holds time unit information
 */
#include <cstdio>
#include <string>
#include "flame2/config.hpp"
#include "xtimeunit.hpp"

namespace flame { namespace model {

XTimeUnit::XTimeUnit() : period_(0), id_(0) {
}

XTimeUnit::~XTimeUnit() {
}

/*!
 * \brief Prints XTimeUnit
 *
 * Prints XTimeUnit to standard out including
 * name,
 * unit and
 * period.
 */
void XTimeUnit::print() {
    std::fprintf(stdout, "\tTime Unit Name: %s\n", getName().c_str());
    std::fprintf(stdout, "\tUnit  : %s\n", getUnit().c_str());
    std::fprintf(stdout, "\tPeriod: %d\n", getPeriod());
}

void XTimeUnit::setName(std::string name) {
    name_ = name;
}

std::string XTimeUnit::getName() {
    return name_;
}

void XTimeUnit::setUnit(std::string unit) {
    unit_ = unit;
}

std::string XTimeUnit::getUnit() {
    return unit_;
}

void XTimeUnit::setPeriodString(std::string period) {
    periodString_ = period;
}

std::string XTimeUnit::getPeriodString() {
    return periodString_;
}

void XTimeUnit::setPeriod(int period) {
    period_ = period;
}

int XTimeUnit::getPeriod() {
    return period_;
}

void XTimeUnit::setID(int id) {
    id_ = id;
}

int XTimeUnit::getID() {
    return id_;
}

}}  // namespace flame::model
