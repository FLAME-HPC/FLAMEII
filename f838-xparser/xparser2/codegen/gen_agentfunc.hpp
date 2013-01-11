/*!
 * \file xparser2/gen_agentfunc.hpp
 * \author Simon Coakley
 * \date January 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Generator for agent function registering.
 */
#ifndef XPARSER__CODEGEN__GEN_AGENTFUNC_HPP_
#define XPARSER__CODEGEN__GEN_AGENTFUNC_HPP_
#include <set>
#include <vector>
#include <utility>
#include <string>
#include <map>
#include "code_generator.hpp"
namespace xparser { namespace codegen {

class GenAgentFunc : public CodeGenerator {
  public:
    explicit GenAgentFunc(const std::string& agent_name, const std::string& func_name,
            const std::string& current_state, const std::string& next_state);
    void AddOutput(const std::string& message_name);
    void AddInput(const std::string& message_name);
    void setMemoryAccessInfoAvailable();
    void AddReadWriteVar(const std::string& var_name);
    void AddReadOnlyVar(const std::string& var_name);
    void Generate(Printer& printer) const;
    
  private:
    std::string agent_name_;
    std::string func_name_;
    std::string current_state_;
    std::string next_state_;
    std::vector<std::string> outputs_;
    std::vector<std::string> inputs_;
    std::vector<std::string> read_write_vars_;
    std::vector<std::string> read_only_vars_;
    bool memory_access_info_available_;

    void print_outputs_(Printer& printer) const;
    void print_inputs_(Printer& printer) const;
    void print_read_write_vars_(Printer& printer) const;
    void print_read_only_vars_(Printer& printer) const;
};

}}  // namespace xparser::codegen
#endif  // XPARSER__CODEGEN__GENAGENTFUNC_HPP_
