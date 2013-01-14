/*!
 * \file xparser2/gen_headerfile.hpp
 * \author Shawn Chin
 * \date January 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief generic generator for building header files
 */
#include "../utils.hpp"
#include "gen_headerfile.hpp"

namespace xparser { namespace codegen {

void GenHeaderFile::Generate(Printer* printer) const {
  // Generate unique include guard
  static const char* include_guard_prefix = "__INCLUDE_GUARD_";
  std::string include_guard(include_guard_prefix);
  include_guard += xparser::utils::gen_random_string(12);

  // Insert include guard
  printer->Print("#ifndef $GUARD$\n", "GUARD", include_guard);
  printer->Print("#define $GUARD$\n", "GUARD", include_guard);
  
  // Insert include statements
  GenerateIncludeStatements(printer);
  // content
  GenerateInsertedContent(printer);

  // close include guard
  printer->Print("#endif  // $GUARD$\n", "GUARD", include_guard);
}
}}  // namespace xparser::codegen
