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
  printer->Print("// validate model\n");
  printer->Print("model.validate();\n\n");
  printer->Print("double start_time, stop_time, total_time;\n");
  printer->Print("// create simulation using model and path to initial pop"
    ", then run simulation\n");
  printer->Print("try {\n  flame::sim::Simulation s(model, pop_path);\n");
  printer->Print("  // register message datatypes\n");
    MessagenameSet::iterator msg;
    for (msg = messages_.begin(); msg != messages_.end(); ++msg)
      printer->Print(
        "  s.registerMessageType<$MSG$_message_t>(\"$MSG$\");\n", "MSG", (*msg));
  printer->Print("\n  start_time = get_time();\n\n"
    "  // Run simulation\n"
    "  s.start(static_cast<size_t>(num_iters),\n"
    "          static_cast<size_t>(num_cores));\n"
    "} catch (const flame::exceptions::flame_io_exception& e) {\n"
    "  die(std::string(\"Invalid data file\") + e.what());\n}\n");
}

}}  // namespace xparser::codegen
