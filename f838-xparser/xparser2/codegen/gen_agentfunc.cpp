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
#include <vector>
#include <map>
#include <utility>
#include <algorithm>
#include "flame2/exceptions/base.hpp"
#include "gen_agentfunc.hpp"

namespace xparser { namespace codegen {

GenAgentFunc::GenAgentFunc(const std::string& agent_name,
    const std::string& func_name,
      const std::string& current_state, const std::string& next_state)
  : agent_name_(agent_name), func_name_(func_name),
    current_state_(current_state),
    next_state_(next_state), memory_access_info_available_(false) {}

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

void GenAgentFunc::AddReadWriteVar(const std::string& var_name) {
  if (std::find(read_write_vars_.begin(), read_write_vars_.end(), var_name)
      != read_write_vars_.end())
        throw flame::exceptions::logic_error("read write var already exists");
  read_write_vars_.push_back(var_name);
}

void GenAgentFunc::AddReadOnlyVar(const std::string& var_name) {
  if (std::find(read_only_vars_.begin(), read_only_vars_.end(), var_name)
      != read_only_vars_.end())
        throw flame::exceptions::logic_error("read only var already exists");
  read_only_vars_.push_back(var_name);
}

void GenAgentFunc::Generate(Printer* printer) const {
  std::map<std::string, std::string> variables;
  variables["AGENT"] = agent_name_;
  variables["FUNC"] = func_name_;
  variables["CURRENT"] = current_state_;
  variables["NEXT"] = next_state_;
  printer->Print("model.addAgentFunction(\"$AGENT$\", \"$FUNC$\","
                 "\"$CURRENT$\", \"$NEXT$\");\n", variables);
  // print outputs and inputs
  print_outputs_(printer);
  print_inputs_(printer);
  // print memory access info
  print_read_write_vars_(printer);
  print_read_only_vars_(printer);
}

void GenAgentFunc::print_outputs_(Printer* printer) const {
  std::map<std::string, std::string> variables;
  variables["AGENT"] = agent_name_;
  variables["FUNC"] = func_name_;
  variables["CURRENT"] = current_state_;
  variables["NEXT"] = next_state_;
  std::vector<std::string>::const_iterator s = outputs_.begin();
  for (; s != outputs_.end(); ++s) {
    variables["MESSAGE"] = (*s);
    printer->Print("model.addAgentFunctionOutput(\"$AGENT$\", \"$FUNC$\","
                   "\"$CURRENT$\", \"$NEXT$\", \"$MESSAGE$\");\n", variables);
  }
}

void GenAgentFunc::print_inputs_(Printer* printer) const {
  std::map<std::string, std::string> variables;
  variables["AGENT"] = agent_name_;
  variables["FUNC"] = func_name_;
  variables["CURRENT"] = current_state_;
  variables["NEXT"] = next_state_;
  std::vector<std::string>::const_iterator s = inputs_.begin();
  for (; s != inputs_.end(); ++s) {
    variables["MESSAGE"] = (*s);
    printer->Print("model.addAgentFunctionInput(\"$AGENT$\", \"$FUNC$\","
                   "\"$CURRENT$\", \"$NEXT$\", \"$MESSAGE$\");\n", variables);
  }
}

void GenAgentFunc::print_read_write_vars_(Printer* printer) const {
  std::map<std::string, std::string> variables;
  variables["AGENT"] = agent_name_;
  variables["FUNC"] = func_name_;
  variables["CURRENT"] = current_state_;
  variables["NEXT"] = next_state_;
  std::vector<std::string>::const_iterator v = read_write_vars_.begin();
  for (; v != read_write_vars_.end(); ++v) {
    variables["VAR"] = (*v);
    printer->Print("model.addAgentFunctionReadWriteVariable"
                   "(\"$AGENT$\", \"$FUNC$\","
                   "\"$CURRENT$\", \"$NEXT$\", \"$VAR$\");\n", variables);
  }
}

void GenAgentFunc::print_read_only_vars_(Printer* printer) const {
  std::map<std::string, std::string> variables;
  variables["AGENT"] = agent_name_;
  variables["FUNC"] = func_name_;
  variables["CURRENT"] = current_state_;
  variables["NEXT"] = next_state_;
  std::vector<std::string>::const_iterator v = read_only_vars_.begin();
  for (; v != read_only_vars_.end(); ++v) {
    variables["VAR"] = (*v);
    printer->Print("model.addAgentFunctionReadOnlyVariable"
                   "(\"$AGENT$\", \"$FUNC$\","
                   "\"$CURRENT$\", \"$NEXT$\", \"$VAR$\");\n", variables);
  }
}

}}  // namespace xparser::codegen
