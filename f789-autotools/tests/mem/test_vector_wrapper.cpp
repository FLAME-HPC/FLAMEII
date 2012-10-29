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
#include <sstream>
#include <boost/test/unit_test.hpp>
#include "flame2/mem/vector_wrapper.hpp"
#include "flame2/exceptions/mem.hpp"

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
  BOOST_CHECK_EQUAL(vi->size(), (size_t)3);
  BOOST_CHECK_EQUAL(pi->size(), (size_t)3);

  std::stringstream stream1;
  pi->OutputToStream(stream1, ", ");
  BOOST_CHECK_EQUAL("1, 2, 3", stream1.str());

  BOOST_CHECK(vd->empty());
  BOOST_CHECK(pd->empty());
  vd->push_back(10.1);
  vd->push_back(20.2);
  vd->push_back(30.3);
  BOOST_CHECK_EQUAL(vd->size(), (size_t)3);
  BOOST_CHECK_EQUAL(pd->size(), (size_t)3);

  std::stringstream stream2;
  pd->OutputToStream(stream2, ", ");
  BOOST_CHECK_EQUAL("10.1, 20.2, 30.3", stream2.str());

  // check GetRawPtr with different offsets
  BOOST_CHECK_THROW(pi->GetRawPtr(100), flame::exceptions::invalid_argument);
  BOOST_CHECK_EQUAL(1, *static_cast<int*>(pi->GetRawPtr()));
  BOOST_CHECK_EQUAL(3, *static_cast<int*>(pi->GetRawPtr(2)));
  BOOST_CHECK_EQUAL(2, *static_cast<int*>(pi->GetRawPtr(1)));

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
