#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Flame Test Suite
#endif
#include <boost/test/unit_test.hpp>
#include "../execution_engine.hpp"

BOOST_AUTO_TEST_SUITE(ExeModule)

namespace exe = flame::exe::multicore;

// Dummy agent functions
FLAME_AGENT_FUNC(func1) { return 1; }
FLAME_AGENT_FUNC(func2) { return 2; }


BOOST_AUTO_TEST_CASE(test_function_map) {
    exe::ExecutionEngine e;

    BOOST_CHECK_THROW(e.GetFunction("foo"), std::out_of_range);

    e.RegisterFunction("f1", &func1);
    e.RegisterFunction("f2", &func2);
    BOOST_CHECK_THROW(e.GetFunction("foo"), std::out_of_range);
    BOOST_CHECK_EQUAL(e.GetFunction("f1")(), 1);
    BOOST_CHECK_EQUAL(e.GetFunction("f2")(), 2);
}

BOOST_AUTO_TEST_SUITE_END()
