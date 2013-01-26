/*!
 * \file tests/xparser2/test_xparser2_code_generator.cpp
 * \author Shawn Chin
 * \date January 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Test suite for xparser2
 */
#define BOOST_TEST_DYN_LINK
#include <set>
#include <string>
#include <sstream>
#include <boost/test/unit_test.hpp>
#include "xparser2/printer.hpp"
#include "xparser2/codegen/code_generator.hpp"

BOOST_AUTO_TEST_SUITE(XPARSER2)

namespace {

class DummyCodeGenerator : public xparser::codegen::CodeGenerator {
  public:
    void Generate(Printer* printer) const {
      printer->Print("// Testing DummyCodeGenerator\n");
    }
};

class DummyCodeGeneratorWithHeader : public xparser::codegen::CodeGenerator {
  public:
    DummyCodeGeneratorWithHeader() {
      RequireSysHeader("string");
      RequireSysHeader("vector");
      RequireSysHeader("string");  // test duplicate removal

      RequireHeader("hello/world.hpp");
      RequireHeader("goodbye/world.hpp");
      RequireHeader("hello/world.hpp");  // test duplicate removal
    }

    void Generate(Printer* printer) const {
      printer->Print("// Testing DummyCodeGeneratorWithHeader\n");
    }
};

}  // end anonymous namespace

BOOST_AUTO_TEST_CASE(xp_codegen) {
  DummyCodeGenerator dummy;
  DummyCodeGeneratorWithHeader header;

  std::set<std::string> expected_headers;
  expected_headers.insert("hello/world.hpp");
  expected_headers.insert("goodbye/world.hpp");

  std::set<std::string> expected_sys_headers;
  expected_sys_headers.insert("string");
  expected_sys_headers.insert("vector");

  // check header requirements queries
  BOOST_CHECK_EQUAL(dummy.GetRequiredHeaders().size(), 0);
  BOOST_CHECK_EQUAL(dummy.GetRequiredSysHeaders().size(), 0);

  BOOST_CHECK_EQUAL(header.GetRequiredHeaders().size(), 2);
  BOOST_CHECK_EQUAL(header.GetRequiredSysHeaders().size(), 2);
  BOOST_CHECK(header.GetRequiredHeaders() == expected_headers);
  BOOST_CHECK(header.GetRequiredSysHeaders() == expected_sys_headers);

  // check code generation
  std::ostringstream dummy_out;
  xparser::Printer dummy_p(&dummy_out);
  dummy.Generate(&dummy_p);
  BOOST_CHECK_EQUAL(dummy_out.str(), "// Testing DummyCodeGenerator\n");

  std::ostringstream header_out;
  xparser::Printer header_p(&header_out);
  header.Generate(&header_p);
  BOOST_CHECK_EQUAL(header_out.str(),
                    "// Testing DummyCodeGeneratorWithHeader\n");
}

BOOST_AUTO_TEST_SUITE_END()


