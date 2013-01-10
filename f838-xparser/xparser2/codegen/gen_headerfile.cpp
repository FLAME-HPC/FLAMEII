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

void GenHeaderFile::Generate(Printer& printer) const {
  static const char* include_guard_prefix = "__INCLUDE_GUARD_";
  
  // Generate unique include guard 
  std::string include_guard(include_guard_prefix);
  include_guard += xparser::utils::gen_random_string(12);

  // Insert include guard
  printer.Print("#ifndef $GUARD$\n", "GUARD", include_guard);
  printer.Print("#define $GUARD$\n", "GUARD", include_guard);
  
  // sys header includes
  StringSet::const_iterator i;
  const StringSet& sysheaders = GetRequiredSysHeaders();
  for (i = sysheaders.begin(); i != sysheaders.end(); ++i) {
    printer.Print("#include <$HEADER$>\n", "HEADER", *i);
  }
  
  // header includes
  const StringSet& headers = GetRequiredHeaders();
  for (i = headers.begin(); i != headers.end(); ++i) {
    printer.Print("#include \"$HEADER$\"\n", "HEADER", *i);
  }
  printer.Print("\n");
  
  // content
  GeneratorVector::const_iterator g;
  for (g = generators_.begin(); g != generators_.end(); ++g) {
    g->Generate(printer);
    printer.Print("\n");
  }

  // close include guard
  printer.Print("#endif  // $GUARD$\n", "GUARD", include_guard);
}

}}  // namespace xparser::codegen
