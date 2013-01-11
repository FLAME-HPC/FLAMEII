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

void GenAgentFunc::Generate(Printer& printer) const {
  std::map<std::string, std::string> variables;
  variables["AGENT"] = agent_name_;
  variables["FUNC"] = func_name_;
  variables["CURRENT"] = current_state_;
  variables["NEXT"] = next_state_;
  printer.Print("model.addAgentFunction(\"$AGENT$\", \"$FUNC$\","
            "\"$CURRENT$\", \"$NEXT$\");\n", variables);
}

}}  // namespace xparser::codegen
