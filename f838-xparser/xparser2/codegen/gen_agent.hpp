/*!
 * \file xparser2/gen_agent.hpp
 * \author Simon Coakley
 * \date January 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Generator for agent registering.
 */
#ifndef XPARSER__CODEGEN__GEN_AGENT_HPP_
#define XPARSER__CODEGEN__GEN_AGENT_HPP_
#include <set>
#include <vector>
#include <utility>
#include <string>
#include <map>
#include "code_generator.hpp"
namespace xparser { namespace codegen {

struct FuncTuple {
    FuncTuple(std::string n, std::string c_s, std::string n_s)
      : name(n), current_state(c_s), next_state(n_s) {}
    std::string name;
    std::string current_state;
    std::string next_state;
};

class GenAgent : public CodeGenerator {
  public:
    typedef std::pair<std::string, std::string> VarPair;
    typedef std::vector<VarPair> VarPairVector;
    typedef std::set<std::string> VarnameSet;
    
    explicit GenAgent(const std::string& agent_name);
    void AddVar(const std::string& var_type, const std::string& var_name);
    void AddFunction(const std::string& func_name,
            const std::string& current_state, const std::string& next_state);
    void Generate(Printer& printer) const;
    
  private:
    // store in vector rather than a map so vars can be output in the same
    // order they were added
    VarPairVector vars_;
    // Keep track of added vars to avoid dups
    VarnameSet dupe_check_;
    std::string agent_name_;
    std::vector<FuncTuple> funcs_;

    void print_vars_(Printer& printer) const;
    void print_funcs_(Printer& printer) const;
};

}}  // namespace xparser::codegen
#endif  // XPARSER__CODEGEN__GENAGENT_HPP_
