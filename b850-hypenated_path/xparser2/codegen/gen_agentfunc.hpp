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

//! \brief Generates code to register an agent function, its inputs, outputs,
//! and memory access
class GenAgentFunc : public CodeGenerator {
  public:
    explicit GenAgentFunc(const std::string& agent_name,
        const std::string& func_name,
            const std::string& current_state, const std::string& next_state);
    //! Adds an output
    void AddOutput(const std::string& message_name);
    //! Adds an input
    void AddInput(const std::string& message_name);
    //! Adds a variable read write memory access
    void AddReadWriteVar(const std::string& var_name);
    //! Adds a variable read only memory access
    void AddReadOnlyVar(const std::string& var_name);
    //! Prints the generated code to the printer instance
    void Generate(Printer* printer) const;

  private:
    std::string agent_name_;  //!< Agent name
    std::string func_name_;  //!< Function name
    std::string current_state_;  //!< Function current state
    std::string next_state_;  //!< Function next state
    std::vector<std::string> outputs_;  //!< Output messages
    std::vector<std::string> inputs_;  //!< Input messages
    //! Read write variable memory access
    std::vector<std::string> read_write_vars_;
    //! Read only variable memory access
    std::vector<std::string> read_only_vars_;

    //! Prints outputs registration code
    void print_outputs_(Printer* printer) const;
    //! Prints inputs registration code
    void print_inputs_(Printer* printer) const;
    //! Prints read write memory access registration code
    void print_read_write_vars_(Printer* printer) const;
    //! Prints read only memory access registration code
    void print_read_only_vars_(Printer* printer) const;
};

}}  // namespace xparser::codegen
#endif  // XPARSER__CODEGEN__GENAGENTFUNC_HPP_
