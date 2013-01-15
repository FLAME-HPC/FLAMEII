/*!
 * \file tests/xparser2/test_xparser2_file_generator.cpp
 * \author Shawn Chin
 * \date January 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Test suite for xparser2
 */
#define BOOST_TEST_DYN_LINK
#include <cstdio>
#include <fstream>
#include <boost/test/unit_test.hpp>
#include "xparser2/utils.hpp"
#include "xparser2/file_generator.hpp"
#include "xparser2/codegen/gen_file.hpp"

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

std::string get_file_contents(const char *filename)
{
  std::ifstream in(filename, std::ios::in | std::ios::binary);
  if (in) {
    std::string contents;
    in.seekg(0, std::ios::end);
    contents.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&contents[0], contents.size());
    in.close();
    return(contents);
  }
  return "";
}

}  // end anonymous namespace

BOOST_AUTO_TEST_CASE(xp_filegen) {
  // instantiate generator for output file
  xparser::codegen::GenFile outfile_generator;

  // append some content
  DummyCodeGeneratorWithHeader h1, h2;
  DummyCodeGenerator d1;
  outfile_generator.Insert(h1);
  outfile_generator.Insert(d1);
  outfile_generator.Insert(h2);
  outfile_generator.Insert(h1);

  // expected output content
  std::string expected_output("#include <string>\n" 
                              "#include <vector>\n"
                              "#include \"goodbye/world.hpp\"\n"
                              "#include \"hello/world.hpp\"\n\n"
                              "// Testing DummyCodeGeneratorWithHeader\n\n"
                              "// Testing DummyCodeGenerator\n\n"
                              "// Testing DummyCodeGeneratorWithHeader\n\n"
                              "// Testing DummyCodeGeneratorWithHeader\n\n");
                             
  // generate output filename
  std::string outfilename("testfile_");
  outfilename += xparser::utils::gen_random_string(5);

  // get file generator instance to manage file output
  xparser::FileGenerator f;
  f.Output(outfilename, outfile_generator);

  // check file content
  std::string file_content = get_file_contents(outfilename.c_str());
  BOOST_CHECK(file_content == expected_output);
  // delete file
  remove(outfilename.c_str());
  
  // --- generate output in a different directory
  xparser::FileGenerator f2("./xparser2/data");
  f2.Output(outfilename, outfile_generator);

  // check file content
  std::string outfilename_new = std::string("./xparser2/data/") + outfilename;
  std::string file_content2 = get_file_contents(outfilename_new.c_str());
  BOOST_CHECK(file_content2 == expected_output);
  // delete file
  remove(outfilename_new.c_str());
}

BOOST_AUTO_TEST_SUITE_END()
