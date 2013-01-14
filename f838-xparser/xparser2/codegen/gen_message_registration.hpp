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

//! Generates code to register a message and register its datatype
class GenMessageRegistration : public CodeGenerator {
  public:
    explicit GenMessageRegistration(const std::string& msg_name);
    //! Adds a message variable and its type
    void AddVar(const std::string& var_type, const std::string& var_name);
    //! Prints the generated code to the printer instance
    void Generate(Printer* printer) const;

  private:
    typedef std::pair<std::string, std::string> VarPair;  //! type-name pair
    typedef std::vector<VarPair> VarPairVector;  //! Collection of vars

    /*! \brief Collection of VarPair
     *
     * A Vector is used rather that a map so variables can be output in the
     * same order they were added
     */
    VarPairVector vars_;
    std::string name_;  //! message name
};
}}  // namespace xparser::codegen
#endif  // XPARSER__CODEGEN__GEN_MESSAGGES_HPP_
