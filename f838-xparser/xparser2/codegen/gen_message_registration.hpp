/*!
 * \file xparser2/gen_message_registration.hpp
 * \author Shawn Chin
 * \date January 2013
 * \copyright Copyright (c) 2013 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2013 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Generate code to define and register messages
 */
#ifndef XPARSER__CODEGEN__GEN_MESSAGGES_HPP_
#define XPARSER__CODEGEN__GEN_MESSAGGES_HPP_
#include <vector>
#include <utility>
#include <string>
#include "code_generator.hpp"
namespace xparser { namespace codegen {
  
class GenMessageRegistration : public CodeGenerator {
public:
    explicit GenMessageRegistration(const std::string& msg_name);
    void AddVar(const std::string& var_type, const std::string& var_name);
    void Generate(Printer& printer) const;
    
  private:
    typedef std::pair<std::string, std::string> VarPair;
    typedef std::vector<VarPair> VarPairVector;

    std::string name_;
    // store in vector rather than a map so vars can be output in the same
    // order they were added
    VarPairVector vars_;
};
}}  // namespace xparser::codegen
#endif  // XPARSER__CODEGEN__GEN_MESSAGGES_HPP_
