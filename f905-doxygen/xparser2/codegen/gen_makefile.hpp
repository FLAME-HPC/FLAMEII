/*!
 * \file xparser2/codegen/gen_makefile.hpp
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

//! Generates a model Makefile
class GenMakefile : public CodeGenerator {
  public:
    typedef std::set<std::string> StringSet;  //! Set of strings

    //! Adds a header file to the list of project files
    void AddHeaderFile(const std::string& filename);
    //! Adds a source file to the list of project files
    void AddSourceFile(const std::string& filename);
    //! Prints the Makefile content to the printer instance
    void Generate(Printer* printer) const;

  private:
    StringSet headers_;  //! Collection of header files
    StringSet sources_;  //! Collection of source files
};

}}  // namespace xparser::codegen
#endif  // XPARSER__CODEGEN__GEN_MAKEFILE_HPP_
