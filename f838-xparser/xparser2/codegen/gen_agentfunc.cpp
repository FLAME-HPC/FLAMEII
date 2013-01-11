/*!
 * \file xparser2/gen_agentfunc.cpp
 * \author Simon Coakley
 * \date January 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Generator for agent function registering.
 */
#include <string>
#include <utility>
#include <algorithm>
#include "flame2/exceptions/base.hpp"
#include "gen_agentfunc.hpp"

namespace xparser { namespace codegen {

GenAgentFunc::GenAgentFunc(const std::string& func_name,
      const std::string& current_state, const std::string& next_state)
  : func_name_(func_name), current_state_(current_state),
    next_state_(next_state) {}

void GenAgentFunc::SetAgentName(const std::string& agent_name) {
  agent_name_ = agent_name;
}

void GenAgentFunc::AddOutput(const std::string& message_name) {
  if (std::find(outputs_.begin(), outputs_.end(), message_name)
    != outputs_.end())
      throw flame::exceptions::logic_error("output already exists");
  outputs_.push_back(message_name);
}

void GenAgentFunc::AddInput(const std::string& message_name) {
  if (std::find(inputs_.begin(), inputs_.end(), message_name)
    != inputs_.end())
      throw flame::exceptions::logic_error("input already exists");
  inputs_.push_back(message_name);
}

void GenAgentFunc::Generate(Printer& printer) const {
  std::map<std::string, std::string> variables;
  variables["AGENT"] = agent_name_;
  variables["FUNC"] = func_name_;
  variables["CURRENT"] = current_state_;
  variables["NEXT"] = next_state_;
  printer.Print("model.addAgentFunction(\"$AGENT$\", \"$FUNC$\","
            "\"$CURRENT$\", \"$NEXT$\");\n", variables);
  // print outputs and inputs
  print_outputs_(printer);
  print_inputs_(printer);
}

void GenAgentFunc::print_outputs_(Printer& printer) const {
  std::map<std::string, std::string> variables;
  variables["AGENT"] = agent_name_;
  variables["FUNC"] = func_name_;
  variables["CURRENT"] = current_state_;
  variables["NEXT"] = next_state_;
  std::vector<std::string>::const_iterator s = outputs_.begin();
  for (; s != outputs_.end(); ++s) {
    variables["MESSAGE"] = (*s);
    printer.Print("model.addAgentFunctionOutput(\"$AGENT$\", \"$FUNC$\","
            "\"$CURRENT$\", \"$NEXT$\", \"$MESSAGE$\");\n", variables);
  }
}

void GenAgentFunc::print_inputs_(Printer& printer) const {
  std::map<std::string, std::string> variables;
  variables["AGENT"] = agent_name_;
  variables["FUNC"] = func_name_;
  variables["CURRENT"] = current_state_;
  variables["NEXT"] = next_state_;
  std::vector<std::string>::const_iterator s = inputs_.begin();
  for (; s != inputs_.end(); ++s) {
    variables["MESSAGE"] = (*s);
    printer.Print("model.addAgentFunctionInput(\"$AGENT$\", \"$FUNC$\","
            "\"$CURRENT$\", \"$NEXT$\", \"$MESSAGE$\");\n", variables);
  }
}

}}  // namespace xparser::codegen
