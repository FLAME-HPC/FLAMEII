/*!
 * \file xparser2/gen_makefile.cpp
 * \author Shawn Chin
 * \date January 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Generator for model Makefile
 */
#include <boost/algorithm/string/join.hpp>
#include "flame2/exceptions/mem.hpp"
#include "../utils.hpp"
#include "gen_makefile.hpp"
namespace xparser { namespace codegen {

void GenMakefile::AddHeaderFile(const std::string& filename) {
  headers_.insert(filename);
}

void GenMakefile::AddSourceFile(const std::string& filename) {
  sources_.insert(filename);
}

void GenMakefile::Generate(Printer& printer) const {
  // first, we make sure the template exists
  static const char* tmpl_name = "Makefile.tmpl";
  std::string makefile_tmpl = xparser::utils::locate_template(tmpl_name);
  if (makefile_tmpl.empty()) {
    throw flame::exceptions::file_not_found(
      std::string("Could not locate template file") + tmpl_name);
  }

  // Define list of source and header files
  printer.Print("SOURCES = $FILES$\n", "FILES",
                boost::algorithm::join(sources_, " "));
  printer.Print("HEADERS = $FILES$\n", "FILES",
                boost::algorithm::join(headers_, " "));
                   
  // Append template to output
  printer.PrintRaw("\n");
  printer.PrintRawFromFile(makefile_tmpl);
}

}}  // namespace xparser::codegen
