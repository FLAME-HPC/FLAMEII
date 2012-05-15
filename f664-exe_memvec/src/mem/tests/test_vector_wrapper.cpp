/*!
 * \file src/mem/tests/test_vector_wrapper.cpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Test suite for the memory module
 */
#define BOOST_TEST_DYN_LINK
#include <vector>
#include <boost/test/unit_test.hpp>
#include "../vector_wrapper.hpp"
#include "exceptions/mem.hpp"

namespace m = flame::mem;
namespace e = flame::exceptions;

BOOST_AUTO_TEST_SUITE(Test)

BOOST_AUTO_TEST_CASE(test_vector_wrapper) {
  m::VectorWrapperBase* pi = new m::VectorWrapper<int>();
  m::VectorWrapperBase* pd = new m::VectorWrapper<double>();

  BOOST_CHECK(*(pi->GetDataType()) == typeid(int));
  BOOST_CHECK(*(pd->GetDataType()) == typeid(double));

  std::vector<int>* vi = static_cast<std::vector<int>*>(pi->GetVectorPtr());
  std::vector<double>* vd = static_cast<std::vector<double>*>(
                                                        pd->GetVectorPtr());

  BOOST_CHECK(vi->empty());
  BOOST_CHECK(pi->empty());
  vi->push_back(1);
  vi->push_back(2);
  vi->push_back(3);
  BOOST_CHECK_EQUAL(vi->size(), 3);
  BOOST_CHECK_EQUAL(pi->size(), 3);

  BOOST_CHECK(vd->empty());
  BOOST_CHECK(pd->empty());
  vd->push_back(10.1);
  vd->push_back(20.2);
  vd->push_back(30.3);
  BOOST_CHECK_EQUAL(vd->size(), 3);
  BOOST_CHECK_EQUAL(pd->size(), 3);

  // ---- iterate using raw pointers ---

  void* i = pi->GetRawPtr();
  int sum = 0;
  while (i != NULL) {
    sum += *static_cast<int*>(i);
    i = pi->StepRawPtr(i);
  }
  BOOST_CHECK_EQUAL(sum, 6);

  void* d = pd->GetRawPtr();
  double sum2 = 0.0;
  while (d != NULL) {
    sum2 += *static_cast<double*>(d);
    d = pd->StepRawPtr(d);
  }
  BOOST_CHECK_CLOSE(sum2, 60.6, 0.000001);

  delete pi;
  delete pd;
}


BOOST_AUTO_TEST_SUITE_END()
