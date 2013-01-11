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
#include <boost/ptr_container/ptr_vector.hpp>
#include <set>
#include <vector>
#include <utility>
#include <string>
#include <map>
#include "code_generator.hpp"
#include "gen_agentfunc.hpp"
namespace xparser { namespace codegen {

class GenAgent : public CodeGenerator {
  public:
    typedef std::pair<std::string, std::string> VarPair;
    typedef std::vector<VarPair> VarPairVector;
    typedef std::set<std::string> VarnameSet;
    
    explicit GenAgent(const std::string& agent_name);
    void AddVar(const std::string& var_type, const std::string& var_name);
    void Generate(Printer& printer) const;
    
  private:
    // store in vector rather than a map so vars can be output in the same
    // order they were added
    VarPairVector vars_;
    // Keep track of added vars to avoid dups
    VarnameSet dupe_check_;
    std::string agent_name_;

    void print_vars_(Printer& printer) const;
};

}}  // namespace xparser::codegen
#endif  // XPARSER__CODEGEN__GENAGENT_HPP_
