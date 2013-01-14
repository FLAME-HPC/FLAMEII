/*!
 * \file xparser2/gen_maincpp.cpp
 * \author Shawn Chin
 * \date January 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief generator for main.cpp file to drive flame simulation
 */
#include "flame2/exceptions/base.hpp"
#include "../utils.hpp"
#include "gen_maincpp.hpp"

namespace xparser { namespace codegen {

GenMainCpp::GenMainCpp() {
  // System headers used
  RequireSysHeader("cstdio");
  RequireSysHeader("string");
  RequireSysHeader("iostream");
  RequireSysHeader("sys/time.h");
}

void GenMainCpp::Generate(Printer* printer) const {
  // Insert include statements
  GenerateIncludeStatements(printer);

  // Insert boiler plate code to start main() function
  static const char* header_tmpl = "main_header.cpp.tmpl";
  std::string tmpl = xparser::utils::locate_template(header_tmpl);
  if (tmpl.empty()) {
    throw flame::exceptions::file_not_found(
      std::string("Could not locate template file") + header_tmpl);
  }
  printer->Print("\n");
  printer->PrintRawFromFile(tmpl);
  printer->Print("\n");
  
  // Insert content from attached generators
  printer->Indent();
  GenerateInsertedContent(printer);
  printer->Outdent();

  // Insert boiler plate code to start main() function
  static const char* footer_tmpl = "main_footer.cpp.tmpl";
  std::string tmpl2 = xparser::utils::locate_template(footer_tmpl);
  if (tmpl2.empty()) {
    throw flame::exceptions::file_not_found(
      std::string("Could not locate template file") + footer_tmpl);
  }
  printer->Print("\n");
  printer->PrintRawFromFile(tmpl2);
}
}}  // namespace xparser::codegen

