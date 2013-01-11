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

void GenAgent::AddFunction(const std::string& func_name,
  const std::string& current_state, const std::string& next_state) {
    std::vector<FuncTuple>::iterator f = funcs_.begin();
    for (; f != funcs_.end(); ++f)
        if ((*f).name == func_name && (*f).current_state == current_state
                && (*f).next_state == next_state)
            throw flame::exceptions::logic_error("function already exists");
    // store func
    funcs_.push_back(FuncTuple(func_name, current_state, next_state));
}

void GenAgent::Generate(Printer& printer) const {
  // generate agent
  printer.Print("model.addAgent(\"$AGENT$\");\n", "AGENT", agent_name_);
  print_vars_(printer);  // print variables
  print_funcs_(printer);  // print functions
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

void GenAgent::print_funcs_(Printer& printer) const {
  std::map<std::string, std::string> variables;
  variables["AGENT"] = agent_name_;
  std::vector<FuncTuple>::const_iterator f = funcs_.begin();
  for (; f != funcs_.end(); ++f) {
    variables["FUNC"] = (*f).name;
    variables["CURRENT"] = (*f).current_state;
    variables["NEXT"] = (*f).next_state;
    printer.Print("model.addAgentFunction(\"$AGENT$\", \"$FUNC$\","
            "\"$CURRENT$\", \"$NEXT$\");\n", variables);
  }
}

}}  // namespace xparser::codegen
