/*!
 * \file xparser2/code_generator.hpp
 * \author Shawn Chin
 * \date January 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Generator for short code snippets
 */
#ifndef XPARSER__CODEGEN__SNIPPETS_HPP_
#define XPARSER__CODEGEN__SNIPPETS_HPP_
#include <string>
#include <vector>
#include "code_generator.hpp"
namespace xparser { namespace codegen {

// Snippet classes defined in this file
class RegisterAgentFuncSnippets;
class AgentFunctionHeaderSnippets;

// Abstract base class for repeating a code snipper for each var
class SingleVarSnippet : public CodeGenerator {
  public:
    inline void Generate(Printer& printer) const {
      const char* snippet = GetSnippetText();
      std::vector<std::string>::const_iterator i;
      for (i = vars_.begin(); i != vars_.end(); ++i) {
        printer.Print(snippet, "VAR", *i);
      }
    }

    inline void Add(const std::string& var) {
      vars_.push_back(var);
    }

    inline void Add(const std::vector<std::string>& var_vector) {
      vars_.insert(vars_.end(), var_vector.begin(), var_vector.end());
    }
    
  protected:

    // must overload this to provide snippet to generate
    // Use $VAR$ as placeholder for where variables are inserter
    virtual const char* GetSnippetText(void) const = 0;

  private:
    std::vector<std::string> vars_;
};


class RegisterAgentFuncSnippets : public SingleVarSnippet {
  protected:
    const char* GetSnippetText(void) const {
      return "model.registerAgentFunction(\"$VAR$\", &$VAR$);\n";
    }
};


class AgentFunctionHeaderSnippets : public SingleVarSnippet {
  public:
    AgentFunctionHeaderSnippets() {
      RequireHeader("flame2.hpp");
    }
    
  protected:
    const char* GetSnippetText(void) const {
      return "FLAME_AGENT_FUNCTION($VAR$);\n";
    }
};



}}  // namespace xparser::codegen
#endif  // XPARSER__CODEGEN__SNIPPETS_HPP_
