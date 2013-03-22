/*!
 * \file xparser2/gen_simulation.cpp
 * \author Shawn Chin
 * \author Simon Coakley
 * \date January 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Generate code to setup simulation
 */
#include <string>
#include "flame2/exceptions/base.hpp"
#include "gen_simulation.hpp"
namespace xparser { namespace codegen {

GenSimulation::GenSimulation() {}

void GenSimulation::AddMessage(const std::string& message_name) {
  if (messages_.find(message_name) != messages_.end()) {
    throw flame::exceptions::logic_error("message already exists");
  }
  messages_.insert(message_name);
}

void GenSimulation::Generate(Printer* printer) const {
  printer->Print("flame::sim::SimManager sim_mgr;\n");
  printer->Print("// set local plugin directory\n");
  printer->Print("sim_mgr.includeIOPluginDirectory(plugin_dir);\n");
  printer->Print("// set population input and output types\n");
  printer->Print("sim_mgr.setPopInputType(input_type);\n");
  printer->Print("sim_mgr.setPopOutputType(output_type);\n");
  printer->Print("// register message types\n");
  MessagenameSet::iterator msg;
  for (msg = messages_.begin(); msg != messages_.end(); ++msg)
    printer->Print("sim_mgr.registerMessageType<$MSG$_message_t>"
      "(\"$MSG$\");\n", "MSG", (*msg));
}

}}  // namespace xparser::codegen
