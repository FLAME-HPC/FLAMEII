/*!
 * \file flame2/model/xioput.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XIOput: holds input/output information
 */
#include <cstdio>
#include <string>
#include <vector>
#include <stdexcept>
#include "flame2/config.hpp"
#include "xioput.hpp"

namespace flame { namespace model {

XIOput::XIOput()
  : messageName_(), randomString_(), randomSet_(false), random_(false),
    sort_(false), sortKey_(), sortOrder_(), filter_(0) {}

XIOput::~XIOput() {
  /* Delete any filter */
  delete filter_;
}

void XIOput::print() {
  std::fprintf(stdout, "\t\t\tMessage Name: %s\n", getMessageName().c_str());
  if (random_) std::fprintf(stdout, "\t\t\tRandom: true\n");
  if (sort_)
    std::fprintf(stdout,
        "\t\t\tSort:\n\t\t\t\tKey: %s\n\t\t\t\tOrder: %s\n",
        getSortKey().c_str(),
        getSortOrder().c_str());
  if (filter_ != 0) {
    std::fprintf(stdout, "\t\t\tFilter:\n");
    filter_->print();
  }
}

void XIOput::setMessageName(std::string name) {
  messageName_ = name;
}

std::string XIOput::getMessageName() {
  return messageName_;
}

void XIOput::setRandomString(std::string random) {
  randomString_ = random;
}

std::string XIOput::getRandomString() {
  return randomString_;
}

void XIOput::setRandomSet(bool random) {
  randomSet_ = random;
}

bool XIOput::isRandomSet() {
  return randomSet_;
}

void XIOput::setRandom(bool random) {
  random_ = random;
}

bool XIOput::isRandom() {
  return random_;
}

void XIOput::setSort(bool sort) {
  sort_ = sort;
}

bool XIOput::isSort() {
  return sort_;
}

void XIOput::setSortKey(std::string key) {
  sortKey_ = key;
}

std::string XIOput::getSortKey() {
  return sortKey_;
}

void XIOput::setSortOrder(std::string order) {
  sortOrder_ = order;
}

std::string XIOput::getSortOrder() {
  return sortOrder_;
}

XCondition * XIOput::addFilter() {
  if (filter_ == 0) filter_ = new XCondition;
  else
    throw std::invalid_argument(
        "a filter has already been added to the input");
  return filter_;
}

XCondition * XIOput::getFilter() {
  return filter_;
}

}}  // namespace flame::model
