/*!
 * \file xparser2/codegen/gen_file.cpp
 * \author Shawn Chin
 * \date January 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief abstract base class for generating files from a collection of
 *        generators
 */
#include "gen_file.hpp"
namespace xparser { namespace codegen {

void GenFile::Generate(Printer* printer) const {
  // Insert include statements
  GenerateIncludeStatements(printer);
  // content
  GenerateInsertedContent(printer);
}

void GenFile::GenerateInsertedContent(Printer* printer) const {
  GeneratorVector::const_iterator g;
  for (g = generators_.begin(); g != generators_.end(); ++g) {
    g->Generate(printer);
    printer->Print("\n");
  }
}

void GenFile::GenerateIncludeStatements(Printer* printer) const {
  // sys header includes
  StringSet::const_iterator i;
  const StringSet& sysheaders = GetRequiredSysHeaders();
  for (i = sysheaders.begin(); i != sysheaders.end(); ++i) {
    printer->Print("#include <$HEADER$>\n", "HEADER", *i);
  }

  // header includes
  const StringSet& headers = GetRequiredHeaders();
  for (i = headers.begin(); i != headers.end(); ++i) {
    printer->Print("#include \"$HEADER$\"\n", "HEADER", *i);
  }
  printer->Print("\n");
}

}}  // namespace xparser::codegen
