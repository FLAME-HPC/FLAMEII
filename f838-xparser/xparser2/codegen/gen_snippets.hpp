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
#include "code_generator.hpp"
namespace xparser { namespace codegen {

// class defined in this file
class GenAgentFunctionHeader;
class GenMessageRegistration;


class GenAgentFunctionHeader : public CodeGenerator {
  public:
    GenAgentFunctionHeader(const std::string& func_name) : func_(func_name) {
      RequireHeader("flame2.hpp");
    }
         
    inline void Generate(Printer& printer) const {
      printer.Print("FLAME_AGENT_FUNCTION($FUNC$);\n", "FUNC", func_);
    }

  private:
    std::string func_;
};

class GenMessageRegistration : public CodeGenerator {
  public:
    GenMessageRegistration(const std::string& msg) : msg_(msg) {}
         
    inline void Generate(Printer& printer) const {
      printer.Print("model.registerMessageType<$MSG$_message>(\"$MSG$\");\n",
                    "MSG", msg_);
    }

  private:
    std::string msg_;
};

}}  // namespace xparser::codegen
#endif  // XPARSER__CODEGEN__SNIPPETS_HPP_
