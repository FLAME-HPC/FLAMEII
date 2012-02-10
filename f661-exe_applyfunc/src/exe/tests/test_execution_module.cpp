/*!
 * \file src/exe/tests/test_execution_module.cpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Test suite for the execution module
 */
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/shared_ptr.hpp>
#include "../execution_engine.hpp"
#include "../execution_thread.hpp"

BOOST_AUTO_TEST_SUITE(ExeModule)

namespace exe = flame::exe::multicore;

// Dummy agent functions
FLAME_AGENT_FUNC(func1) { return *mem; }
FLAME_AGENT_FUNC(func2) { return *mem + 1; }

FLAME_AGENT_FUNC(incr_mem) {
    (*mem)++;
    return 10;
}

BOOST_AUTO_TEST_CASE(test_function_map) {
    exe::ExecutionEngine e;

    BOOST_CHECK_THROW(e.GetFunction("foo"), std::out_of_range);

    e.RegisterFunction("f1", &func1);
    e.RegisterFunction("f2", &func2);
    int i = 1;
    BOOST_CHECK_THROW(e.GetFunction("foo"), std::out_of_range);
    BOOST_CHECK_EQUAL(e.GetFunction("f1")(&i), 1);
    BOOST_CHECK_EQUAL(e.GetFunction("f2")(&i), 2);
}

BOOST_AUTO_TEST_CASE(test_run_function) {
    exe::ExecutionEngine e;
    e.RegisterFunction("incr_mem", &incr_mem);

    exe::MemVectorPtr mem(new exe::MemVector());
    mem->push_back(1);
    mem->push_back(2);
    mem->push_back(3);

    e.RunFunction("incr_mem", mem);
    BOOST_CHECK_EQUAL(mem->at(0), 2);
    BOOST_CHECK_EQUAL(mem->at(1), 3);
    BOOST_CHECK_EQUAL(mem->at(2), 4);

    e.RunFunction("incr_mem", mem);
    BOOST_CHECK_EQUAL(mem->at(0), 3);
    BOOST_CHECK_EQUAL(mem->at(1), 4);
    BOOST_CHECK_EQUAL(mem->at(2), 5);
}

BOOST_AUTO_TEST_SUITE_END()
