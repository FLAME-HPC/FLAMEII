/*!
 * \file xparser2/gen_makefile.hpp
 * \author Shawn Chin
 * \date January 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Generator for model Makefile
 */
#ifndef XPARSER__CODEGEN__GEN_MAKEFILE_HPP_
#define XPARSER__CODEGEN__GEN_MAKEFILE_HPP_
#include <set>
#include <string>
#include "code_generator.hpp"
namespace xparser { namespace codegen {

class GenMakefile : public CodeGenerator {
  public:
    typedef std::set<std::string> StringSet;
    
    GenMakefile() {}
    void AddHeaderFile(const std::string& filename);
    void AddSourceFile(const std::string& filename);
    void Generate(Printer& printer) const;
    
  private:
    StringSet headers_;
    StringSet sources_;
};
  
}}  // namespace xparser::codegen
#endif  // XPARSER__CODEGEN__GEN_MAKEFILE_HPP_
