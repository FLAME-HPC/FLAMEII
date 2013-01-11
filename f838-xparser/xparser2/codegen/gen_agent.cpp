/*!
 * \file xparser2/gen_agent.cpp
 * \author Simon Coakley
 * \date January 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Generator for agent registering.
 */
#include <string>
#include <utility>
#include "flame2/exceptions/base.hpp"
#include "gen_agent.hpp"

namespace xparser { namespace codegen {

GenAgent::GenAgent(const std::string& agent_name)
    : agent_name_(agent_name) {}

// var_type and var_name validation not performed
void GenAgent::AddVar(const std::string& var_type,
                           const std::string& var_name) {
  if (dupe_check_.find(var_name) != dupe_check_.end()) {
    throw flame::exceptions::logic_error("variable already exists");
  }
  vars_.push_back(VarPair(var_type, var_name));  // store type and var
  dupe_check_.insert(var_name);  // remember var name for dupe check
}

void GenAgent::InsertFunc(const GenAgentFunc& generator) {
  // inherit header dependencies
  RequireHeader(generator.GetRequiredHeaders());
  RequireSysHeader(generator.GetRequiredSysHeaders());

  // Create copy of generator
  GenAgentFunc * gencopy = new GenAgentFunc(generator);
  // Set the func agent name
  gencopy->SetAgentName(agent_name_);
  // Store copy of generator
  Insert(gencopy);
}

void GenAgent::Generate(Printer& printer) const {
  // generate agent
  printer.Print("model.addAgent(\"$AGENT$\");\n", "AGENT", agent_name_);
  print_vars_(printer);  // print variables
  GenerateInsertedContent(printer);
}

void GenAgent::print_vars_(Printer& printer) const {
  std::map<std::string, std::string> variables;
  variables["AGENT"] = agent_name_;
  VarPairVector::const_iterator i = vars_.begin();
  for (; i != vars_.end(); ++i) {
    variables["TYPE"] = i->first;
    variables["VAR"] = i->second;
    printer.Print("model.addAgentVariable(\"$AGENT$\", \"$TYPE$\", \"$VAR$\");\n",
            variables);
  }
}

}}  // namespace xparser::codegen
